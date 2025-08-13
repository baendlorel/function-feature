#include <nan.h>
#include <v8.h>
#include "v8_helpers.h"

using namespace Nan;

namespace function_kind {

// Function to analyze V8 function and determine its kind
std::string AnalyzeFunctionKind(v8::Local<v8::Function> func, v8::Isolate* isolate) {
    v8::HandleScope scope(isolate);
    v8::Local<v8::Context> context = isolate->GetCurrentContext();
    
    // Get function's internal properties
    v8::Local<v8::String> func_string = func->ToString(context).ToLocalChecked();
    v8::String::Utf8Value utf8_string(isolate, func_string);
    std::string source_code(*utf8_string);
    
    // Check if it's a class constructor using V8 internal flags
    // This uses V8's internal representation to detect class constructors
    if (func->IsConstructor()) {
        // Further check if it's actually a class constructor
        if (source_code.find("class ") == 0) {
            return "class_constructor";
        }
        // Regular constructor function
        return "constructor";
    }
    
    // Check for async functions
    if (func->IsAsyncFunction()) {
        // Check if it's also a generator
        if (func->IsGeneratorFunction()) {
            return "async_generator";
        }
        return "async";
    }
    
    // Check for generator functions
    if (func->IsGeneratorFunction()) {
        return "generator";
    }
    
    // Check for bound functions
    v8::Local<v8::String> name_string = func->GetName()->ToString(context).ToLocalChecked();
    v8::String::Utf8Value name_utf8(isolate, name_string);
    std::string name(*name_utf8);
    if (name.find("bound ") == 0) {
        return "bound";
    }
    
    // Check for arrow functions by examining prototype property
    v8::Local<v8::String> prototype_key = v8::String::NewFromUtf8(isolate, "prototype", v8::NewStringType::kNormal).ToLocalChecked();
    v8::Maybe<bool> has_prototype = func->Has(context, prototype_key);
    
    if (has_prototype.IsJust() && !has_prototype.FromJust()) {
        return "arrow";
    }
    
    // Check for native functions
    if (source_code.find("[native code]") != std::string::npos) {
        return "native";
    }
    
    // Check for method functions (no function keyword, has parentheses)
    if (source_code.find("function") == std::string::npos && 
        source_code.find("(") != std::string::npos &&
        source_code.find("=>") == std::string::npos) {
        return "method";
    }
    
    // Default to regular function
    return "regular";
}

// Get detailed function information
v8::Local<v8::Object> GetFunctionDetails(v8::Local<v8::Function> func, v8::Isolate* isolate) {
    v8::EscapableHandleScope scope(isolate);
    v8::Local<v8::Context> context = isolate->GetCurrentContext();
    v8::Local<v8::Object> result = v8::Object::New(isolate);
    
    // Basic properties
    v8::Local<v8::String> kind_key = v8::String::NewFromUtf8(isolate, "kind", v8::NewStringType::kNormal).ToLocalChecked();
    std::string kind = AnalyzeFunctionKind(func, isolate);
    v8::Local<v8::String> kind_value = v8::String::NewFromUtf8(isolate, kind.c_str(), v8::NewStringType::kNormal).ToLocalChecked();
    result->Set(context, kind_key, kind_value).Check();
    
    // Function name
    v8::Local<v8::String> name_key = v8::String::NewFromUtf8(isolate, "name", v8::NewStringType::kNormal).ToLocalChecked();
    v8::Local<v8::Value> name_value = func->GetName();
    result->Set(context, name_key, name_value).Check();
    
    // Function length (parameter count) - use GetLength() method
    v8::Local<v8::String> length_key = v8::String::NewFromUtf8(isolate, "length", v8::NewStringType::kNormal).ToLocalChecked();
    v8::Local<v8::Value> length_prop = func->Get(context, v8::String::NewFromUtf8(isolate, "length", v8::NewStringType::kNormal).ToLocalChecked()).ToLocalChecked();
    result->Set(context, length_key, length_prop).Check();
    
    // Boolean flags
    v8::Local<v8::String> is_constructor_key = v8::String::NewFromUtf8(isolate, "isConstructor", v8::NewStringType::kNormal).ToLocalChecked();
    v8::Local<v8::Boolean> is_constructor_value = v8::Boolean::New(isolate, func->IsConstructor());
    result->Set(context, is_constructor_key, is_constructor_value).Check();
    
    v8::Local<v8::String> is_async_key = v8::String::NewFromUtf8(isolate, "isAsync", v8::NewStringType::kNormal).ToLocalChecked();
    v8::Local<v8::Boolean> is_async_value = v8::Boolean::New(isolate, func->IsAsyncFunction());
    result->Set(context, is_async_key, is_async_value).Check();
    
    v8::Local<v8::String> is_generator_key = v8::String::NewFromUtf8(isolate, "isGenerator", v8::NewStringType::kNormal).ToLocalChecked();
    v8::Local<v8::Boolean> is_generator_value = v8::Boolean::New(isolate, func->IsGeneratorFunction());
    result->Set(context, is_generator_key, is_generator_value).Check();
    
    // Check prototype existence
    v8::Local<v8::String> prototype_key = v8::String::NewFromUtf8(isolate, "prototype", v8::NewStringType::kNormal).ToLocalChecked();
    v8::Maybe<bool> has_prototype = func->Has(context, prototype_key);
    v8::Local<v8::String> has_prototype_key = v8::String::NewFromUtf8(isolate, "hasPrototype", v8::NewStringType::kNormal).ToLocalChecked();
    v8::Local<v8::Boolean> has_prototype_value = v8::Boolean::New(isolate, has_prototype.IsJust() && has_prototype.FromJust());
    result->Set(context, has_prototype_key, has_prototype_value).Check();
    
    // Source code (for debugging)
    v8::Local<v8::String> source_key = v8::String::NewFromUtf8(isolate, "source", v8::NewStringType::kNormal).ToLocalChecked();
    v8::Local<v8::String> source_value = func->ToString(context).ToLocalChecked();
    result->Set(context, source_key, source_value).Check();
    
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
    
    std::string kind = AnalyzeFunctionKind(func, isolate);
    v8::Local<v8::String> result = v8::String::NewFromUtf8(isolate, kind.c_str(), v8::NewStringType::kNormal).ToLocalChecked();
    
    info.GetReturnValue().Set(result);
}

// NAN method: getFunctionInfo
NAN_METHOD(GetFunctionInfo) {
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
    
    v8::Local<v8::Object> result = GetFunctionDetails(func, isolate);
    info.GetReturnValue().Set(result);
}

// NAN method: isClassConstructor (specific check for class constructors)
NAN_METHOD(IsClassConstructor) {
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
    v8::Local<v8::Context> context = isolate->GetCurrentContext();
    
    // Check if it's a constructor and has class syntax
    bool is_constructor = func->IsConstructor();
    bool is_class = false;
    
    if (is_constructor) {
        v8::Local<v8::String> source = func->ToString(context).ToLocalChecked();
        v8::String::Utf8Value utf8_source(isolate, source);
        std::string source_str(*utf8_source);
        is_class = (source_str.find("class ") == 0);
    }
    
    v8::Local<v8::Boolean> result = v8::Boolean::New(isolate, is_class);
    info.GetReturnValue().Set(result);
}

// Module initialization
NAN_MODULE_INIT(Init) {
    Nan::Set(target, Nan::New("getFunctionKind").ToLocalChecked(),
        Nan::GetFunction(Nan::New<v8::FunctionTemplate>(GetFunctionKind)).ToLocalChecked());
    
    Nan::Set(target, Nan::New("getFunctionInfo").ToLocalChecked(),
        Nan::GetFunction(Nan::New<v8::FunctionTemplate>(GetFunctionInfo)).ToLocalChecked());
    
    Nan::Set(target, Nan::New("isClassConstructor").ToLocalChecked(),
        Nan::GetFunction(Nan::New<v8::FunctionTemplate>(IsClassConstructor)).ToLocalChecked());
}

NODE_MODULE(function_kind, Init)

} // namespace function_kind
