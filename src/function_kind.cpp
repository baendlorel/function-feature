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
  bool isConstructor = func->IsConstructor();
  v8::Local<v8::String> is_constructor_key =
      v8::String::NewFromUtf8(isolate, "isConstructor",
                              v8::NewStringType::kNormal)
          .ToLocalChecked();
  v8::Local<v8::Boolean> is_constructor_value =
      v8::Boolean::New(isolate, isConstructor);
  result->Set(context, is_constructor_key, is_constructor_value).Check();

  // IsAsyncFunction flag
  bool isAsync = func->IsAsyncFunction();
  v8::Local<v8::String> is_async_key =
      v8::String::NewFromUtf8(isolate, "isAsyncFunction",
                              v8::NewStringType::kNormal)
          .ToLocalChecked();
  v8::Local<v8::Boolean> is_async_value = v8::Boolean::New(isolate, isAsync);
  result->Set(context, is_async_key, is_async_value).Check();

  // IsGeneratorFunction flag
  bool isGenerator = func->IsGeneratorFunction();
  v8::Local<v8::String> is_generator_key =
      v8::String::NewFromUtf8(isolate, "isGeneratorFunction",
                              v8::NewStringType::kNormal)
          .ToLocalChecked();
  v8::Local<v8::Boolean> is_generator_value =
      v8::Boolean::New(isolate, isGenerator);
  result->Set(context, is_generator_key, is_generator_value).Check();

  // isArrowFunction 字段逻辑
  v8::Local<v8::String> is_arrow_key =
      v8::String::NewFromUtf8(isolate, "isArrowFunction",
                              v8::NewStringType::kNormal)
          .ToLocalChecked();
  v8::Local<v8::Value> is_arrow_value;
  if (isConstructor) {
    is_arrow_value = Nan::False();
  } else if (!isConstructor && !isAsync && !isGenerator) {
    is_arrow_value = Nan::True();
  } else {
    is_arrow_value = Nan::Null();
  }
  result->Set(context, is_arrow_key, is_arrow_value).Check();

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
