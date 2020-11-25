#pragma once

#include <stdarg.h>

enum NodeType
{
    TYPE_INVALID = -1,

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

static const size_t OPERATIONS_COUNT = 10;
static const size_t UNARY_OPERATIONS_START = OP_LOG;
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

struct ExprNode
{
    NodeType type  = TYPE_CONST;
    double   value = 0;
};

typedef ExprNode BTElem_t;

struct BTNode;

struct BinaryTree
{
    BTNode* root = nullptr;
};

static const bool BT_TRAVERSE_RUN = true;

BinaryTree* construct         (BinaryTree* tree);
void        destroy           (BinaryTree* tree);

BinaryTree* newTree           ();
void        deleteTree        (BinaryTree* tree);

BTNode*     newNode           ();
BTNode*     newNode           (BTElem_t value);
void        deleteNode        (BTNode* node);

void        preOrderTraverse  (BTNode* subRoot, bool (*function)(BTNode* node, va_list args), ...);
void        inOrderTraverse   (BTNode* subRoot, bool (*function)(BTNode* node, va_list args), ...);
void        postOrderTraverse (BTNode* subRoot, bool (*function)(BTNode* node, va_list args), ...);

BTNode*     getRoot           (BinaryTree* tree);
void        setRoot           (BinaryTree* tree, BTNode* root);

BTElem_t    getValue          (BTNode* node);
BTNode*     getParent         (BTNode* node);
BTNode*     getLeft           (BTNode* node);
BTNode*     getRight          (BTNode* node);
bool        isLeft            (BTNode* node);

void        setValue          (BTNode* node, BTElem_t value);
void        setParent         (BTNode* node, BTNode* parent);
void        setLeft           (BTNode* node, BTNode* left);
void        setRight          (BTNode* node, BTNode* right);

