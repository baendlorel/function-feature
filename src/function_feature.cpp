#include <nan.h>
#include <v8-function.h>
#include <v8.h>

using namespace Nan;

namespace function_feature {
typedef v8::Local<v8::Context> LCtx;
typedef v8::Local<v8::Function> LFun;
typedef v8::Local<v8::String> LStr;
typedef v8::Local<v8::Object> LObj;
typedef v8::Isolate* v8Iso;

void Set(v8Iso isolate, LObj result, LCtx context, const char* k, bool v) {
  auto raw = v8::String::NewFromUtf8(isolate, k, v8::NewStringType::kNormal);
  LStr key = raw.ToLocalChecked();

  v8::Local<v8::Boolean> value = v8::Boolean::New(isolate, k);

  result->Set(context, key, value).Check();
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
  Set(isolate, result, ctx, "isBound", fn->GetBoundFunction()->IsFunction());

  return scope.Escape(result);
}

NAN_METHOD(GetFunctionFeatures) {
  if (info.Length() != 1) {
    Nan::ThrowTypeError("Expected exactly 1 argument");
    return;
  }

  if (!info[0]->IsFunction()) {
    Nan::ThrowTypeError("Argument must be a function");
    return;
  }

  LFun func = LFun::Cast(info[0]);
  v8Iso isolate = info.GetIsolate();

  LObj result = GetFeatures(func, isolate);
  info.GetReturnValue().Set(result);
}

NAN_METHOD(GetBoundFunction) {
  if (info.Length() != 1) {
    Nan::ThrowTypeError("Expected exactly 1 argument");
    return;
  }

  if (!info[0]->IsFunction()) {
    Nan::ThrowTypeError("Argument must be a function");
    return;
  }

  LFun func = LFun::Cast(info[0]);
  v8::Local<v8::Value> bound = func->GetBoundFunction();
  info.GetReturnValue().Set(bound);
}

// NAN method: setFunctionName
NAN_METHOD(SetFunctionName) {
  if (info.Length() != 2) {
    Nan::ThrowTypeError("Expected 2 arguments: function, name");
    return;
  }
  if (!info[0]->IsFunction()) {
    Nan::ThrowTypeError("First argument must be a function");
    return;
  }
  if (!info[1]->IsString()) {
    Nan::ThrowTypeError("Second argument must be a string");
    return;
  }
  LFun func = LFun::Cast(info[0]);
  v8Iso isolate = info.GetIsolate();
  LStr name = info[1].As<v8::String>();
  func->SetName(name);
  info.GetReturnValue().Set(func);
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
