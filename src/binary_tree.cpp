#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "binary_tree.h"

#define CHECK_NULL(value, action) if (value == nullptr) { action; }

struct BTNode
{
    BTElem_t value;

    BTNode* parent = nullptr;
    BTNode* left   = nullptr;
    BTNode* right  = nullptr;
};


bool deleteNode        (BTNode* node, va_list args);

bool preOrderTraverse  (BTNode* subRoot, bool (*function)(BTNode* node, va_list args), va_list args);
bool inOrderTraverse   (BTNode* subRoot, bool (*function)(BTNode* node, va_list args), va_list args);
bool postOrderTraverse (BTNode* subRoot, bool (*function)(BTNode* node, va_list args), va_list args);

BinaryTree* construct(BinaryTree* tree)
{
    CHECK_NULL(tree, return nullptr);

    tree->root = nullptr;

    return tree;
}

BinaryTree* newTree()
{
    BinaryTree* tree = (BinaryTree*) calloc(1, sizeof(BinaryTree));
    CHECK_NULL(tree, return nullptr);

    return construct(tree);
}

void destroy(BinaryTree* tree)
{
    assert(tree != nullptr);

    postOrderTraverse(tree->root, &deleteNode);

    tree->root = nullptr;
}

void deleteTree(BinaryTree* tree)
{
    assert(tree != nullptr);

    destroy(tree);
    free(tree);
}

BTNode* newNode()
{
    BTNode* node = (BTNode*) calloc(1, sizeof(BTNode));
    CHECK_NULL(node, return nullptr);

    return node;
}

BTNode* newNode(BTElem_t value)
{
    BTNode* node = newNode();
    CHECK_NULL(node, return nullptr);

    node->value = value;

    return node;
}

void deleteNode(BTNode* node)
{
    assert(node != nullptr);

    node->parent = nullptr;
    node->left   = nullptr;
    node->right  = nullptr;

    free(node);
}

bool deleteNode(BTNode* node, va_list args)
{
    deleteNode(node);

    return BT_TRAVERSE_RUN;
}

#define TRAVERSE_SUBTREE(traverse, side) if (traverse(subRoot->side, function, args)  == !BT_TRAVERSE_RUN) \
                                         {                                                                 \
                                             return !BT_TRAVERSE_RUN;                                      \
                                         }

void preOrderTraverse(BTNode* subRoot, bool (*function)(BTNode* node, va_list args), ...)
{
    CHECK_NULL(subRoot, return);

    va_list args = {};
    va_start(args, &function);

    preOrderTraverse(subRoot, function, args);

    va_end(args);
}

bool preOrderTraverse(BTNode* subRoot, bool (*function)(BTNode* node, va_list args), va_list args)
{
    CHECK_NULL(subRoot, return BT_TRAVERSE_RUN);

    if (function(subRoot, args) == !BT_TRAVERSE_RUN) { return !BT_TRAVERSE_RUN; };

    TRAVERSE_SUBTREE(preOrderTraverse, left);
    TRAVERSE_SUBTREE(preOrderTraverse, right);

    return BT_TRAVERSE_RUN;
}

void inOrderTraverse(BTNode* subRoot, bool (*function)(BTNode* node, va_list args), ...)
{
    CHECK_NULL(subRoot, return);

    va_list args = {};
    va_start(args, &function);

    inOrderTraverse(subRoot, function, args);

    va_end(args);
}

bool inOrderTraverse(BTNode* subRoot, bool (*function)(BTNode* node, va_list args), va_list args)
{
    CHECK_NULL(subRoot, return BT_TRAVERSE_RUN);

    TRAVERSE_SUBTREE(inOrderTraverse, left);

    if (function(subRoot, args) == !BT_TRAVERSE_RUN) { return !BT_TRAVERSE_RUN; };

    TRAVERSE_SUBTREE(inOrderTraverse, right);

    return BT_TRAVERSE_RUN;
}

void postOrderTraverse(BTNode* subRoot, bool (*function)(BTNode* node, va_list args), ...)
{
    CHECK_NULL(subRoot, return);

    va_list args = {};
    va_start(args, &function);

    postOrderTraverse(subRoot, function, args);

    va_end(args);
}

bool postOrderTraverse(BTNode* subRoot, bool (*function)(BTNode* node, va_list args), va_list args)
{
    CHECK_NULL(subRoot, return BT_TRAVERSE_RUN);

    TRAVERSE_SUBTREE(postOrderTraverse, left);
    TRAVERSE_SUBTREE(postOrderTraverse, right);

    if (function(subRoot, args) == !BT_TRAVERSE_RUN) { return !BT_TRAVERSE_RUN; };

    return BT_TRAVERSE_RUN;
}

BTNode* getRoot(BinaryTree* tree)
{
    assert(tree != nullptr);

    return tree->root;
}

void setRoot(BinaryTree* tree, BTNode* root)
{
    assert(tree != nullptr);

    tree->root = root;
}

BTElem_t getValue(BTNode* node)
{
    assert(node != nullptr);

    return node->value;
}

BTNode* getParent(BTNode* node)
{
    assert(node != nullptr);

    return node->parent;
}

BTNode* getLeft(BTNode* node)
{
    assert(node != nullptr);

    return node->left;
}

BTNode* getRight(BTNode* node)
{
    assert(node != nullptr);

    return node->right;
}

bool isLeft(BTNode* node)
{
    assert(node != nullptr);
    CHECK_NULL(node->parent, return false);

    return node->parent->left == node;
}

void setValue(BTNode* node, BTElem_t value)
{
    assert(node != nullptr);

    node->value = value;
}

void setParent(BTNode* node, BTNode* parent)
{
    assert(node != nullptr);

    node->parent = parent;
}

void setLeft(BTNode* node, BTNode* left)
{
    assert(node != nullptr);

    node->left = left;
}

void setRight(BTNode* node, BTNode* right)
{
    assert(node != nullptr);

    node->right = right;
}
