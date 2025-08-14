import { it, describe } from 'node:test';
import assert from 'node:assert';
import { protoToString } from '../lib/index.mjs';

describe('protoToString', () => {
  it('returns native toString for function', () => {
    function foo() {}
    const str = protoToString(foo);
    assert.ok(str.startsWith('function foo'));
  });

  it('returns native toString for class', () => {
    class Bar {}
    const str = protoToString(Bar);
    assert.ok(str.startsWith('class Bar'));
  });

  it('not affected by user override', () => {
    function baz() {}
    baz.toString = () => 'fake';
    const str = protoToString(baz);
    assert.ok(str.startsWith('function baz'));
  });
});
