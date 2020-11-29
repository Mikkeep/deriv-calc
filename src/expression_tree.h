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

bool      hasVariable       (ETNode* root, char variable);

void      setData           (ETNode* node, NodeType type, ETNodeData data);
void      setData           (ETNode* node, double number);
void      setData           (ETNode* node, char var);
void      setData           (ETNode* node, Operation op);

void      graphDump         (ExprTree* tree);
void      latexDump         (ExprTree* tree);