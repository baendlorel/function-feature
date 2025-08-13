import { it, describe } from 'node:test';
import assert from 'node:assert';
import { getFeatures } from 'function-feature';
describe('published', () => {
  it('test', () => {
    const a = () => {};
    const feat = getFeatures(a);
    assert.equal(feat.isConstructor, false);
    assert.equal(feat.isAsyncFunction, false);
  });
});
