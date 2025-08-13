import { it, describe } from 'node:test';
import assert from 'node:assert';
import { getBoundFunction } from '../lib/index.mjs';

describe('getBoundFunction', () => {
  it('returns original for bound', () => {
    function bar() {}
    const bound = bar.bind(null);
    assert.equal(getBoundFunction(bound), bar);
  });

  it('returns undefined for non-bound', () => {
    function baz() {}
    assert.equal(getBoundFunction(baz), undefined);
  });
});
