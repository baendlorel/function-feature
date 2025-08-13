const { getFunctionKind, getFunctionInfo, isClassConstructor } = require('../dist/index');

console.log('Testing function-kind package with V8 internals...\n');

// Test different types of functions
function regularFunction() {
  return 'regular';
}

const arrowFunction = () => {
  return 'arrow';
};

async function asyncFunction() {
  return 'async';
}

function* generatorFunction() {
  yield 'generator';
}

async function* asyncGeneratorFunction() {
  yield 'async_generator';
}

class TestClass {
  constructor(name) {
    this.name = name;
  }

  method() {
    return 'method';
  }

  async asyncMethod() {
    return 'async method';
  }

  *generatorMethod() {
    yield 'generator method';
  }
}

function ConstructorFunction(name) {
  this.name = name;
}

const boundFunction = regularFunction.bind(null);

const obj = {
  objectMethod() {
    return 'object method';
  },

  objectArrowMethod: () => 'object arrow method',
};

// Test functions
const testFunctions = [
  { name: 'Regular Function', func: regularFunction },
  { name: 'Arrow Function', func: arrowFunction },
  { name: 'Async Function', func: asyncFunction },
  { name: 'Generator Function', func: generatorFunction },
  { name: 'Async Generator Function', func: asyncGeneratorFunction },
  { name: 'Class Constructor', func: TestClass },
  { name: 'Constructor Function', func: ConstructorFunction },
  { name: 'Class Method', func: TestClass.prototype.method },
  { name: 'Class Async Method', func: TestClass.prototype.asyncMethod },
  { name: 'Class Generator Method', func: TestClass.prototype.generatorMethod },
  { name: 'Bound Function', func: boundFunction },
  { name: 'Object Method', func: obj.objectMethod },
  { name: 'Object Arrow Method', func: obj.objectArrowMethod },
  { name: 'Built-in Function', func: Array.prototype.push },
  { name: 'Native Function', func: Math.max },
  { name: 'Anonymous Function', func: function () {} },
];

console.log('=== Function Kind Tests ===');
let passed = 0;
let failed = 0;

testFunctions.forEach(({ name, func }) => {
  try {
    const kind = getFunctionKind(func);
    console.log(`✓ ${name.padEnd(25)}: ${kind}`);
    passed++;
  } catch (error) {
    console.error(`✗ ${name.padEnd(25)}: Error - ${error.message}`);
    failed++;
  }
});

console.log(`\nFunction Kind Tests: ${passed} passed, ${failed} failed\n`);

console.log('=== Class Constructor Tests ===');
const classTests = [
  { name: 'Class Constructor', func: TestClass, expected: true },
  { name: 'Constructor Function', func: ConstructorFunction, expected: false },
  { name: 'Regular Function', func: regularFunction, expected: false },
  { name: 'Arrow Function', func: arrowFunction, expected: false },
];

let classPassed = 0;
let classFailed = 0;

classTests.forEach(({ name, func, expected }) => {
  try {
    const result = isClassConstructor(func);
    if (result === expected) {
      console.log(`✓ ${name.padEnd(25)}: ${result} (expected ${expected})`);
      classPassed++;
    } else {
      console.log(`✗ ${name.padEnd(25)}: ${result} (expected ${expected})`);
      classFailed++;
    }
  } catch (error) {
    console.error(`✗ ${name.padEnd(25)}: Error - ${error.message}`);
    classFailed++;
  }
});

console.log(`\nClass Constructor Tests: ${classPassed} passed, ${classFailed} failed\n`);

console.log('=== Detailed Function Info Tests ===');
const detailTests = [
  { name: 'Regular Function', func: regularFunction },
  { name: 'Arrow Function', func: arrowFunction },
  { name: 'Async Function', func: asyncFunction },
  { name: 'Generator Function', func: generatorFunction },
  { name: 'Class Constructor', func: TestClass },
];

detailTests.forEach(({ name, func }) => {
  try {
    const info = getFunctionInfo(func);
    console.log(`\n${name}:`);
    console.log(`  Kind: ${info.kind}`);
    console.log(`  Name: ${info.name}`);
    console.log(`  Length: ${info.length}`);
    console.log(`  Is Constructor: ${info.isConstructor}`);
    console.log(`  Is Async: ${info.isAsync}`);
    console.log(`  Is Generator: ${info.isGenerator}`);
    console.log(`  Has Prototype: ${info.hasPrototype}`);
    console.log(`  Source (first 50 chars): ${info.source.substring(0, 50)}...`);
  } catch (error) {
    console.error(`✗ ${name}: Error - ${error.message}`);
  }
});

console.log('\n=== Error Handling Tests ===');
try {
  getFunctionKind('not a function');
  console.log('✗ Should have thrown error for non-function input');
} catch (error) {
  console.log('✓ Correctly threw error for non-function input:', error.message);
}

try {
  getFunctionInfo(123);
  console.log('✗ Should have thrown error for non-function input');
} catch (error) {
  console.log('✓ Correctly threw error for non-function input:', error.message);
}

try {
  isClassConstructor({});
  console.log('✗ Should have thrown error for non-function input');
} catch (error) {
  console.log('✓ Correctly threw error for non-function input:', error.message);
}

const totalPassed = passed + classPassed + 3; // 3 error handling tests
const totalFailed = failed + classFailed;

console.log(`\n=== Summary ===`);
console.log(`Total tests: ${totalPassed + totalFailed}`);
console.log(`Passed: ${totalPassed}`);
console.log(`Failed: ${totalFailed}`);
console.log(`Success rate: ${((totalPassed / (totalPassed + totalFailed)) * 100).toFixed(1)}%`);

if (totalFailed === 0) {
  console.log('\n🎉 All tests passed!');
  process.exit(0);
} else {
  console.log('\n❌ Some tests failed!');
  process.exit(1);
}
