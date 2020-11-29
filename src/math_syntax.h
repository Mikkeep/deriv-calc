#pragma once

#include <math.h>

//----------------------------------------------------------------------------- 
//! @defgroup MATH_CONSTANTS Constants specification
//! @addtogroup MATH_CONSTANTS
//! @{

static const double PI_CONST = acos(-1);
static const double E_CONST  = exp(1);

bool        isConstant      (double value);  
const char* getConstantName (double constant);

//! @}
//-----------------------------------------------------------------------------

//----------------------------------------------------------------------------- 
//! @defgroup MATH_OPERATIONS Supporting operations
//! @addtogroup MATH_OPERATIONS
//! @{

enum Operation
{
    OP_INVALID = -1,

    /* Binary operations */
    OP_ADD, OP_SUB, OP_MUL, OP_DIV,
    OP_POW,

    /* Unary operations */
    OP_LOG, OP_EXP, 
    OP_SIN, OP_COS, OP_TAN  
};

static const size_t OPERATIONS_COUNT             = 10;
static const int    UNARY_OPERATIONS_START       = OP_LOG;
static const char*  OPERATIONS[OPERATIONS_COUNT] = 
                        { 
                            "+", "-", "*", "/",
                            "^", 
                            "log", "exp",
                            "sin", "cos", "tan" 
                        };

bool   isOperationUnary (Operation operation);
bool   isArithmeticOp   (Operation operation);  
bool   isTrigOp         (Operation operation);

double evaluateUnary    (Operation operation, double arg);
double evaluateBinary   (Operation operation, double arg1, double arg2);

//! @}
//-----------------------------------------------------------------------------