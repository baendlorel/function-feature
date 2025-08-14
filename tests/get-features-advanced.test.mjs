import { it, describe } from 'node:test';
import assert from 'node:assert';
import { getFeatures } from '../lib/index.mjs';

describe('getFeatures advanced', () => {
  it('proxy function', () => {
    function base() {}
    const proxy = new Proxy(base, {});
    const flags = getFeatures(proxy);
    assert.equal(flags.isProxy, true);
    assert.equal(flags.origin, base);
  });

  it('bound proxy function', () => {
    function base() {}
    const proxy = new Proxy(base, {});
    const bound = proxy.bind(null);
    const flags = getFeatures(bound);
    assert.equal(flags.isBound, true);
    assert.equal(flags.isProxy, true);
    assert.equal(flags.origin, base);
  });

  it('proxy object', () => {
    const target = { x: 1 };
    const proxy = new Proxy(target, {});
    // getFeatures should throw for non-function
    assert.throws(() => getFeatures(proxy));
  });
});
