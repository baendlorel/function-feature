# function-feature

High-performance V8 native function feature detection for Node.js. Supports Node.js 16/18/20/22/24, auto-loads prebuilt binaries, no Nan/N-API dependency required.

## Installation

```bash
npm install function-feature
```

## Import

```js
import {
  getFeatures,
  getBound,
  getBoundOrigin,
  getOrigin,
  setName,
  protoToString,
  isClass,
} from 'function-feature';
```

## API Reference

### getFeatures(fn: Function): FunctionFeaturesResult

Analyze a JavaScript function using V8 internals and return its feature flags:

- `isConstructor`: true if the function is a constructor (can be called with `new`)
- `isAsyncFunction`: true if the function is an async function
- `isGeneratorFunction`: true if the function is a generator function
- `isProxy`: true if the function is a Proxy
- `isCallable`: true if the function is callable
- `isBound`: true if the function is a bound function (created by `Function.prototype.bind`)
- `isClass`: true if the function is a class (either user-defined or native)
- `origin`: the true origin function, unwrapped from bound/proxy

**Example:**

```js
getFeatures(function test(a, b) {});
// { isConstructor: false, isAsyncFunction: false, ..., origin: [Function: test] }
```

### getBound(fn: Function): Function | undefined

Get the original function that satisfies `fn = fn0.bind(thisArg, ...args)`. If not bound, returns itself.

**Example:**

```js
function test() {}
const bound = test.bind(null);
getBound(bound); // returns test
```

### getBoundOrigin(fn: Function): Function

If the function is bound multiple times, returns the most original unbound function; otherwise returns itself.

**Example:**

```js
const f0 = function () {};
const f1 = f0.bind(null, 1);
const f2 = f1.bind(null, 2);
getBoundOrigin(f2); // returns f0
```

### getOrigin(fn: Function): Function

Get the true origin function, tracing through both bound and proxy wrappers. If not wrapped, returns itself.

**Example:**

```js
const f0 = function () {};
const proxy = new Proxy(f0, {});
const bound = proxy.bind(null);
getOrigin(bound); // returns f0
```

### setName(fn: Function, name: string): Function

Set the name of a function. Only works if the `name` property is configurable. Class/arrow/native functions cannot be changed.

**Example:**

```js
function foo() {}
setName(foo, 'bar');
console.log(foo.name); // 'bar'
```

### protoToString(fn: Function): string

Equivalent to `Function.prototype.toString` but uses V8 internals to get the string representation. Safe and not affected by user overrides.

**Example:**

```js
protoToString(function foo() {});
```

### isClass(fn: Function): boolean

Check if a function is a class (either user-defined or native constructor).

- Must be a constructor
- Gets the original function (unwraps bound functions)
- Checks if it's a native constructor (Array, Object, etc.)
- Checks function.toString() for class syntax patterns

**Example:**

```js
isClass(Array); // true
isClass(class A {}); // true
isClass(function () {}); // false
```

## License

MIT
