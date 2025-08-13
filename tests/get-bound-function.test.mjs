import { it, describe } from 'node:test';
import assert from 'node:assert';
import { getBound } from '../lib/index.mjs';

describe('getBound', () => {
  it('returns original for bound', () => {
    function bar() {}
    const bound1 = bar.bind(null);
    const bound2 = bound1.bind(null);
    assert.equal(getBound(bound1), bar);
    assert.notEqual(getBound(bound2), getBound(bound1));
  });

  it('returns undefined for non-bound', () => {
    function baz() {}
    assert.equal(getBound(baz), undefined);
  });
});
