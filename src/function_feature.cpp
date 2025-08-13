#include <node.h>
#include <v8.h>

namespace function_feature {
typedef v8::Local<v8::Context> LCtx;
typedef v8::Local<v8::Function> LFun;
typedef v8::Local<v8::String> LStr;
typedef v8::MaybeLocal<v8::String> MStr;
typedef v8::Local<v8::Object> LObj;
typedef v8::Isolate* Isol;
typedef v8::FunctionCallback FnCB;
typedef v8::Local<v8::Value> LVal;

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

  return scope.Escape(result);
}

void Throws(v8::FunctionCallbackInfo<v8::Value> info, const char* msg) {
  info.GetIsolate()->ThrowException(v8::Exception::TypeError(
      v8::String::NewFromUtf8(info.GetIsolate(), msg,
                              v8::NewStringType::kNormal)
          .ToLocalChecked()));
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
  LStr str = fn->FunctionProtoToString(ctx).ToLocalChecked();
  info.GetReturnValue().Set(str);
}
// #endregion

void Init(v8::Local<v8::Object> target) {
  _SetFn(target, "getFeatures", GetFeatures);
  _SetFn(target, "getBound", GetBound);
  _SetFn(target, "getBoundOrigin", GetBoundOrigin);
  _SetFn(target, "setName", SetName);
  _SetFn(target, "protoToString", ProtoToString);
}

NODE_MODULE(function_feature, Init)

}  // namespace function_feature
