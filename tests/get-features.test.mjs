import { describe, it, expect } from 'vitest';
import { getFeatures } from '../lib/index.mjs';

describe('getFeatures', () => {
  it('regular function', () => {
    function regularFunc() {}
    const flags = getFeatures(regularFunc);
    expect(flags.isConstructor).toBe(true);
    expect(flags.isAsyncFunction).toBe(false);
    expect(flags.isGeneratorFunction).toBe(false);
    expect(flags.isBound).toBe(false);
    expect(flags.isClass).toBe(false);
  });

  it('bound function', () => {
    function foo() {}
    const bound = foo.bind(null);
    const flags = getFeatures(bound);
    expect(flags.isBound).toBe(true);
    expect(flags.isConstructor).toBe(true);
  });

  it('class', () => {
    class A {}
    const flags = getFeatures(A);
    expect(flags.isBound).toBe(false);
    expect(flags.isConstructor).toBe(true);
    expect(flags.isCallable).toBe(true);
    expect(flags.isClass).toBe(true);
  });
});
