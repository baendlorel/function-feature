import { createRequire } from 'module';
const require = createRequire(import.meta.url);
const lib = require('../build/Release/function_feature');

export const getFunctionKind = lib.getFunctionKind;
