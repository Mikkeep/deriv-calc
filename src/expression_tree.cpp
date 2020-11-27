#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "expression_tree.h"

#define CHECK_NULL(value, action) if (value == nullptr) { action; }

bool deleteNode        (ETNode* node, va_list args);

bool preOrderTraverse  (ETNode* root, bool (*function)(ETNode* node, va_list args), va_list args);
bool inOrderTraverse   (ETNode* root, bool (*function)(ETNode* node, va_list args), va_list args);
bool postOrderTraverse (ETNode* root, bool (*function)(ETNode* node, va_list args), va_list args);

ExprTree* construct(ExprTree* tree)
{
    CHECK_NULL(tree, return nullptr);

    tree->root = nullptr;

    return tree;
}

ExprTree* newTree()
{
    ExprTree* tree = (ExprTree*) calloc(1, sizeof(ExprTree));
    CHECK_NULL(tree, return nullptr);

    return construct(tree);
}

void destroy(ExprTree* tree)
{
    assert(tree != nullptr);

    if (tree->root != nullptr) { destroySubtree(tree->root); }

    tree->root = nullptr;
}

void destroySubtree(ETNode* root)
{
    assert(root != nullptr);

    postOrderTraverse(root, &deleteNode);
}

void deleteTree(ExprTree* tree)
{
    assert(tree != nullptr);

    destroy(tree);
    free(tree);
}

ETNode* newNode()
{
    return (ETNode*) calloc(1, sizeof(ETNode));
}

ETNode* newNode(NodeType type, ETNodeData data, ETNode* left, ETNode* right)
{
    ETNode* node = newNode();
    CHECK_NULL(node, return nullptr);

    node->type    = type;
    node->data    = data;

    node->parent  = nullptr;
    node->left    = left;
    node->right   = right;

    if (left  != nullptr) { left->parent  = node; }
    if (right != nullptr) { right->parent = node; }

    return node;
}

void deleteNode(ETNode* node)
{
    assert(node != nullptr);

    node->parent = nullptr;
    node->left   = nullptr;
    node->right  = nullptr;

    free(node);
}

bool deleteNode(ETNode* node, va_list args)
{
    deleteNode(node);

    return ET_TRAVERSE_RUN;
}

ETNode* copyTree(const ETNode* node)
{
    if (node == nullptr) { return nullptr; }

    return newNode(node->type, node->data, copyTree(node->left), copyTree(node->right));
}

#define TRAVERSE_SUBTREE(traverse, side) if (traverse(root->side, function, args)  == !ET_TRAVERSE_RUN) \
                                         {                                                                 \
                                             return !ET_TRAVERSE_RUN;                                      \
                                         }

void preOrderTraverse(ETNode* root, bool (*function)(ETNode* node, va_list args), ...)
{
    CHECK_NULL(root, return);

    va_list args = {};
    va_start(args, &function);

    preOrderTraverse(root, function, args);

    va_end(args);
}

bool preOrderTraverse(ETNode* root, bool (*function)(ETNode* node, va_list args), va_list args)
{
    CHECK_NULL(root, return ET_TRAVERSE_RUN);

    if (function(root, args) == !ET_TRAVERSE_RUN) { return !ET_TRAVERSE_RUN; };

    TRAVERSE_SUBTREE(preOrderTraverse, left);
    TRAVERSE_SUBTREE(preOrderTraverse, right);

    return ET_TRAVERSE_RUN;
}

void inOrderTraverse(ETNode* root, bool (*function)(ETNode* node, va_list args), ...)
{
    CHECK_NULL(root, return);

    va_list args = {};
    va_start(args, &function);

    inOrderTraverse(root, function, args);

    va_end(args);
}

bool inOrderTraverse(ETNode* root, bool (*function)(ETNode* node, va_list args), va_list args)
{
    CHECK_NULL(root, return ET_TRAVERSE_RUN);

    TRAVERSE_SUBTREE(inOrderTraverse, left);

    if (function(root, args) == !ET_TRAVERSE_RUN) { return !ET_TRAVERSE_RUN; };

    TRAVERSE_SUBTREE(inOrderTraverse, right);

    return ET_TRAVERSE_RUN;
}

void postOrderTraverse(ETNode* root, bool (*function)(ETNode* node, va_list args), ...)
{
    CHECK_NULL(root, return);

    va_list args = {};
    va_start(args, &function);

    postOrderTraverse(root, function, args);

    va_end(args);
}

bool postOrderTraverse(ETNode* root, bool (*function)(ETNode* node, va_list args), va_list args)
{
    CHECK_NULL(root, return ET_TRAVERSE_RUN);

    TRAVERSE_SUBTREE(postOrderTraverse, left);
    TRAVERSE_SUBTREE(postOrderTraverse, right);

    if (function(root, args) == !ET_TRAVERSE_RUN) { return !ET_TRAVERSE_RUN; };

    return ET_TRAVERSE_RUN;
}

bool isOperationUnary(Operation operation)
{
    return operation >= UNARY_OPERATIONS_START;
}

bool isLeft(const ETNode* node)
{
    assert(node         != nullptr);
    assert(node->parent != nullptr);

    return node == node->parent->left;
}

bool isNumeric(const ETNode* node)
{
    assert(node != nullptr);

    return isTypeNumber(node) || isTypeConst(node);
}

bool isTypeNumber(const ETNode* node)
{
    assert(node != nullptr);

    return node->type == TYPE_NUMBER;
}

bool isTypeConst(const ETNode* node)
{
    assert(node != nullptr);

    return node->type == TYPE_CONST;
}

bool isTypeVar(const ETNode* node)
{
    assert(node != nullptr);

    return node->type == TYPE_VAR;
}

bool isTypeOp(const ETNode* node)
{
    assert(node != nullptr);

    return node->type == TYPE_OP;
}

bool isArithmeticOp(Operation op)
{
    return op >= OP_ADD && op <= OP_DIV;
}

bool isTrigOp(Operation op)
{
    return op >= OP_SIN && op <= OP_TAN;
}

bool isConstant(double value)
{
    return compare(value, E_CONST)  == 0 || 
           compare(value, PI_CONST) == 0;
}

void setData(ETNode* node, NodeType type, ETNodeData data)
{
    assert(node != nullptr);

    node->type = type;
    node->data = data;
}

void setData(ETNode* node, double number)
{
    assert(node != nullptr);

    if (isConstant(number)) { node->type = TYPE_CONST; }
    else                    { node->type = TYPE_NUMBER; }

    node->data.number = number;
}

void setData(ETNode* node, char var)
{
    assert(node != nullptr);

    node->type     = TYPE_VAR;
    node->data.var = var;
}

void setData(ETNode* node, Operation op)
{
    assert(node != nullptr);

    node->type    = TYPE_OP;
    node->data.op = op;
}

const char* getConstantName(double constant)
{
    if      (compare(constant, E_CONST)  == 0) { return "e"; }   
    else if (compare(constant, PI_CONST) == 0) { return "pi"; }  

    return "NO_SUCH_CONSTANT"; 
}