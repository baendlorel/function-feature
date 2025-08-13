/**
 * Result object for V8 function type flags analysis.
 *
 * - isConstructor: `true` if the function is a constructor (can be called with new).
 *   - this is intrinsic, even if it is proxied and prevented from being called with new.
 * - isAsyncFunction: `true` if the function is an async function.
 * - isGeneratorFunction: `true` if the function is a generator function.
 * - isArrowFunction: true if and only if all three above are false;
 *   - `true` if and only if isConstructor, isAsyncFunction, and isGeneratorFunction are all false.
 *   - `false` if isConstructor is true.
 *   - `null` in all other cases.
 */
export interface FunctionKindResult {
  /**
   * True if the function is a constructor (can be called with new).
   */
  isConstructor: boolean;

  /**
   * True if the function is an async function (declared with async).
   */
  isAsyncFunction: boolean;

  /**
   * True if the function is a generator function (declared with *).
   */
  isGeneratorFunction: boolean;

  isProxy: boolean;

  isCallable: boolean;
}

/**
 * Analyze a JavaScript function using V8 internals and return its type flags.
 * 
 * - `isConstructor`: `true` if the function is a constructor (can be called with new).
 *   - this is intrinsic, even if it is proxied and prevented from being called with new.
 * - `isAsyncFunction`: `true` if the function is an async function.
 * - `isGeneratorFunction`: `true` if the function is a generator function.
 * - `isProxy`: 
 * - `isCallable`: 
 *
 * @param func The function to analyze.
 * @returns An object containing isConstructor, isAsyncFunction, isGeneratorFunction, and isArrowFunction.

 */
export declare function getFunctionKind(func: Function): FunctionKindResult;
