# function-feature

> High-performance V8 native function feature detection for Node.js. Supports Node.js 16/18/20/22/24, auto-loads prebuilt binaries, no Nan/N-API dependency required.

## Installation

```bash
npm install function-feature
```

## Import

```js
import { getFeatures, getBound, setName } from 'function-feature';
```

## API Reference

### 1. getFeatures(fn)

- **Parameter**: `fn` Any JS function object
- **Returns**: An object containing the function's type features
- **Feature fields**:
  - `isConstructor` Whether it's a constructor (can be called with `new`)
  - `isAsyncFunction` Whether it's an async function
  - `isGeneratorFunction` Whether it's a generator function
  - `isProxy` Whether it's a proxy
  - `isCallable` Whether it's callable
  - `isBound` Whether it's a bound function (created by `Function.prototype.bind`)

**Example**:

```js
getFeatures(function test(a, b) {});
// { isConstructor: false, isAsyncFunction: false, ... }
```

### 2. getBound(fn)

- **Parameter**: `fn` Any JS function object
- **Returns**: If `fn` is a bound function (created by `bind`), returns the original unbound function; otherwise returns itself

**Example**:

```js
function test() {}
const bound = test.bind(null);
getBound(bound); // returns the original test function
```

### 3. setName(fn, name)

- **Parameter**: `fn` Any JS function object, `name` string
- **Returns**: The function after setting its name (modifies in place)
- **Note**: Only works for non-class/non-arrow functions. The `name` property of class/arrow functions cannot be changed.

**Example**:

```js
function foo() {}
setName(foo, 'bar');
console.log(foo.name); // 'bar'
```

## Features

- Supports multiple Node versions, auto-loads prebuilt binaries
- Pure V8 native API
- Compatible with ESM/CJS
- High performance, zero dependencies

## LICENSE

MIT
