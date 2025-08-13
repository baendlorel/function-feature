import { it, describe } from 'node:test';
import assert from 'node:assert';
import { getBoundFunction } from '../lib/index.mjs';

describe('getBoundFunction', () => {
  it('returns original for bound', () => {
    function bar() {}
    const bound1 = bar.bind(null);
    const bound2 = bound1.bind(null);
    assert.equal(getBoundFunction(bound1), bar);
    assert.notEqual(getBoundFunction(bound2), getBoundFunction(bound1));
  });

  it('returns undefined for non-bound', () => {
    function baz() {}
    assert.equal(getBoundFunction(baz), undefined);
  });
});
