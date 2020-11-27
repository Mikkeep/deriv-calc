#pragma once

#include <math.h>
#include <stdarg.h>

#define UTB_DECLARATIONS_ONLY
#include "utilib.h"

enum NodeType
{
    TYPE_INVALID = -1,

    TYPE_NUMBER,
    TYPE_CONST,
    TYPE_VAR,
    TYPE_OP
};

enum Operation
{
    OP_INVALID = -1,

    /* Binary operations */
    OP_ADD,
    OP_SUB,
    OP_MUL,
    OP_DIV,
    
    OP_POW,

    /* Unary operations */
    OP_LOG, 
    OP_EXP, 

    OP_SIN, 
    OP_COS, 
    OP_TAN  
};

static const size_t OPERATIONS_COUNT             = 10;
static const int    UNARY_OPERATIONS_START       = OP_LOG;
static const char*  OPERATIONS[OPERATIONS_COUNT] = { 
                                                     "+",   
                                                     "-",   
                                                     "*", 
                                                     "/",

                                                     "^", 

                                                     "log", 
                                                     "exp",

                                                     "sin", 
                                                     "cos", 
                                                     "tan" 
                                                   };

// enum SimplifyType
// {
//     STYPE_ZERO,    
//     STYPE_ONE,    
//     STYPE_EULERS_N,    
//     STYPE_IDENTITY   
// };

union ETNodeData
{
    double    number;
    double    constant;
    char      var;
    Operation op;
};

struct ETNode
{
    NodeType   type   = TYPE_INVALID;
    ETNodeData data   = {};

    ETNode*    parent = nullptr;
    ETNode*    left   = nullptr;
    ETNode*    right  = nullptr;
};

struct ExprTree
{
    ETNode* root = nullptr;
};

static const double PI_CONST = acos(-1);
static const double E_CONST  = exp(1);

struct SimplifyExpr
{
    Operation operation;
    bool      commutative;
    double    arg;

    double    result;
};

static const size_t SIMPLIFY_EXPRS_COUNT   = 11;
static const double SIMPLIFY_EXPR_IDENTITY = NAN;
#define isIdentityType(simplifyExpr) isnan(simplifyExpr.result)

static const SimplifyExpr SIMPLIFY_EXPRS[SIMPLIFY_EXPRS_COUNT] = 
                            { 
                                {OP_ADD, true,  0.0,     SIMPLIFY_EXPR_IDENTITY},
                                {OP_SUB, false, 0.0,     SIMPLIFY_EXPR_IDENTITY},

                                {OP_MUL, true,  1.0,     SIMPLIFY_EXPR_IDENTITY},
                                {OP_MUL, true,  0.0,     0.0                   },
                                {OP_DIV, false, 1.0,     SIMPLIFY_EXPR_IDENTITY},

                                {OP_POW, false, 1.0,     SIMPLIFY_EXPR_IDENTITY},
                                {OP_POW, false, 0.0,     1.0                   },

                                {OP_LOG, false, 1.0,     0.0                   },
                                {OP_LOG, false, E_CONST, 1.0                   },

                                {OP_EXP, false, 0.0,     1.0                   },
                                {OP_EXP, false, 1,       E_CONST               },
                            };


static const bool ET_TRAVERSE_RUN = true;

ExprTree* construct         (ExprTree* tree);
void      destroy           (ExprTree* tree);
void      destroySubtree    (ETNode* root);

ExprTree* newTree           ();
void      deleteTree        (ExprTree* tree);

ETNode*   newNode           ();
ETNode*   newNode           (NodeType type, ETNodeData data, ETNode* left, ETNode* right);
void      deleteNode        (ETNode* node);

ETNode*   copyTree          (const ETNode* node);

void      preOrderTraverse  (ETNode* root, bool (*function)(ETNode* node, va_list args), ...);
void      inOrderTraverse   (ETNode* root, bool (*function)(ETNode* node, va_list args), ...);
void      postOrderTraverse (ETNode* root, bool (*function)(ETNode* node, va_list args), ...);

bool      isOperationUnary  (Operation operation);

bool      isLeft            (const ETNode* node);
bool      isNumeric         (const ETNode* node);
bool      isTypeNumber      (const ETNode* node);
bool      isTypeConst       (const ETNode* node);
bool      isTypeVar         (const ETNode* node);
bool      isTypeOp          (const ETNode* node);

bool      isArithmeticOp    (Operation op);  
bool      isTrigOp          (Operation op);  

bool      isConstant        (double value);  

void      setData           (ETNode* node, NodeType type, ETNodeData data);
void      setData           (ETNode* node, double number);
void      setData           (ETNode* node, char var);
void      setData           (ETNode* node, Operation op);

const char* getConstantName(double constant);