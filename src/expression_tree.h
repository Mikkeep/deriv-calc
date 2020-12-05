#pragma once

#include <stdarg.h>
#include "math_syntax.h"

enum NodeType
{
    TYPE_INVALID = -1,

    TYPE_NUMBER,
    TYPE_VAR,
    TYPE_OP
};

union ETNodeData
{
    double    number;
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

static const bool ET_TRAVERSE_RUN = true;

#define UNARY_OP(operation, arg) *newNode(TYPE_OP, { .op = OP_##operation }, NULL,             (ETNode*) &(arg))
#define BINARY_OP(operation)     *newNode(TYPE_OP, { .op = OP_##operation }, (ETNode*) &tree1, (ETNode*) &tree2)

#define LOG(arg) UNARY_OP(LOG, arg)
#define EXP(arg) UNARY_OP(EXP, arg)
#define SIN(arg) UNARY_OP(SIN, arg)
#define COS(arg) UNARY_OP(COS, arg)
#define TAN(arg) UNARY_OP(TAN, arg)

#define NUM(num)      (*newNode(TYPE_NUMBER, { .number = num      }, nullptr, nullptr))
#define VAR(variable) (*newNode(TYPE_VAR,    { .var    = variable }, nullptr, nullptr))

ETNode&   operator +        (const ETNode& tree1, const ETNode& tree2);
ETNode&   operator -        (const ETNode& tree1, const ETNode& tree2);
ETNode&   operator *        (const ETNode& tree1, const ETNode& tree2);
ETNode&   operator /        (const ETNode& tree1, const ETNode& tree2);
ETNode&   operator ^        (const ETNode& tree1, const ETNode& tree2);

ExprTree* construct         (ExprTree* tree);
void      destroy           (ExprTree* tree);
void      destroySubtree    (ETNode* root);

ExprTree* newTree           ();
void      deleteTree        (ExprTree* tree);

ETNode*   newNode           ();
ETNode*   newNode           (NodeType type, ETNodeData data, ETNode* left, ETNode* right);
void      deleteNode        (ETNode* node);

void      copyNode          (ETNode* dest, const ETNode* src);
ETNode*   copyTree          (const ETNode* node);

void      preOrderTraverse  (ETNode* root, bool (*function)(ETNode* node, va_list args), ...);
void      inOrderTraverse   (ETNode* root, bool (*function)(ETNode* node, va_list args), ...);
void      postOrderTraverse (ETNode* root, bool (*function)(ETNode* node, va_list args), ...);

bool      isLeft            (const ETNode* node);
bool      isTypeNumber      (const ETNode* node);
bool      isTypeVar         (const ETNode* node);
bool      isTypeOp          (const ETNode* node);

double    evaluateSubtree   (ETNode* root);
void      substitute        (ETNode* root, char variable, double value);
bool      hasVariable       (ETNode* root, char variable);

void      setData           (ETNode* node, NodeType type, ETNodeData data);
void      setData           (ETNode* node, double number);
void      setData           (ETNode* node, char var);
void      setData           (ETNode* node, Operation op);

void      graphDump         (ExprTree* tree);
void      latexDump         (ExprTree* tree);