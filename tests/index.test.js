const { test, describe } = require('node:test');
const assert = require('node:assert');
const getFunctionKind = require('../dist/index.js');

describe('getFunctionKind', () => {
  test('regular function', () => {
    function regularFunc() {}
    const flags = getFunctionKind(regularFunc);

    assert.strictEqual(flags.IsConstructor, true);
    assert.strictEqual(flags.IsAsyncFunction, false);
    assert.strictEqual(flags.IsGeneratorFunction, false);
  });

  test('arrow function', () => {
    const arrowFunc = () => {};
    const flags = getFunctionKind(arrowFunc);

    assert.strictEqual(flags.IsConstructor, false);
    assert.strictEqual(flags.IsAsyncFunction, false);
    assert.strictEqual(flags.IsGeneratorFunction, false);
  });

  test('async function', () => {
    async function asyncFunc() {}
    const flags = getFunctionKind(asyncFunc);

    assert.strictEqual(flags.IsConstructor, false);
    assert.strictEqual(flags.IsAsyncFunction, true);
    assert.strictEqual(flags.IsGeneratorFunction, false);
  });

  test('async arrow function', () => {
    const asyncArrowFunc = async () => {};
    const flags = getFunctionKind(asyncArrowFunc);

    assert.strictEqual(flags.IsConstructor, false);
    assert.strictEqual(flags.IsAsyncFunction, true);
    assert.strictEqual(flags.IsGeneratorFunction, false);
  });

  test('generator function', () => {
    function* generatorFunc() {}
    const flags = getFunctionKind(generatorFunc);

    assert.strictEqual(flags.IsConstructor, false);
    assert.strictEqual(flags.IsAsyncFunction, false);
    assert.strictEqual(flags.IsGeneratorFunction, true);
  });

  test('async generator function', () => {
    async function* asyncGeneratorFunc() {}
    const flags = getFunctionKind(asyncGeneratorFunc);

    assert.strictEqual(flags.IsConstructor, false);
    assert.strictEqual(flags.IsAsyncFunction, true);
    assert.strictEqual(flags.IsGeneratorFunction, true);
  });

  test('class constructor', () => {
    class TestClass {}
    const flags = getFunctionKind(TestClass);

    assert.strictEqual(flags.IsConstructor, true);
    assert.strictEqual(flags.IsAsyncFunction, false);
    assert.strictEqual(flags.IsGeneratorFunction, false);
  });

  test('bound function', () => {
    function originalFunc() {}
    const boundFunc = originalFunc.bind(null);
    const flags = getFunctionKind(boundFunc);

    assert.strictEqual(flags.IsConstructor, true);
    assert.strictEqual(flags.IsAsyncFunction, false);
    assert.strictEqual(flags.IsGeneratorFunction, false);
  });

  test('bound arrow function', () => {
    const originalArrowFunc = () => {};
    const boundArrowFunc = originalArrowFunc.bind(null);
    const flags = getFunctionKind(boundArrowFunc);

    assert.strictEqual(flags.IsConstructor, false);
    assert.strictEqual(flags.IsAsyncFunction, false);
    assert.strictEqual(flags.IsGeneratorFunction, false);
  });

  test('bound async function', () => {
    async function originalAsyncFunc() {}
    const boundAsyncFunc = originalAsyncFunc.bind(null);
    const flags = getFunctionKind(boundAsyncFunc);

    assert.strictEqual(flags.IsConstructor, false);
    assert.strictEqual(flags.IsAsyncFunction, false);
    assert.strictEqual(flags.IsGeneratorFunction, false);
  });

  test('bound generator function', () => {
    function* originalGeneratorFunc() {}
    const boundGeneratorFunc = originalGeneratorFunc.bind(null);
    const flags = getFunctionKind(boundGeneratorFunc);

    assert.strictEqual(flags.IsConstructor, false);
    assert.strictEqual(flags.IsAsyncFunction, false);
    assert.strictEqual(flags.IsGeneratorFunction, false);
  });

  test('bound class constructor', () => {
    class TestClass {}
    const boundClass = TestClass.bind(null);
    const flags = getFunctionKind(boundClass);

    assert.strictEqual(flags.IsConstructor, true);
    assert.strictEqual(flags.IsAsyncFunction, false);
    assert.strictEqual(flags.IsGeneratorFunction, false);
  });

  test('native function', () => {
    const flags = getFunctionKind(Array.isArray);

    assert.strictEqual(flags.IsConstructor, false);
    assert.strictEqual(flags.IsAsyncFunction, false);
    assert.strictEqual(flags.IsGeneratorFunction, false);
  });

  test('method function', () => {
    const obj = {
      method() {},
    };
    const flags = getFunctionKind(obj.method);

    assert.strictEqual(flags.IsConstructor, false);
    assert.strictEqual(flags.IsAsyncFunction, false);
    assert.strictEqual(flags.IsGeneratorFunction, false);
  });

  test('async method function', () => {
    const obj = {
      async method() {},
    };
    const flags = getFunctionKind(obj.method);

    assert.strictEqual(flags.IsConstructor, false);
    assert.strictEqual(flags.IsAsyncFunction, true);
    assert.strictEqual(flags.IsGeneratorFunction, false);
  });

  test('generator method function', () => {
    const obj = {
      *method() {},
    };
    const flags = getFunctionKind(obj.method);

    assert.strictEqual(flags.IsConstructor, true);
    assert.strictEqual(flags.IsAsyncFunction, false);
    assert.strictEqual(flags.IsGeneratorFunction, true);
  });

  test('error handling - non-function input', () => {
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
