#include <nan.h>
#include <v8-function.h>
#include <v8.h>

using namespace Nan;

namespace function_kind {

// bool IsArrowFunction(v8::Local<v8::Function> func) {
//   v8::Function* v8Func = v8::Function::Cast(*func);
//   return v8Func.;  // 需要 V8 调试符号
// }

void SetResult(v8::Isolate* isolate,
               v8::Local<v8::Object> result,
               v8::Local<v8::Context> context,
               const char* key,
               bool value) {
  v8::Local<v8::String> v8_key =
      v8::String::NewFromUtf8(isolate, key, v8::NewStringType::kNormal)
          .ToLocalChecked();
  v8::Local<v8::Boolean> v8_value = v8::Boolean::New(isolate, key);
  result->Set(context, v8_key, v8_value).Check();
}

// Get V8 function type flags
v8::Local<v8::Object> GetFunctionKind(v8::Local<v8::Function> func,
                                      v8::Isolate* isolate) {
  v8::EscapableHandleScope scope(isolate);
  v8::Local<v8::Context> context = isolate->GetCurrentContext();
  v8::Local<v8::Object> result = v8::Object::New(isolate);

  bool isConstructor = func->IsConstructor();
  bool isAsync = func->IsAsyncFunction();
  bool isGenerator = func->IsGeneratorFunction();
  bool isProxy = func->IsProxy();

  SetResult(isolate, result, context, "isConstructor", isConstructor);
  SetResult(isolate, result, context, "isAsyncFunction", isAsync);
  SetResult(isolate, result, context, "isGeneratorFunction", isGenerator);
  SetResult(isolate, result, context, "isProxy", isProxy);

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
