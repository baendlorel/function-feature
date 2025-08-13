import { it, describe } from 'node:test';
import assert from 'node:assert';

import { getFunctionFeatures, getBoundFunction } from '../lib/index.mjs';

describe('getFunctionFeatures', () => {
  it('regular function', () => {
    function regularFunc() {}
    const flags = getFunctionFeatures(regularFunc);

    assert.equal(flags.isConstructor, true);
    assert.equal(flags.isAsyncFunction, false);
    assert.equal(flags.isGeneratorFunction, false);
  });

  it('arrow function', () => {
    const arrowFunc = () => {};
    const flags = getFunctionFeatures(arrowFunc);

    assert.equal(flags.isConstructor, false);
    assert.equal(flags.isAsyncFunction, false);
    assert.equal(flags.isGeneratorFunction, false);
  });

  it('async function', () => {
    async function asyncFunc() {}
    const flags = getFunctionFeatures(asyncFunc);

    assert.equal(flags.isConstructor, false);
    assert.equal(flags.isAsyncFunction, true);
    assert.equal(flags.isGeneratorFunction, false);
  });

  it('async arrow function', () => {
    const asyncArrowFunc = async () => {};
    const flags = getFunctionFeatures(asyncArrowFunc);

    assert.equal(flags.isConstructor, false);
    assert.equal(flags.isAsyncFunction, true);
    assert.equal(flags.isGeneratorFunction, false);
  });

  it('generator function', () => {
    function* generatorFunc() {}
    const flags = getFunctionFeatures(generatorFunc);

    assert.equal(flags.isConstructor, false);
    assert.equal(flags.isAsyncFunction, false);
    assert.equal(flags.isGeneratorFunction, true);
  });

  it('async generator function', () => {
    async function* asyncGeneratorFunc() {}
    const flags = getFunctionFeatures(asyncGeneratorFunc);

    assert.equal(flags.isConstructor, false);
    assert.equal(flags.isAsyncFunction, true);
    assert.equal(flags.isGeneratorFunction, true);
  });

  it('class constructor', () => {
    class TestClass {}
    const flags = getFunctionFeatures(TestClass);

    assert.equal(flags.isConstructor, true);
    assert.equal(flags.isAsyncFunction, false);
    assert.equal(flags.isGeneratorFunction, false);
  });

  it('bound function', () => {
    function originalFunc() {}
    const boundFunc = originalFunc.bind(null);
    const flags = getFunctionFeatures(boundFunc);

    assert.equal(flags.isConstructor, true);
    assert.equal(flags.isAsyncFunction, false);
    assert.equal(flags.isGeneratorFunction, false);
  });

  it('bound arrow function', () => {
    const originalArrowFunc = () => {};
    const boundArrowFunc = originalArrowFunc.bind(null);
    const flags = getFunctionFeatures(boundArrowFunc);

    assert.equal(flags.isConstructor, false);
    assert.equal(flags.isAsyncFunction, false);
    assert.equal(flags.isGeneratorFunction, false);
  });

  it('bound async function', () => {
    async function originalAsyncFunc() {}
    const boundAsyncFunc = originalAsyncFunc.bind(null);
    const flags = getFunctionFeatures(boundAsyncFunc);

    assert.equal(flags.isConstructor, false);
    assert.equal(flags.isAsyncFunction, false);
    assert.equal(flags.isGeneratorFunction, false);
  });

  it('bound generator function', () => {
    function* originalGeneratorFunc() {}
    const boundGeneratorFunc = originalGeneratorFunc.bind(null);
    const flags = getFunctionFeatures(boundGeneratorFunc);

    assert.equal(flags.isConstructor, false);
    assert.equal(flags.isAsyncFunction, false);
    assert.equal(flags.isGeneratorFunction, false);
  });

  it('bound class constructor', () => {
    class TestClass {}
    const boundClass = TestClass.bind(null);
    const flags = getFunctionFeatures(boundClass);

    assert.equal(flags.isConstructor, true);
    assert.equal(flags.isAsyncFunction, false);
    assert.equal(flags.isGeneratorFunction, false);
  });

  it('native function', () => {
    const flags = getFunctionFeatures(Array.isArray);

    assert.equal(flags.isConstructor, false);
    assert.equal(flags.isAsyncFunction, false);
    assert.equal(flags.isGeneratorFunction, false);
  });

  it('method function', () => {
    const obj = {
      method() {},
    };
    const flags = getFunctionFeatures(obj.method);

    assert.equal(flags.isConstructor, false);
    assert.equal(flags.isAsyncFunction, false);
    assert.equal(flags.isGeneratorFunction, false);
  });

  it('async method function', () => {
    const obj = {
      async method() {},
    };
    const flags = getFunctionFeatures(obj.method);

    assert.equal(flags.isConstructor, false);
    assert.equal(flags.isAsyncFunction, true);
    assert.equal(flags.isGeneratorFunction, false);
  });

  it('generator method function', () => {
    const obj = {
      *method() {},
    };
    const flags = getFunctionFeatures(obj.method);

    assert.equal(flags.isConstructor, false);
    assert.equal(flags.isAsyncFunction, false);
    assert.equal(flags.isGeneratorFunction, true);
  });

  it('error handling - non-function input', () => {
    assert.throws(() => {
      getFunctionFeatures('not a function');
    }, TypeError);

    assert.throws(() => {
      getFunctionFeatures(null);
    }, TypeError);

    assert.throws(() => {
      getFunctionFeatures(undefined);
    }, TypeError);

    assert.throws(() => {
      getFunctionFeatures(123);
    }, TypeError);
  });
});

describe('getBoundFunction', () => {
  it('returns original function for bound', () => {
    function foo() {}
    const bound = foo.bind(null);
    const orig = getBoundFunction(bound);
    assert.equal(orig, foo);
  });

  it('returns undefined for non-bound', () => {
    function bar() {}
    const orig = getBoundFunction(bar);
    assert.equal(orig, undefined);
  });
});
