#include <assert.h>
#include <ctype.h>
#include <string.h>
#include "math_syntax.h"
#include "utilib.h"

//----------------------------------------------------------------------------- 
//! @addtogroup MATH_CONSTANTS
//! @{

bool isConstant(double value)
{
    for (size_t i = 0; i < CONSTANTS_COUNT; i++)
    {
        if (dcompare(value, CONSTANTS[i].value) == 0) 
        { 
            return true; 
        }
    }

    return false;
}

const char* getConstantName(double constant)
{
    for (size_t i = 0; i < CONSTANTS_COUNT; i++)
    {
        if (dcompare(constant, CONSTANTS[i].value) == 0) 
        { 
            return CONSTANTS[i].name; 
        }
    }

    return nullptr;
}

//! @}
//-----------------------------------------------------------------------------

//----------------------------------------------------------------------------- 
//! @addtogroup MATH_OPERATIONS
//! @{

bool isOperationUnary(Operation operation)
{
    return operation >= UNARY_OPERATIONS_START;
}

bool isArithmeticOp(Operation operation)
{
    return operation >= OP_ADD && operation <= OP_DIV;
}

bool isTrigOp(Operation operation)
{
    return operation >= OP_SIN && operation <= OP_TAN;
}

double evaluateUnary(Operation operation, double arg)
{
    assert(isOperationUnary(operation));

    switch (operation)
    {
        case OP_LOG: return log(arg); 
        case OP_EXP: return exp(arg);
        case OP_SIN: return sin(arg);
        case OP_COS: return cos(arg);
        case OP_TAN: return tan(arg);

        default: return NAN;
    }

    return NAN;
}

double evaluateBinary(Operation operation, double arg1, double arg2)
{
    assert(!isOperationUnary(operation));
    assert(operation != OP_INVALID);

    switch (operation)
    {
        case OP_ADD: return arg1 + arg2; 
        case OP_SUB: return arg1 - arg2;
        case OP_MUL: return arg1 * arg2;
        case OP_DIV: return arg1 / arg2;
        case OP_POW: return pow(arg1, arg2);

        default: return NAN;
    }

    return NAN;
}

//! @}
//-----------------------------------------------------------------------------

//----------------------------------------------------------------------------- 
//! @addtogroup MATH_VARIABLES
//! @{

bool isVariable(char symbol)
{
    for (size_t i = 0; i < CONSTANTS_COUNT; i++)
    {
        if (CONSTANTS[i].nameLength == 1 && CONSTANTS[i].name[0] == symbol)
        {
            return false;
        }
    }

    return isalpha(symbol) && strchr(INVALID_VARIABLE_SYMBOLS, symbol) == nullptr;
}

//! @}
//-----------------------------------------------------------------------------