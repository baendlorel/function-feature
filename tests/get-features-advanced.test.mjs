import { describe, it, expect } from 'vitest';
import { getFeatures } from '../lib/index.mjs';

describe('getFeatures advanced', () => {
  it('proxy function', () => {
    function base() {}
    const proxy = new Proxy(base, {});
    const flags = getFeatures(proxy);
    expect(flags.isProxy).toBe(true);
    expect(flags.origin).toBe(base);
  });

  it('bound proxy function', () => {
    function base() {}
    const proxy = new Proxy(base, {});
    const bound = proxy.bind(null);
    const flags = getFeatures(bound);
    expect(flags.isBound).toBe(true);
    expect(flags.isProxy).toBe(true);
    expect(flags.origin).toBe(base);
  });

  it('proxy object throws', () => {
    const target = { x: 1 };
    const proxy = new Proxy(target, {});
    expect(() => getFeatures(proxy)).toThrow();
  });
});
