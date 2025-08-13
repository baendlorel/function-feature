import { it, describe } from 'node:test';
import assert from 'node:assert';
import { getFeatures } from '../lib/index.mjs';

describe('getFeatures', () => {
  it('regular function', () => {
    function regularFunc() {}
    const flags = getFeatures(regularFunc);
    assert.equal(flags.isConstructor, true);
    assert.equal(flags.isAsyncFunction, false);
    assert.equal(flags.isGeneratorFunction, false);
    assert.equal(flags.isBound, false);
  });

  it('bound function', () => {
    function foo() {}
    const bound = foo.bind(null);
    const flags = getFeatures(bound);
    assert.equal(flags.isBound, true);
    assert.equal(flags.isConstructor, true);
  });

  it('class', () => {
    class A {}
    const flags = getFeatures(A);
    assert.equal(flags.isBound, false);
    assert.equal(flags.isConstructor, true);
    assert.equal(flags.isCallable, true);
  });
});
