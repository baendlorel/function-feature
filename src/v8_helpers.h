#ifndef V8_HELPERS_H
#define V8_HELPERS_H

#include <v8.h>
#include <string>

namespace function_kind {

// Helper functions for V8 introspection
std::string GetFunctionSource(v8::Local<v8::Function> func, v8::Isolate* isolate);
bool HasInternalSlot(v8::Local<v8::Function> func, const std::string& slot_name, v8::Isolate* isolate);
std::string DetectFunctionKindFromSource(const std::string& source);

} // namespace function_kind

#endif // V8_HELPERS_H
