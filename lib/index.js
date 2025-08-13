const functionKind = require('./build/Release/function_kind');

/**
 * Get V8 function type flags (IsConstructor, IsAsyncFunction, IsGeneratorFunction)
 * @param {Function} func - The function to analyze
 * @returns {Object} Object with IsConstructor, IsAsyncFunction, IsGeneratorFunction boolean flags
 */
export const getFunctionKind = functionKind.getFunctionKind;
