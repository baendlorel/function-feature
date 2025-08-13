type Fn = (...args: unknown[]) => unknown;

/**
 * Result object for V8 function feature analysis
 *
 * - isConstructor: `true` if the function is a constructor (can be called with new)
 * - isAsyncFunction: `true` if the function is an async function
 * - isGeneratorFunction: `true` if the function is a generator function
 * - isProxy: `true` if the function is a Proxy
 * - isCallable: `true` if the function is callable
 * - isBound: `true` if the function is a bound function (created by Function.prototype.bind)
 */
interface FunctionFeaturesResult {
  isConstructor: boolean;
  isAsyncFunction: boolean;
  isGeneratorFunction: boolean;
  isProxy: boolean;
  isCallable: boolean;
  isBound: boolean;
}

/**
 * Analyze a JavaScript function using V8 internals and return its feature flags
 *
 * @param fn The function to analyze
 * @returns An object containing isConstructor, isAsyncFunction, isGeneratorFunction, isProxy, isCallable, isBound
 */
export declare function getFeatures(fn: Fn): FunctionFeaturesResult;

/**
 * Get the function `fn0` satisfies `fn = fn0.bind(thisArg, ...args)`
 */
export declare function getBound<T extends Fn>(fn: T): T | undefined;

/**
 * If the function is bound, returns the most original function. Otherwise returns itself
 * @example
 * const f0 = function(...args) {};
 * const f1 = f0.bind(null,1);
 * const f2 = f1.bind(null,2);
 *
 * // returns the most original function
 * console.log(getBoundOrigin(f2) === f0); // true
 *
 * // if not bound, return itself
 * console.log(getBoundOrigin(f0) === f0); // true
 */
export declare function getBoundOrigin<T extends Fn>(fn: T): T | undefined;

/**
 * Set the name of a function
 * - Use v8-function.h: void SetName(Local<String> name)
 * - cannot set name when `configurable` is `false`
 *   - because it is not the real [[SetFunctionName]] in es262
 * @param fn The function to rename
 * @param name The new name
 * @returns The function itself
 */
export declare function setName<T extends Fn>(fn: T, name: string): T;

/**
 * Equivalent to `Function.prototype.toString` but uses V8 internals to get the string representation
 * - safe and will not be affected by the `toString` override
 * @param fn The function to convert to a string
 */
export declare function protoToString(fn: Fn): string;
