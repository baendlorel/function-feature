import { it, describe } from 'node:test';
import assert from 'node:assert';
import { getFunctionFeatures } from '../lib/index.mjs';

describe('getFunctionFeatures', () => {
  it('regular function', () => {
    function regularFunc() {}
    const flags = getFunctionFeatures(regularFunc);
    console.log('flags', flags);
    assert.equal(flags.isConstructor, true);
    assert.equal(flags.isAsyncFunction, false);
    assert.equal(flags.isGeneratorFunction, false);
    assert.equal(flags.isBound, false);
  });

  it('bound function', () => {
    function foo() {}
    const bound = foo.bind(null);
    const flags = getFunctionFeatures(bound);
    assert.equal(flags.isBound, true);
    assert.equal(flags.isConstructor, false);
  });
});
