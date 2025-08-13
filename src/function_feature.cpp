#include <nan.h>
#include <v8-exception.h>
#include <v8-isolate.h>
#include <v8.h>
using namespace Nan;

namespace function_feature {
typedef v8::Local<v8::Context> LCtx;
typedef v8::Local<v8::Function> LFun;
typedef v8::Local<v8::String> LStr;
typedef v8::Local<v8::Object> LObj;
typedef v8::Isolate* v8Iso;

void Set(v8Iso isolate, LObj result, LCtx context, const char* k, bool v) {
  v8::HandleScope scope(isolate);

  v8::MaybeLocal<v8::String> maybe_key =
      v8::String::NewFromUtf8(isolate, k, v8::NewStringType::kNormal);
  v8::Local<v8::String> key;
  if (!maybe_key.ToLocal(&key)) {
    return;
  }

  v8::Local<v8::Boolean> value = v8::Boolean::New(isolate, v);
  auto maybe_result = result->Set(context, key, value);
  maybe_result.Check();
}

// Get V8 function feature flags
LObj GetFeatures(LFun fn, v8Iso isolate) {
  v8::EscapableHandleScope scope(isolate);
  LCtx ctx = isolate->GetCurrentContext();
  LObj result = v8::Object::New(isolate);

  Set(isolate, result, ctx, "isConstructor", fn->IsConstructor());
  Set(isolate, result, ctx, "isAsyncFunction", fn->IsAsyncFunction());
  Set(isolate, result, ctx, "isGeneratorFunction", fn->IsGeneratorFunction());
  Set(isolate, result, ctx, "isProxy", fn->IsProxy());
  Set(isolate, result, ctx, "isCallable", fn->IsCallable());

  bool isBound = false;
  v8::Local<v8::Value> boundFn = fn->GetBoundFunction();
  isBound = !boundFn->IsUndefined();
  Set(isolate, result, ctx, "isBound", isBound);

  return scope.Escape(result);
}

void Throws(const FunctionCallbackInfo<v8::Value>& info, const char* msg) {
  info.GetIsolate()->ThrowException(v8::Exception::TypeError(
      v8::String::NewFromUtf8(info.GetIsolate(), msg,
                              v8::NewStringType::kNormal)
          .ToLocalChecked()));
}

void GetFunctionFeatures(const FunctionCallbackInfo<v8::Value>& info) {
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

void GetBoundFunction(const FunctionCallbackInfo<v8::Value>& info) {
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

void SetFunctionName(const FunctionCallbackInfo<v8::Value>& info) {
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

// Module initialization
NAN_MODULE_INIT(Init) {
  Nan::Set(target, Nan::New("getFunctionFeatures").ToLocalChecked(),
           Nan::GetFunction(Nan::New<v8::FunctionTemplate>(GetFunctionFeatures))
               .ToLocalChecked());
  Nan::Set(target, Nan::New("getBoundFunction").ToLocalChecked(),
           Nan::GetFunction(Nan::New<v8::FunctionTemplate>(GetBoundFunction))
               .ToLocalChecked());
  Nan::Set(target, Nan::New("setFunctionName").ToLocalChecked(),
           Nan::GetFunction(Nan::New<v8::FunctionTemplate>(SetFunctionName))
               .ToLocalChecked());
}

NODE_MODULE(function_feature, Init)

}  // namespace function_feature
