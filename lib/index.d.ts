/**
 * Result object for V8 function feature analysis.
 *
 * - isConstructor: `true` if the function is a constructor (can be called with new).
 * - isAsyncFunction: `true` if the function is an async function.
 * - isGeneratorFunction: `true` if the function is a generator function.
 * - isProxy: `true` if the function is a Proxy.
 * - isCallable: `true` if the function is callable.
 * - isBound: `true` if the function is a bound function (created by Function.prototype.bind).
 */
export interface FunctionFeaturesResult {
  isConstructor: boolean;
  isAsyncFunction: boolean;
  isGeneratorFunction: boolean;
  isProxy: boolean;
  isCallable: boolean;
  isBound: boolean;
}

/**
 * Analyze a JavaScript function using V8 internals and return its feature flags.
 *
 * @param fn The function to analyze.
 * @returns An object containing isConstructor, isAsyncFunction, isGeneratorFunction, isProxy, isCallable, isBound.
 */
export declare function getFunctionFeatures(fn: Function): FunctionFeaturesResult;

/**
 * If the function is bound, returns the original function. Otherwise returns undefined.
 */
export declare function getBoundFunction(fn: Function): Function | undefined;

/**
 * Set the name of a function (C++ layer, ignores configurable).
 * @param fn The function to rename.
 * @param name The new name.
 * @returns The function itself.
 */
export declare function setFunctionName(fn: Function, name: string): Function;
