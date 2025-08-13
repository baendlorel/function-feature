import { it, describe } from 'node:test';
import assert from 'node:assert';
import { setFunctionName } from '../lib/index.mjs';

describe('setFunctionName', () => {
  it('sets function name', () => {
    function fn() {}
    setFunctionName(fn, 'newName');
    assert.equal(fn.name, 'newName');
  });

  it('sets function name with configurable = false', () => {
    function fn() {}
    Object.defineProperty(fn, 'name', {
      value: 'cannotmodify',
      configurable: false,
      writable: false,
    });
    assert.equal(fn.name, 'cannotmodify');
    setFunctionName(fn, 'newName');
    assert.equal(fn.name, 'cannotmodify');
  });
});
