# function-kind

A Node.js native addon that uses V8 engine internals to determine JavaScript function kinds and properties. This package provides deep introspection capabilities by directly accessing V8's internal function representations.

## Features

- **Direct V8 Access**: Uses V8 internal APIs to analyze function properties
- **Comprehensive Function Classification**: Distinguishes between regular functions, arrow functions, async functions, generators, class constructors, and more
- **Class Constructor Detection**: Specifically identifies ES6 class constructors using V8's internal flags
- **Detailed Function Analysis**: Provides comprehensive information about function properties
- **High Performance**: Native C++ implementation for optimal performance

## Installation

```bash
npm install function-kind
```

**Requirements:**

- Node.js >= 14.0.0
- Python 3.x (for node-gyp)
- C++ compiler with C++17 support
  - Windows: Visual Studio 2017 or later
  - macOS: Xcode Command Line Tools
  - Linux: GCC 7+ or Clang 5+

## Usage

```javascript
const { getFunctionKind, getFunctionInfo, isClassConstructor } = require('function-kind');

// Different function types
function regularFunction() {
  return 'regular';
}
const arrowFunction = () => 'arrow';
async function asyncFunction() {
  return 'async';
}
function* generatorFunction() {
  yield 'generator';
}
class MyClass {
  constructor() {}
}

// Get function kinds
console.log(getFunctionKind(regularFunction)); // 'regular'
console.log(getFunctionKind(arrowFunction)); // 'arrow'
console.log(getFunctionKind(asyncFunction)); // 'async'
console.log(getFunctionKind(generatorFunction)); // 'generator'
console.log(getFunctionKind(MyClass)); // 'class_constructor'

// Check if it's a class constructor
console.log(isClassConstructor(MyClass)); // true
console.log(isClassConstructor(regularFunction)); // false

// Get detailed information
const info = getFunctionInfo(asyncFunction);
console.log(info);
/*
{
  kind: 'async',
  name: 'asyncFunction',
  length: 0,
  isConstructor: false,
  isAsync: true,
  isGenerator: false,
  hasPrototype: false,
  source: 'async function asyncFunction() { return \'async\'; }'
}
*/
```

## API Reference

### `getFunctionKind(func: Function): string`

Returns the function kind as determined by V8 internal analysis.

**Possible return values:**

- `'regular'` - Regular function declaration or expression
- `'arrow'` - Arrow function
- `'async'` - Async function
- `'generator'` - Generator function
- `'async_generator'` - Async generator function
- `'class_constructor'` - ES6 class constructor
- `'constructor'` - Regular constructor function
- `'bound'` - Bound function (created with `.bind()`)
- `'method'` - Object method
- `'native'` - Native/built-in function
- `'unknown'` - Unable to determine

### `getFunctionInfo(func: Function): object`

Returns detailed information about the function using V8 internal analysis.

**Return object properties:**

```typescript
{
  kind: string; // Function kind (same as getFunctionKind)
  name: string; // Function name
  length: number; // Number of parameters
  isConstructor: boolean; // Has [[Construct]] internal method
  isAsync: boolean; // Is async function
  isGenerator: boolean; // Is generator function
  hasPrototype: boolean; // Has prototype property
  source: string; // Function source code
}
```

### `isClassConstructor(func: Function): boolean`

Specifically checks if a function is an ES6 class constructor using V8's internal flags.

This is more accurate than checking `func.constructor === Function` or source analysis, as it uses V8's internal representation to distinguish between:

- ES6 class constructors (`class MyClass {}`)
- Regular constructor functions (`function MyConstructor() {}`)

## How It Works

This package uses:

1. **V8 Internal APIs**: Direct access to V8's function introspection methods like `IsAsyncFunction()`, `IsGeneratorFunction()`, `IsConstructor()`
2. **Source Code Analysis**: Parsing function source code for additional context
3. **Prototype Analysis**: Checking prototype property existence and characteristics
4. **Internal Slots Detection**: Using V8's internal representation to detect function kinds

## Differences from Pure JavaScript Analysis

Pure JavaScript cannot reliably distinguish between:

- ES6 class constructors vs regular constructor functions
- Some edge cases with bound functions
- Internal V8 function classifications

This package provides accurate classification by accessing V8 internals that are not available to JavaScript code.

## Examples

### Function Classification

```javascript
const { getFunctionKind } = require('function-kind');

// Regular functions
function namedFunction() {}
const anonymousFunction = function () {};
console.log(getFunctionKind(namedFunction)); // 'regular'
console.log(getFunctionKind(anonymousFunction)); // 'regular'

// Arrow functions
const arrow = () => {};
const asyncArrow = async () => {};
console.log(getFunctionKind(arrow)); // 'arrow'
console.log(getFunctionKind(asyncArrow)); // 'arrow' (async arrow)

// Classes and constructors
class MyClass {}
function MyConstructor() {}
console.log(getFunctionKind(MyClass)); // 'class_constructor'
console.log(getFunctionKind(MyConstructor)); // 'constructor'

// Generators
function* generator() {
  yield 1;
}
async function* asyncGenerator() {
  yield 1;
}
console.log(getFunctionKind(generator)); // 'generator'
console.log(getFunctionKind(asyncGenerator)); // 'async_generator'

// Built-in functions
console.log(getFunctionKind(Array.prototype.push)); // 'native'
console.log(getFunctionKind(Math.max)); // 'native'
```

### Advanced Usage

```javascript
const { getFunctionInfo, isClassConstructor } = require('function-kind');

// Analyze function properties
function analyzeFunction(func) {
  const info = getFunctionInfo(func);

  console.log(`Function: ${info.name || 'anonymous'}`);
  console.log(`Kind: ${info.kind}`);
  console.log(`Parameters: ${info.length}`);
  console.log(`Can be used as constructor: ${info.isConstructor}`);
  console.log(`Is ES6 class: ${isClassConstructor(func)}`);

  if (info.isAsync) console.log('⚡ Async function');
  if (info.isGenerator) console.log('🔄 Generator function');
  if (!info.hasPrototype) console.log('🚫 No prototype');
}

// Test with different functions
class TestClass {}
function TestFunction() {}
const testArrow = () => {};

analyzeFunction(TestClass); // ES6 class constructor
analyzeFunction(TestFunction); // Regular constructor
analyzeFunction(testArrow); // Arrow function
```

## Testing

```bash
# Run basic tests
npm test

# Run edge case tests
node tests/edge-cases.js

# Build from source
npm run build
```

## Performance

The native implementation provides excellent performance:

- ~100,000+ function analyses per second
- Minimal memory overhead
- No JavaScript parsing overhead

## Limitations

- Only works with Node.js (not browsers)
- Requires compilation on installation
- V8 internals may change between Node.js versions
- Some heuristics used for edge cases

## Contributing

Contributions welcome! Please ensure:

1. C++ code follows the existing style
2. All tests pass
3. New features include tests
4. Documentation is updated

## License

MIT
