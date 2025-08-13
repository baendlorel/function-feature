import { it, describe } from 'node:test';
import assert from 'node:assert';

import { getFunctionKind } from '../dist/index.js';

describe('getFunctionKind', () => {
  it('regular function', () => {
    function regularFunc() {}
    const flags = getFunctionKind(regularFunc);

    assert(flags.isConstructor === true);
    assert(flags.isAsyncFunction === false);
    expect(flags.isGeneratorFunction === false);
  });

  it('arrow function', () => {
    const arrowFunc = () => {};
    const flags = getFunctionKind(arrowFunc);

    assert(flags.isConstructor === false);
    assert(flags.isAsyncFunction === false);
    assert(flags.isGeneratorFunction === false);
  });

  it('async function', () => {
    async function asyncFunc() {}
    const flags = getFunctionKind(asyncFunc);

    assert(flags.isConstructor === false);
    assert(flags.isAsyncFunction === true);
    assert(flags.isGeneratorFunction === false);
  });

  it('async arrow function', () => {
    const asyncArrowFunc = async () => {};
    const flags = getFunctionKind(asyncArrowFunc);

    assert(flags.isConstructor === false);
    assert(flags.isAsyncFunction === true);
    assert(flags.isGeneratorFunction === false);
  });

  it('generator function', () => {
    function* generatorFunc() {}
    const flags = getFunctionKind(generatorFunc);

    assert(flags.isConstructor === false);
    assert(flags.isAsyncFunction === false);
    assert(flags.isGeneratorFunction === true);
  });

  it('async generator function', () => {
    async function* asyncGeneratorFunc() {}
    const flags = getFunctionKind(asyncGeneratorFunc);

    assert(flags.isConstructor === false);
    assert(flags.isAsyncFunction === true);
    assert(flags.isGeneratorFunction === true);
  });

  it('class constructor', () => {
    class TestClass {}
    const flags = getFunctionKind(TestClass);

    assert(flags.isConstructor === true);
    assert(flags.isAsyncFunction === false);
    assert(flags.isGeneratorFunction === false);
  });

  it('bound function', () => {
    function originalFunc() {}
    const boundFunc = originalFunc.bind(null);
    const flags = getFunctionKind(boundFunc);

    assert(flags.isConstructor === true);
    assert(flags.isAsyncFunction === false);
    assert(flags.isGeneratorFunction === false);
  });

  it('bound arrow function', () => {
    const originalArrowFunc = () => {};
    const boundArrowFunc = originalArrowFunc.bind(null);
    const flags = getFunctionKind(boundArrowFunc);

    assert(flags.isConstructor === false);
    assert(flags.isAsyncFunction === false);
    assert(flags.isGeneratorFunction === false);
  });

  it('bound async function', () => {
    async function originalAsyncFunc() {}
    const boundAsyncFunc = originalAsyncFunc.bind(null);
    const flags = getFunctionKind(boundAsyncFunc);

    assert(flags.isConstructor === false);
    assert(flags.isAsyncFunction === false);
    assert(flags.isGeneratorFunction === false);
  });

  it('bound generator function', () => {
    function* originalGeneratorFunc() {}
    const boundGeneratorFunc = originalGeneratorFunc.bind(null);
    const flags = getFunctionKind(boundGeneratorFunc);

    assert(flags.isConstructor === false);
    assert(flags.isAsyncFunction === false);
    assert(flags.isGeneratorFunction === false);
  });

  it('bound class constructor', () => {
    class TestClass {}
    const boundClass = TestClass.bind(null);
    const flags = getFunctionKind(boundClass);

    assert(flags.isConstructor === true);
    assert(flags.isAsyncFunction === false);
    assert(flags.isGeneratorFunction === false);
  });

  it('native function', () => {
    const flags = getFunctionKind(Array.isArray);

    assert(flags.isConstructor === false);
    assert(flags.isAsyncFunction === false);
    assert(flags.isGeneratorFunction === false);
  });

  it('method function', () => {
    const obj = {
      method() {},
    };
    const flags = getFunctionKind(obj.method);

    assert(flags.isConstructor === false);
    assert(flags.isAsyncFunction === false);
    assert(flags.isGeneratorFunction === false);
  });

  it('async method function', () => {
    const obj = {
      async method() {},
    };
    const flags = getFunctionKind(obj.method);

    assert(flags.isConstructor === false);
    assert(flags.isAsyncFunction === true);
    assert(flags.isGeneratorFunction === false);
  });

  it('generator method function', () => {
    const obj = {
      *method() {},
    };
    const flags = getFunctionKind(obj.method);

    assert(flags.isConstructor === true);
    assert(flags.isAsyncFunction === false);
    assert(flags.isGeneratorFunction === true);
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
