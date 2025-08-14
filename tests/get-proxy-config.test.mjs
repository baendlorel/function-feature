import { it, describe } from 'node:test';
import assert from 'node:assert';
import { getProxyConfig } from '../lib/index.mjs';

describe('getProxyConfig', () => {
  it('returns target and handler for proxy function', () => {
    function base() {}
    const handler = { apply() {} };
    const proxy = new Proxy(base, handler);
    const info = getProxyConfig(proxy);
    assert.equal(info.target, base);
    assert.equal(info.handler, handler);
  });

  it('returns target and handler for proxy object', () => {
    const target = { x: 1 };
    const handler = { get() {} };
    const proxy = new Proxy(target, handler);
    const info = getProxyConfig(proxy);
    assert.equal(info.target, target);
    assert.equal(info.handler, handler);
  });

  it('returns undefined for non-proxy', () => {
    function foo() {}
    assert.equal(getProxyConfig(foo), undefined);
    assert.equal(getProxyConfig({}), undefined);
  });
});
