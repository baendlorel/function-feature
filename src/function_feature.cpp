#include <node.h>
#include <v8-exception.h>
#include <v8-isolate.h>
#include <v8.h>

namespace function_feature {
typedef v8::Local<v8::Context> LCtx;
typedef v8::Local<v8::Function> LFun;
typedef v8::Local<v8::String> LStr;
typedef v8::MaybeLocal<v8::String> MLStr;
typedef v8::Local<v8::Object> LObj;
typedef v8::Isolate* v8Iso;
typedef v8::FunctionCallback FnCB;

LStr ToKey(v8Iso isolate, const char* k) {
  auto strType = v8::NewStringType::kNormal;
  MLStr maybe_key = v8::String::NewFromUtf8(isolate, k, strType);
  LStr key;
  if (!maybe_key.ToLocal(&key)) {
    return key;  // Return an empty Local<String> if conversion fails
  }
  return key;
}

void SetBool(LObj result, const char* k, bool v) {
  v8Iso isolate = result->GetIsolate();
  LCtx ctx = isolate->GetCurrentContext();
  LStr key = ToKey(isolate, k);

  v8::Local<v8::Boolean> value = v8::Boolean::New(isolate, v);
  auto maybe_result = result->Set(ctx, key, value);
  maybe_result.Check();
}

void SetFn(LObj result, const char* k, FnCB fn) {
  v8Iso isolate = result->GetIsolate();
  LCtx ctx = isolate->GetCurrentContext();
  LStr key = ToKey(isolate, k);

  v8::Local<v8::FunctionTemplate> tpl = v8::FunctionTemplate::New(isolate, fn);
  v8::Local<v8::Function> value =
      tpl->GetFunction(isolate->GetCurrentContext()).ToLocalChecked();

  auto maybe_result = result->Set(ctx, key, value);
  maybe_result.Check();
}

// Get V8 function feature flags
LObj GetFeatures(LFun fn, v8Iso isolate) {
  v8::EscapableHandleScope scope(isolate);
  LCtx ctx = isolate->GetCurrentContext();
  LObj result = v8::Object::New(isolate);

  SetBool(result, "isConstructor", fn->IsConstructor());
  SetBool(result, "isAsyncFunction", fn->IsAsyncFunction());
  SetBool(result, "isGeneratorFunction", fn->IsGeneratorFunction());
  SetBool(result, "isProxy", fn->IsProxy());
  SetBool(result, "isCallable", fn->IsCallable());
  SetBool(result, "isBound", fn->GetBoundFunction()->IsFunction());

  return scope.Escape(result);
}

void Throws(v8::FunctionCallbackInfo<v8::Value> info, const char* msg) {
  info.GetIsolate()->ThrowException(v8::Exception::TypeError(
      v8::String::NewFromUtf8(info.GetIsolate(), msg,
                              v8::NewStringType::kNormal)
          .ToLocalChecked()));
}

void GetFunctionFeatures(const v8::FunctionCallbackInfo<v8::Value>& info) {
  if (info.Length() != 1) {
    Throws(info, "Expected exactly 1 argument");
    return;
  }

  if (!info[0]->IsFunction()) {
    Throws(info, "Argument must be a function");
    return;
  }

  LFun func = LFun::Cast(info[0]);
  v8Iso isolate = info.GetIsolate();

  LObj result = GetFeatures(func, isolate);
  info.GetReturnValue().Set(result);
}

void GetBoundFunction(const v8::FunctionCallbackInfo<v8::Value>& info) {
  if (info.Length() != 1) {
    Throws(info, "Expected exactly 1 argument");
    return;
  }

  if (!info[0]->IsFunction()) {
    Throws(info, "Argument must be a function");
    return;
  }

  LFun func = LFun::Cast(info[0]);

  v8::Local<v8::Value> bound = func->GetBoundFunction();
  info.GetReturnValue().Set(bound);
}

void SetFunctionName(const v8::FunctionCallbackInfo<v8::Value>& info) {
  if (info.Length() != 2) {
    Throws(info, "Expected 2 arguments: function, name");
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
  LFun fn = LFun::Cast(info[0]);
  v8Iso isolate = info.GetIsolate();
  LStr name = info[1].As<v8::String>();
  fn->SetName(name);
  info.GetReturnValue().Set(fn);
}

void Init(v8::Local<v8::Object> target) {
  SetFn(target, "getFunctionFeatures", GetFunctionFeatures);
  SetFn(target, "getBoundFunction", GetBoundFunction);
  SetFn(target, "setFunctionName", SetFunctionName);
}

NODE_MODULE(function_feature, Init)

}  // namespace function_feature
