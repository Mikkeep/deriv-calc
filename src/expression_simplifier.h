#pragma once

#include "expression_tree.h"

//----------------------------------------------------------------------------- 
//! @defgroup MATH_SIMPIFYING Simplifying specification
//! @addtogroup MATH_SIMPIFYING
//! @{

enum SimplifyArgTarget
{
    SAT_FST,
    SAT_SND,
    SAT_ANY
};

struct SimplifyExpr
{
    Operation         operation;
    SimplifyArgTarget target;
    double            arg;
    double            result;
};

static const size_t SIMPLIFY_EXPRS_COUNT   = 13;
static const double SIMPLIFY_EXPR_IDENTITY = NAN;
#define isIdentityType(simplifyExpr) isnan(simplifyExpr.result)

static const SimplifyExpr SIMPLIFY_EXPRS[SIMPLIFY_EXPRS_COUNT] = 
                            { 
                                {OP_ADD, SAT_ANY, 0.0,     SIMPLIFY_EXPR_IDENTITY},
                                {OP_SUB, SAT_SND, 0.0,     SIMPLIFY_EXPR_IDENTITY},

                                {OP_MUL, SAT_ANY, 1.0,     SIMPLIFY_EXPR_IDENTITY},
                                {OP_MUL, SAT_ANY, 0.0,     0.0                   },
                                
                                {OP_DIV, SAT_SND, 1.0,     SIMPLIFY_EXPR_IDENTITY},
                                {OP_DIV, SAT_FST, 0.0,     0.0},

                                {OP_POW, SAT_SND, 1.0,     SIMPLIFY_EXPR_IDENTITY},
                                {OP_POW, SAT_SND, 0.0,     1.0                   },
                                {OP_POW, SAT_FST, 1.0,     1.0                   },

                                {OP_LOG, SAT_SND, 1.0,     0.0                   },
                                {OP_LOG, SAT_SND, E_CONST, 1.0                   },

                                {OP_EXP, SAT_SND, 0.0,     1.0                   },
                                {OP_EXP, SAT_SND, 1,       E_CONST               }
                            };

//! @}
//-----------------------------------------------------------------------------

void simplifyTree(ExprTree* tree);
