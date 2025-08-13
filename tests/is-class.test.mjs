import { it, describe } from 'node:test';
import assert from 'node:assert';
import { isClass } from '../lib/index.mjs';

describe('isClass', () => {
  it('should return true for class declarations', () => {
    class TestClass {}
    assert.equal(isClass(TestClass), true);
  });

  it('should return true for class expressions', () => {
    const TestClass = class {};
    assert.equal(isClass(TestClass), true);
  });

  it('should return true for native constructors', () => {
    assert.equal(isClass(Array), true);
    assert.equal(isClass(Object), true);
    assert.equal(isClass(Function), true);
    assert.equal(isClass(Boolean), true);
    assert.equal(isClass(Number), true);
    assert.equal(isClass(String), true);
    assert.equal(isClass(Date), true);
    assert.equal(isClass(RegExp), true);
    assert.equal(isClass(Error), true);
  });

  it('should return false for regular functions', () => {
    function regularFunction() {}
    assert.equal(isClass(regularFunction), false);
  });

  it('should return false for arrow functions', () => {
    const arrowFunction = () => {};
    assert.equal(isClass(arrowFunction), false);
  });

  it('should return false for async functions', () => {
    async function asyncFunction() {}
    assert.equal(isClass(asyncFunction), false);
  });

  it('should return false for generator functions', () => {
    function* generatorFunction() {}
    assert.equal(isClass(generatorFunction), false);
  });

  it('should work with bound classes', () => {
    class TestClass {}
    const boundClass = TestClass.bind(null);
    assert.equal(isClass(boundClass), true);
  });

  it('should work with bound functions', () => {
    function regularFunction() {}
    const boundFunction = regularFunction.bind(null);
    assert.equal(isClass(boundFunction), false);
  });
});
