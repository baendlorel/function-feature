import { it, describe } from 'node:test';
import assert from 'node:assert';
import { getOrigin } from '../lib/index.mjs';

describe('getOrigin', () => {
  it('unwraps bound and proxy functions', () => {
    function base() {}
    const proxy = new Proxy(base, {});
    const bound = proxy.bind(null);
    assert.equal(getOrigin(bound), base);
    assert.equal(getOrigin(proxy), base);
  });

  it('returns itself for non-wrapped function', () => {
    function foo() {}
    assert.equal(getOrigin(foo), foo);
  });

  it('returns object itself if not proxy', () => {
    const obj = { a: 1 };
    assert.equal(getOrigin(obj), obj);
  });

  it('unwraps proxy object', () => {
    const target = { x: 1 };
    const handler = {};
    const proxy = new Proxy(target, handler);
    assert.equal(getOrigin(proxy), target);
  });
});
