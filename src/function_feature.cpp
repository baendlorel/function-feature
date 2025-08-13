#include <node.h>
#include <v8.h>
#include <string>

namespace function_feature {
typedef v8::Local<v8::Context> LCtx;
typedef v8::Local<v8::Function> LFun;
typedef v8::Local<v8::String> LStr;
typedef v8::MaybeLocal<v8::String> MStr;
typedef v8::Local<v8::Object> LObj;
typedef v8::Isolate* Isol;
typedef v8::FunctionCallback FnCB;
typedef v8::Local<v8::Value> LVal;

void Throws(v8::FunctionCallbackInfo<v8::Value> info, const char* msg) {
  info.GetIsolate()->ThrowException(v8::Exception::TypeError(
      v8::String::NewFromUtf8(info.GetIsolate(), msg,
                              v8::NewStringType::kNormal)
          .ToLocalChecked()));
}

LStr _ToKey(Isol isolate, const char* k) {
  auto strType = v8::NewStringType::kNormal;
  MStr maybe_key = v8::String::NewFromUtf8(isolate, k, strType);
  LStr key;
  if (!maybe_key.ToLocal(&key)) {
    return key;  // Return an empty Local<String> if conversion fails
  }
  return key;
}

void _SetBool(LObj result, const char* k, bool v) {
  Isol isolate = result->GetIsolate();
  LCtx ctx = isolate->GetCurrentContext();
  LStr key = _ToKey(isolate, k);

  v8::Local<v8::Boolean> value = v8::Boolean::New(isolate, v);
  auto maybe_result = result->Set(ctx, LVal::Cast(key), value);
  maybe_result.Check();
}

void _SetFn(LObj result, const char* k, FnCB fn) {
  Isol isolate = result->GetIsolate();
  LCtx ctx = isolate->GetCurrentContext();
  LStr key = _ToKey(isolate, k);

  v8::Local<v8::FunctionTemplate> tpl = v8::FunctionTemplate::New(isolate, fn);
  v8::Local<v8::Function> value =
      tpl->GetFunction(isolate->GetCurrentContext()).ToLocalChecked();

  auto maybe_result = result->Set(ctx, LVal::Cast(key), value);
  maybe_result.Check();
}

LFun _GetBoundOrigin(LFun func) {
  LFun current = func;
  while (true) {
    LVal bound = current->GetBoundFunction();
    if (bound->IsUndefined()) {
      break;
    }

    LFun next = LFun::Cast(bound);
    // & prevent inpredictable infinite loop
    if (next == current) {
      break;
    }
    current = next;
  }
  return current;
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

// Check if function string starts with 'class ' or '[class '
bool _HasClassSyntax(const std::string& func_str) {
  if (func_str.length() < 6)
    return false;

  // Check for 'class ' at the beginning
  if (func_str.substr(0, 6) == "class") {
    // Find the opening brace
    size_t brace_pos = func_str.find('{');
    if (brace_pos != std::string::npos) {
      // Check if there's no parenthesis before the brace
      size_t paren_pos = func_str.find('(');
      return paren_pos == std::string::npos || paren_pos > brace_pos;
    }
  }

  // Check for '[class ' at the beginning (native class toString)
  if (func_str.length() >= 7 && func_str.substr(0, 7) == "[class") {
    return true;
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
  LFun origin = _GetBoundOrigin(func);

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

void GetBoundOrigin(const v8::FunctionCallbackInfo<v8::Value>& info) {
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

  LFun origin = _GetBoundOrigin(func);
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
  _SetFn(target, "getBoundOrigin", GetBoundOrigin);
  _SetFn(target, "setName", SetName);
  _SetFn(target, "protoToString", ProtoToString);
  _SetFn(target, "isClass", IsClass);
}

NODE_MODULE(function_feature, Init)

}  // namespace function_feature
