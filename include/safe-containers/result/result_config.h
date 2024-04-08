#pragma once

// Replaces exceptions with asserts when accessing
// the value of a result that has an error.
#define RESULT_DISABLE_EXCEPTIONS 1

// Disables implicit bool coercion of result types.
#define RESULT_DISABLE_BOOL_COERCION 1