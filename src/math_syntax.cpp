#include <assert.h>
#include "math_syntax.h"
#include "utilib.h"

//----------------------------------------------------------------------------- 
//! @addtogroup MATH_CONSTANTS
//! @{

bool isConstant(double value)
{
    return dcompare(value, E_CONST)  == 0 || 
           dcompare(value, PI_CONST) == 0;
}

const char* getConstantName(double constant)
{
    if      (dcompare(constant, E_CONST)  == 0) { return "e"; }   
    else if (dcompare(constant, PI_CONST) == 0) { return "pi"; }  

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