import { it, describe } from 'node:test';
import assert from 'node:assert';
import { setName } from '../lib/index.mjs';

describe('setName', () => {
  it('sets function name', () => {
    function fn() {}
    setName(fn, 'newName');
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
    setName(fn, 'newName');
    assert.equal(fn.name, 'cannotmodify');
  });

  it('sets name from symbol description', () => {
    function fn() {}
    setName(fn, Symbol('desc'));
    assert.equal(fn.name, '[desc]');
  });

  it('sets name from symbol with undefined description', () => {
    function fn() {}
    const sym = Symbol();
    setName(fn, sym);
    assert.equal(fn.name, '');
  });
});
