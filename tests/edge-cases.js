const { getFunctionKind, getFunctionInfo, isClassConstructor } = require('../dist/index');

console.log('Running edge case tests for function-kind...\n');

// Edge case functions
const edgeCases = {
  // Nested functions
  outerFunction: function outerFunction() {
    function innerFunction() {
      return 'inner';
    }
    return innerFunction;
  },

  // Higher order functions
  higherOrder: function (callback) {
    return function () {
      return callback();
    };
  },

  // Immediately invoked function expression
  iife: (function () {
    return function namedIIFE() {
      return 'iife';
    };
  })(),

  // Function with complex prototype manipulation
  prototypeManipulated: function () {},

  // Async arrow function
  asyncArrow: async () => {
    return 'async arrow';
  },

  // Generator arrow (not valid syntax, but we can test error handling)
  // generatorArrow: () => * {},  // This would be a syntax error

  // Function created with Function constructor
  dynamicFunction: new Function('return "dynamic"'),

  // Bound async function
  boundAsync: null, // Will be set below

  // Class with static methods
  ClassWithStatic: class {
    static staticMethod() {
      return 'static';
    }

    static async staticAsyncMethod() {
      return 'static async';
    }
  },

  // Extended class
  ExtendedClass: class extends Array {
    constructor() {
      super();
    }

    customMethod() {
      return 'extended';
    }
  },
};

// Set up bound async function
async function asyncForBinding() {
  return 'bound async';
}
edgeCases.boundAsync = asyncForBinding.bind(null);

// Delete prototype from one function to test
delete edgeCases.prototypeManipulated.prototype;

console.log('=== Edge Case Function Kind Tests ===');

// Test nested function
console.log('\n1. Nested Functions:');
const innerFunc = edgeCases.outerFunction();
console.log(`Outer function: ${getFunctionKind(edgeCases.outerFunction)}`);
console.log(`Inner function: ${getFunctionKind(innerFunc)}`);

// Test higher order function
console.log('\n2. Higher Order Functions:');
const returnedFunc = edgeCases.higherOrder(() => 'test');
console.log(`Higher order function: ${getFunctionKind(edgeCases.higherOrder)}`);
console.log(`Returned function: ${getFunctionKind(returnedFunc)}`);

// Test IIFE result
console.log('\n3. IIFE Result:');
console.log(`IIFE result: ${getFunctionKind(edgeCases.iife)}`);

// Test prototype manipulated function
console.log('\n4. Prototype Manipulated:');
console.log(`Prototype deleted function: ${getFunctionKind(edgeCases.prototypeManipulated)}`);

// Test async arrow
console.log('\n5. Async Arrow:');
console.log(`Async arrow: ${getFunctionKind(edgeCases.asyncArrow)}`);

// Test dynamic function
console.log('\n6. Dynamic Function:');
console.log(`Function() constructor: ${getFunctionKind(edgeCases.dynamicFunction)}`);

// Test bound async
console.log('\n7. Bound Async:');
console.log(`Bound async function: ${getFunctionKind(edgeCases.boundAsync)}`);

// Test class with static methods
console.log('\n8. Class Static Methods:');
console.log(`Class constructor: ${getFunctionKind(edgeCases.ClassWithStatic)}`);
console.log(`Static method: ${getFunctionKind(edgeCases.ClassWithStatic.staticMethod)}`);
console.log(`Static async method: ${getFunctionKind(edgeCases.ClassWithStatic.staticAsyncMethod)}`);

// Test extended class
console.log('\n9. Extended Class:');
console.log(`Extended class: ${getFunctionKind(edgeCases.ExtendedClass)}`);
console.log(
  `Extended class method: ${getFunctionKind(edgeCases.ExtendedClass.prototype.customMethod)}`
);

console.log('\n=== Detailed Analysis of Complex Cases ===');

// Analyze the most interesting cases
const complexCases = [
  { name: 'Async Arrow', func: edgeCases.asyncArrow },
  { name: 'Dynamic Function', func: edgeCases.dynamicFunction },
  { name: 'Bound Async', func: edgeCases.boundAsync },
  { name: 'Extended Class', func: edgeCases.ExtendedClass },
];

complexCases.forEach(({ name, func }) => {
  try {
    const info = getFunctionInfo(func);
    console.log(`\n${name}:`);
    console.log(`  Kind: ${info.kind}`);
    console.log(`  Name: ${info.name}`);
    console.log(`  Is Constructor: ${info.isConstructor}`);
    console.log(`  Is Async: ${info.isAsync}`);
    console.log(`  Is Generator: ${info.isGenerator}`);
    console.log(`  Has Prototype: ${info.hasPrototype}`);
    console.log(`  Is Class Constructor: ${isClassConstructor(func)}`);
  } catch (error) {
    console.error(`  Error analyzing ${name}: ${error.message}`);
  }
});

console.log('\n=== Performance Test ===');

// Simple performance test
const testFunc = function testPerformance() {
  return 'test';
};
const iterations = 10000;

console.time('getFunctionKind performance');
for (let i = 0; i < iterations; i++) {
  getFunctionKind(testFunc);
}
console.timeEnd('getFunctionKind performance');

console.time('getFunctionInfo performance');
for (let i = 0; i < iterations; i++) {
  getFunctionInfo(testFunc);
}
console.timeEnd('getFunctionInfo performance');

console.log(`\nPerformed ${iterations} function analyses each`);

console.log('\n=== Memory Usage ===');
const used = process.memoryUsage();
for (let key in used) {
  console.log(`${key}: ${Math.round((used[key] / 1024 / 1024) * 100) / 100} MB`);
}

console.log('\nEdge case tests completed!');
