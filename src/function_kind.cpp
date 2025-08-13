#include <nan.h>
#include <v8.h>

using namespace Nan;

namespace function_kind {

// Get V8 function type flags
v8::Local<v8::Object> GetFunctionKind(v8::Local<v8::Function> func,
                                      v8::Isolate* isolate) {
  v8::EscapableHandleScope scope(isolate);
  v8::Local<v8::Context> context = isolate->GetCurrentContext();
  v8::Local<v8::Object> result = v8::Object::New(isolate);

  // IsConstructor flag
  v8::Local<v8::String> is_constructor_key =
      v8::String::NewFromUtf8(isolate, "isConstructor",
                              v8::NewStringType::kNormal)
          .ToLocalChecked();
  v8::Local<v8::Boolean> is_constructor_value =
      v8::Boolean::New(isolate, func->IsConstructor());
  result->Set(context, is_constructor_key, is_constructor_value).Check();

  // IsAsyncFunction flag
  v8::Local<v8::String> is_async_key =
      v8::String::NewFromUtf8(isolate, "isAsyncFunction",
                              v8::NewStringType::kNormal)
          .ToLocalChecked();
  v8::Local<v8::Boolean> is_async_value =
      v8::Boolean::New(isolate, func->IsAsyncFunction());
  result->Set(context, is_async_key, is_async_value).Check();

  // IsGeneratorFunction flag
  v8::Local<v8::String> is_generator_key =
      v8::String::NewFromUtf8(isolate, "isGeneratorFunction",
                              v8::NewStringType::kNormal)
          .ToLocalChecked();
  v8::Local<v8::Boolean> is_generator_value =
      v8::Boolean::New(isolate, func->IsGeneratorFunction());
  result->Set(context, is_generator_key, is_generator_value).Check();

  return scope.Escape(result);
}

// NAN method: getFunctionKind
NAN_METHOD(GetFunctionKind) {
  if (info.Length() != 1) {
    Nan::ThrowTypeError("Expected exactly 1 argument");
    return;
  }

  if (!info[0]->IsFunction()) {
    Nan::ThrowTypeError("Argument must be a function");
    return;
  }

  v8::Local<v8::Function> func = v8::Local<v8::Function>::Cast(info[0]);
  v8::Isolate* isolate = info.GetIsolate();

  v8::Local<v8::Object> result = GetFunctionKind(func, isolate);
  info.GetReturnValue().Set(result);
}

// Module initialization
NAN_MODULE_INIT(Init) {
  Nan::Set(target, Nan::New("getFunctionKind").ToLocalChecked(),
           Nan::GetFunction(Nan::New<v8::FunctionTemplate>(GetFunctionKind))
               .ToLocalChecked());
}

NODE_MODULE(function_kind, Init)

}  // namespace function_kind
