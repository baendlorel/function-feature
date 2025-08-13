import { it, describe } from 'node:test';
import assert from 'node:assert';

import { getFunctionKind } from '../lib/index.mjs';

describe('getFunctionKind', () => {
  it('regular function', () => {
    function regularFunc() {}
    const flags = getFunctionKind(regularFunc);

    assert.equal(flags.isConstructor, true);
    assert.equal(flags.isAsyncFunction, false);
    assert.equal(flags.isGeneratorFunction, false);
    assert.equal(flags.isArrowFunction, false);
  });

  it('arrow function', () => {
    const arrowFunc = () => {};
    const flags = getFunctionKind(arrowFunc);

    assert.equal(flags.isConstructor, false);
    assert.equal(flags.isAsyncFunction, false);
    assert.equal(flags.isGeneratorFunction, false);
    assert.equal(flags.isArrowFunction, true);
  });

  it('async function', () => {
    async function asyncFunc() {}
    const flags = getFunctionKind(asyncFunc);

    assert.equal(flags.isConstructor, false);
    assert.equal(flags.isAsyncFunction, true);
    assert.equal(flags.isGeneratorFunction, false);
    assert.equal(flags.isArrowFunction, null);
  });

  it('async arrow function', () => {
    const asyncArrowFunc = async () => {};
    const flags = getFunctionKind(asyncArrowFunc);

    assert.equal(flags.isConstructor, false);
    assert.equal(flags.isAsyncFunction, true);
    assert.equal(flags.isGeneratorFunction, false);
    assert.equal(flags.isArrowFunction, null);
  });

  it('generator function', () => {
    function* generatorFunc() {}
    const flags = getFunctionKind(generatorFunc);

    assert.equal(flags.isConstructor, false);
    assert.equal(flags.isAsyncFunction, false);
    assert.equal(flags.isGeneratorFunction, true);
    assert.equal(flags.isArrowFunction, null);
  });

  it('async generator function', () => {
    async function* asyncGeneratorFunc() {}
    const flags = getFunctionKind(asyncGeneratorFunc);

    assert.equal(flags.isConstructor, false);
    assert.equal(flags.isAsyncFunction, true);
    assert.equal(flags.isGeneratorFunction, true);
    assert.equal(flags.isArrowFunction, null);
  });

  it('class constructor', () => {
    class TestClass {}
    const flags = getFunctionKind(TestClass);

    assert.equal(flags.isConstructor, true);
    assert.equal(flags.isAsyncFunction, false);
    assert.equal(flags.isGeneratorFunction, false);
    assert.equal(flags.isArrowFunction, false);
  });

  it('bound function', () => {
    function originalFunc() {}
    const boundFunc = originalFunc.bind(null);
    const flags = getFunctionKind(boundFunc);

    assert.equal(flags.isConstructor, true);
    assert.equal(flags.isAsyncFunction, false);
    assert.equal(flags.isGeneratorFunction, false);
    assert.equal(flags.isArrowFunction, false);
  });

  it('bound arrow function', () => {
    const originalArrowFunc = () => {};
    const boundArrowFunc = originalArrowFunc.bind(null);
    const flags = getFunctionKind(boundArrowFunc);

    assert.equal(flags.isConstructor, false);
    assert.equal(flags.isAsyncFunction, false);
    assert.equal(flags.isGeneratorFunction, false);
    assert.equal(flags.isArrowFunction, true);
  });

  it('bound async function', () => {
    async function originalAsyncFunc() {}
    const boundAsyncFunc = originalAsyncFunc.bind(null);
    const flags = getFunctionKind(boundAsyncFunc);

    assert.equal(flags.isConstructor, false);
    assert.equal(flags.isAsyncFunction, false);
    assert.equal(flags.isGeneratorFunction, false);
    assert.equal(flags.isArrowFunction, null);
  });

  it('bound generator function', () => {
    function* originalGeneratorFunc() {}
    const boundGeneratorFunc = originalGeneratorFunc.bind(null);
    const flags = getFunctionKind(boundGeneratorFunc);

    assert.equal(flags.isConstructor, false);
    assert.equal(flags.isAsyncFunction, false);
    assert.equal(flags.isGeneratorFunction, false);
    assert.equal(flags.isArrowFunction, null);
  });

  it('bound class constructor', () => {
    class TestClass {}
    const boundClass = TestClass.bind(null);
    const flags = getFunctionKind(boundClass);

    assert.equal(flags.isConstructor, true);
    assert.equal(flags.isAsyncFunction, false);
    assert.equal(flags.isGeneratorFunction, false);
    assert.equal(flags.isArrowFunction, false);
  });

  it('native function', () => {
    const flags = getFunctionKind(Array.isArray);

    assert.equal(flags.isConstructor, false);
    assert.equal(flags.isAsyncFunction, false);
    assert.equal(flags.isGeneratorFunction, false);
    assert.equal(flags.isArrowFunction, false);
  });

  it('method function', () => {
    const obj = {
      method() {},
    };
    const flags = getFunctionKind(obj.method);

    assert.equal(flags.isConstructor, false);
    assert.equal(flags.isAsyncFunction, false);
    assert.equal(flags.isGeneratorFunction, false);
    // fixme 成员函数也不能new！
    assert.equal(flags.isArrowFunction, false);
  });

  it('async method function', () => {
    const obj = {
      async method() {},
    };
    const flags = getFunctionKind(obj.method);

    assert.equal(flags.isConstructor, false);
    assert.equal(flags.isAsyncFunction, true);
    assert.equal(flags.isGeneratorFunction, false);
    assert.equal(flags.isArrowFunction, null);
  });

  it('generator method function', () => {
    const obj = {
      *method() {},
    };
    const flags = getFunctionKind(obj.method);

    assert.equal(flags.isConstructor, false);
    assert.equal(flags.isAsyncFunction, false);
    assert.equal(flags.isGeneratorFunction, true);
    assert.equal(flags.isArrowFunction, null);
  });

  it('error handling - non-function input', () => {
    assert.throws(() => {
      getFunctionKind('not a function');
    }, TypeError);

    assert.throws(() => {
      getFunctionKind(null);
    }, TypeError);

    assert.throws(() => {
      getFunctionKind(undefined);
    }, TypeError);

    assert.throws(() => {
      getFunctionKind(123);
    }, TypeError);
  });
});
