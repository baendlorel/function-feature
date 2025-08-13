#include "v8_helpers.h"
#include <regex>

namespace function_kind {

std::string GetFunctionSource(v8::Local<v8::Function> func, v8::Isolate* isolate) {
    v8::HandleScope scope(isolate);
    v8::Local<v8::Context> context = isolate->GetCurrentContext();
    
    v8::Local<v8::String> source_string = func->ToString(context).ToLocalChecked();
    v8::String::Utf8Value utf8_source(isolate, source_string);
    
    return std::string(*utf8_source);
}

bool HasInternalSlot(v8::Local<v8::Function> func, const std::string& slot_name, v8::Isolate* isolate) {
    // This is a placeholder for checking internal slots
    // In practice, we rely on V8's public API methods like IsAsyncFunction(), IsGeneratorFunction(), etc.
    v8::HandleScope scope(isolate);
    v8::Local<v8::Context> context = isolate->GetCurrentContext();
    
    // Check if the function has specific properties that indicate internal slots
    if (slot_name == "AsyncFunction") {
        return func->IsAsyncFunction();
    } else if (slot_name == "GeneratorFunction") {
        return func->IsGeneratorFunction();
    } else if (slot_name == "Constructor") {
        return func->IsConstructor();
    }
    
    return false;
}

std::string DetectFunctionKindFromSource(const std::string& source) {
    // Remove whitespace for easier matching
    std::string trimmed = source;
    trimmed.erase(0, trimmed.find_first_not_of(" \t\n\r"));
    
    // Check for class constructors
    if (trimmed.find("class ") == 0) {
        return "class_constructor";
    }
    
    // Check for async generator
    if (trimmed.find("async function*") == 0 || 
        (trimmed.find("async ") == 0 && trimmed.find("function*") != std::string::npos)) {
        return "async_generator";
    }
    
    // Check for async function
    if (trimmed.find("async ") == 0 || trimmed.find("async function") == 0) {
        return "async";
    }
    
    // Check for generator function
    if (trimmed.find("function*") == 0) {
        return "generator";
    }
    
    // Check for arrow function
    if (trimmed.find("function") == std::string::npos && trimmed.find("=>") != std::string::npos) {
        return "arrow";
    }
    
    // Check for regular function
    if (trimmed.find("function ") == 0) {
        return "regular";
    }
    
    // Check for method (no function keyword, has parentheses)
    if (trimmed.find("function") == std::string::npos && 
        trimmed.find("(") != std::string::npos &&
        trimmed.find("=>") == std::string::npos) {
        return "method";
    }
    
    // Check for native code
    if (source.find("[native code]") != std::string::npos) {
        return "native";
    }
    
    return "unknown";
}

} // namespace function_kind
