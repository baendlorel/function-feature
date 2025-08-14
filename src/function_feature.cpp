#include <node.h>
#include <v8.h>
#include <string>

/**
 * Check if `{` appears before `(` in a function string
 *
 * - ignores comment
 * @param src function string
 */
bool _HasBraceBeforeParen(const std::string& src) {
  size_t i = 4, len = src.length();
  while (i < len) {
    // & skip comment
    // skip /* ... */
    if (i + 1 < len && src[i] == '/' && src[i + 1] == '*') {
      i += 2;
      while (i + 1 < len && !(src[i] == '*' && src[i + 1] == '/')) {
        i++;
      }
      i += 2;  // skip */
    }
    // skip // ...
    else if (i + 1 < len && src[i] == '/' && src[i + 1] == '/') {
      i += 2;
      while (i < len && src[i] != '\n') {
        i++;
      }
      // keep the newline character
      if (i < len && src[i] == '\n') {
        i++;
      }
    }
    // & not comment
    else {
      if (src[i] == '{') {
        return true;
      }
      if (src[i] == '(') {
        return false;
      }
      i++;
    }
  }
  return false;
}

namespace function_feature {
typedef v8::Isolate* Isol;
typedef v8::FunctionCallback FnCB;

// Local handle types
typedef v8::Local<v8::Context> LCtx;
typedef v8::Local<v8::Value> LVal;
typedef v8::Local<v8::Proxy> LPxy;
typedef v8::Local<v8::Function> LFun;
typedef v8::Local<v8::String> LStr;
typedef v8::MaybeLocal<v8::String> MStr;
typedef v8::Local<v8::Object> LObj;
constexpr auto STR_TYPE = v8::NewStringType::kNormal;

void Throws(v8::FunctionCallbackInfo<v8::Value> info, const char* msg) {
  Isol isolate = info.GetIsolate();
  MStr maybe_msg = v8::String::NewFromUtf8(info.GetIsolate(), msg, STR_TYPE);
  LVal err = v8::Exception::TypeError(maybe_msg.ToLocalChecked());
  isolate->ThrowException(err);
}

LVal _ToKey(Isol isolate, const char* k) {
  MStr maybe_key = v8::String::NewFromUtf8(isolate, k, STR_TYPE);
  LStr key;
  if (!maybe_key.ToLocal(&key)) {
    return key;  // Return an empty Local<String> if conversion fails
  }
  return LVal::Cast(key);
}

void _SetVal(LObj result, const char* k, bool v) {}

void _SetBool(LObj result, const char* k, bool v) {
  Isol isolate = result->GetIsolate();
  LCtx ctx = isolate->GetCurrentContext();
  LVal key = _ToKey(isolate, k);

  auto value = v8::Boolean::New(isolate, v);
  auto maybe_result = result->Set(ctx, key, value);
  maybe_result.Check();
}

void _SetFn(LObj result, const char* k, FnCB fn) {
  Isol isolate = result->GetIsolate();
  LCtx ctx = isolate->GetCurrentContext();
  LVal key = _ToKey(isolate, k);

  auto tpl = v8::FunctionTemplate::New(isolate, fn);
  LFun value = tpl->GetFunction(isolate->GetCurrentContext()).ToLocalChecked();

  auto maybe_result = result->Set(ctx, key, value);
  maybe_result.Check();
}

LFun _GetOrigin(Isol isolate, LFun func) {
  LFun current = func;

  while (true) {
    bool changed = false;

    // First check bound target
    LVal bound = current->GetBoundFunction();
    if (bound->IsFunction()) {
      LFun boundFunc = LFun::Cast(bound);
      if (boundFunc != current) {
        current = boundFunc;
        changed = true;
        continue;
      }
    }

    // Then check proxy target
    if (current->IsProxy()) {
      LPxy proxy = LPxy::Cast(current);
      LVal target = proxy->GetTarget();
      if (target->IsFunction()) {
        LFun targetFunc = LFun::Cast(target);
        if (targetFunc != current) {
          current = targetFunc;
          changed = true;
          continue;
        }
      }
    }

    // If no change occurred, we've reached the origin
    if (!changed) {
      break;
    }
  }

  return current;
}

LFun _GetProxyTarget(Isol isolate, LVal o) {
  if (o->IsProxy()) {
    LPxy proxy = LPxy::Cast(o);
    return proxy->GetTarget();  // & proxied target must not be non-object
  }
  return v8::Undefined(isolate);
}

// Check if a function is a native constructor like Array, Boolean, etc.
bool _IsNativeConstructor(LFun func, Isol isolate) {
  LCtx ctx = isolate->GetCurrentContext();

  // Get the global object to access native constructors
  LObj global = ctx->Global();

  // List of native constructor names to check
  const char* native_constructors[] = {
      "Array",       "Boolean",      "Date",          "Error",
      "Function",    "Number",       "Object",        "RegExp",
      "String",      "Symbol",       "Promise",       "Map",
      "Set",         "WeakMap",      "WeakSet",       "ArrayBuffer",
      "DataView",    "Float32Array", "Float64Array",  "Int8Array",
      "Int16Array",  "Int32Array",   "Uint8Array",    "Uint8ClampedArray",
      "Uint16Array", "Uint32Array",  "BigInt64Array", "BigUint64Array"};

  for (const char* name : native_constructors) {
    LStr key = _ToKey(isolate, name);
    v8::MaybeLocal<v8::Value> maybe_constructor = global->Get(ctx, key);
    LVal constructor_val;
    if (maybe_constructor.ToLocal(&constructor_val) &&
        constructor_val->IsFunction()) {
      LFun constructor = LFun::Cast(constructor_val);
      if (constructor == func) {
        return true;
      }
    }
  }

  return false;
}

// Check if function string starts with 'class' or '[class'
bool _HasClassSyntax(const std::string& func_str) {
  // minimum length for a class definition is 'class{}'
  if (func_str.length() < 7) {
    return false;
  }

  // Check for 'class' at the beginning
  if (func_str.substr(0, 5) == "class" || func_str.substr(0, 6) == "[class") {
    // 跳过注释，判断在未遇到圆括号时先遇到大括号
    return _HasBraceBeforeParen(func_str);
  }

  return false;
}

// Main function to check if a function is a class
bool _IsClass(LFun func, Isol isolate) {
  // Step 0: Must be a constructor
  if (!func->IsConstructor()) {
    return false;
  }

  // Get the original function (not bound)
  LFun origin = _GetOrigin(isolate, func);

  // Step 1: Check if it's a native constructor
  if (_IsNativeConstructor(origin, isolate)) {
    return true;
  }

  // Step 2: Check function.toString() for class syntax
  LCtx ctx = isolate->GetCurrentContext();
  MStr maybe_str = origin->FunctionProtoToString(ctx);
  LStr str;
  if (!maybe_str.ToLocal(&str)) {
    return false;
  }

  // Convert V8 string to std::string
  v8::String::Utf8Value utf8_str(isolate, str);
  std::string func_str(*utf8_str);

  return _HasClassSyntax(func_str);
}

// Get V8 function feature flags
LObj _GetFeatures(LFun fn, Isol isolate) {
  v8::EscapableHandleScope scope(isolate);
  LCtx ctx = isolate->GetCurrentContext();
  LObj result = v8::Object::New(isolate);

  _SetBool(result, "isConstructor", fn->IsConstructor());
  _SetBool(result, "isAsyncFunction", fn->IsAsyncFunction());
  _SetBool(result, "isGeneratorFunction", fn->IsGeneratorFunction());
  _SetBool(result, "isProxy", fn->IsProxy());
  _SetBool(result, "isCallable", fn->IsCallable());
  _SetBool(result, "isBound", fn->GetBoundFunction()->IsFunction());
  _SetBool(result, "isClass", _IsClass(fn, isolate));
  _SetBool(result, "origin", _GetOrigin(fn, isolate));

  return scope.Escape(result);
}

// #region Exported Functions
void GetFeatures(const v8::FunctionCallbackInfo<v8::Value>& info) {
  if (info.Length() < 1) {
    Throws(info, "Expected at least 1 argument");
    return;
  }

  if (!info[0]->IsFunction()) {
    Throws(info, "Argument must be a function");
    return;
  }

  LVal arg0 = info[0];
  LFun func = LFun::Cast(arg0);
  Isol isolate = info.GetIsolate();

  LObj result = _GetFeatures(func, isolate);
  info.GetReturnValue().Set(result);
}

void GetBound(const v8::FunctionCallbackInfo<v8::Value>& info) {
  if (info.Length() < 1) {
    Throws(info, "Expected at least 1 argument");
    return;
  }

  if (!info[0]->IsFunction()) {
    Throws(info, "Argument must be a function");
    return;
  }

  LVal arg0 = info[0];
  LFun func = LFun::Cast(arg0);

  LVal bound = func->GetBoundFunction();
  info.GetReturnValue().Set(bound);
}

void GetProxyTarget(const v8::FunctionCallbackInfo<v8::Value>& info) {
  if (info.Length() < 1) {
    Throws(info, "Expected at least 1 argument");
    return;
  }

  LVal arg0 = info[0];
  if (!arg0->IsObject() && !arg0->IsFunction()) {
    Throws(info, "Argument must be a function or an object");
    return;
  }

  LFun func = LFun::Cast(arg0);
  Isol isolate = info.GetIsolate();

  LVal proxyTarget = _GetProxyTarget(isolate, func);
  info.GetReturnValue().Set(proxyTarget);
}

void GetOrigin(const v8::FunctionCallbackInfo<v8::Value>& info) {
  if (info.Length() < 1) {
    Throws(info, "Expected at least 1 argument");
    return;
  }

  if (!info[0]->IsFunction()) {
    Throws(info, "Argument must be a function");
    return;
  }

  LVal arg0 = info[0];
  LFun func = LFun::Cast(arg0);
  Isol isolate = info.GetIsolate();

  LFun origin = _GetOrigin(isolate, func);
  info.GetReturnValue().Set(origin);
}

void SetName(const v8::FunctionCallbackInfo<v8::Value>& info) {
  if (info.Length() < 2) {
    Throws(info, "Expected at least 2 arguments: function, name");
    return;
  }
  if (!info[0]->IsFunction()) {
    Throws(info, "First argument must be a function");
    return;
  }
  if (!info[1]->IsString()) {
    Throws(info, "Second argument must be a string");
    return;
  }
  LVal arg0 = info[0];
  LFun fn = LFun::Cast(arg0);
  Isol isolate = info.GetIsolate();
  LStr name = info[1].As<v8::String>();

  // Check if this is a native constructor before trying to set name
  if (_IsNativeConstructor(fn, isolate)) {
    Throws(info, "Cannot set name on native constructor");
    return;
  }

  fn->SetName(name);
  info.GetReturnValue().Set(fn);
}

void ProtoToString(const v8::FunctionCallbackInfo<v8::Value>& info) {
  if (info.Length() < 1) {
    Throws(info, "Expected at least 1 argument");
    return;
  }

  if (!info[0]->IsFunction()) {
    Throws(info, "Argument must be a function");
    return;
  }
  LVal arg0 = info[0];
  LFun fn = LFun::Cast(arg0);
  Isol isolate = info.GetIsolate();
  LCtx ctx = isolate->GetCurrentContext();
  v8::Local<v8::String> str = fn->FunctionProtoToString(ctx).ToLocalChecked();
  info.GetReturnValue().Set(str);
}

void IsClass(const v8::FunctionCallbackInfo<v8::Value>& info) {
  if (info.Length() < 1) {
    Throws(info, "Expected at least 1 argument");
    return;
  }

  if (!info[0]->IsFunction()) {
    Throws(info, "Argument must be a function");
    return;
  }

  LVal arg0 = info[0];
  LFun func = LFun::Cast(arg0);
  Isol isolate = info.GetIsolate();

  bool is_class = _IsClass(func, isolate);
  info.GetReturnValue().Set(v8::Boolean::New(isolate, is_class));
}
// #endregion

void Init(v8::Local<v8::Object> target) {
  _SetFn(target, "getFeatures", GetFeatures);
  _SetFn(target, "getBound", GetBound);
  _SetFn(target, "getOrigin", GetOrigin);
  _SetFn(target, "setName", SetName);
  _SetFn(target, "protoToString", ProtoToString);
  _SetFn(target, "isClass", IsClass);
}

NODE_MODULE(function_feature, Init)

}  // namespace function_feature
