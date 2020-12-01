#include <assert.h>
#include <stdio.h>
#include "expression_simplifier.h"
#include "utilib.h"

void simplifyNode      (ETNode* node, NodeType newType, ETNodeData data);
void simplifyNode      (ETNode* node, ETNode* child);
bool simplifyOps       (ETNode* root);
bool precalcConstExprs (ETNode* root);

void simplifyTree(ExprTree* tree)
{
    assert(tree       != nullptr);
    assert(tree->root != nullptr);

    while (precalcConstExprs(tree->root) || simplifyOps(tree->root));
}

void simplifyNode(ETNode* node, NodeType newType, ETNodeData data)
{
    assert(node != nullptr);

    setData(node, newType, data);

    if (node->left  != nullptr) { destroySubtree(node->left);  }
    if (node->right != nullptr) { destroySubtree(node->right); }

    node->left  = nullptr;
    node->right = nullptr;
}

void simplifyNode(ETNode* node, ETNode* child)
{
    assert(node  != nullptr);
    assert(child != nullptr);

    ETNode* other = isLeft(child) ? node->right : node->left;
    if (other != nullptr) { destroySubtree(other); }

    child->parent = node->parent;
    copyNode(node, child);

    deleteNode(child);
}

#define SIMPLIFY(otherSide) if (isIdentityType(simplifyType))                            \
                                simplifyNode(root, root->otherSide);                     \
                            else                                                         \
                                simplifyNode(root, TYPE_NUMBER, { simplifyType.result });

bool simplifyOps(ETNode* root)
{
    if (root == nullptr) { return false; }

    bool isChanged = simplifyOps(root->left) || simplifyOps(root->right);

    if (isTypeOp(root))
    {
        Operation         operation      = root->data.op;
        SimplifyExpr      simplifyType   = {}; 
        SimplifyArgTarget simplifyTarget = {};

        for (size_t i = 0; i < SIMPLIFY_EXPRS_COUNT; i++)
        {
            simplifyType   = SIMPLIFY_EXPRS[i];
            simplifyTarget = simplifyType.target;

            if (operation != simplifyType.operation) { continue; }

            assert(root->right != nullptr);

            if ((simplifyTarget == SAT_SND || simplifyTarget == SAT_ANY) && isTypeNumber(root->right) && 
                dcompare(root->right->data.number, simplifyType.arg) == 0)
            {
                SIMPLIFY(left)
                isChanged = true;
                break;
            }

            if ((simplifyTarget == SAT_FST || simplifyTarget == SAT_ANY) && isTypeNumber(root->left) && 
                dcompare(root->left->data.number, simplifyType.arg) == 0)
            {
                SIMPLIFY(right)
                isChanged = true;
                break;
            }
        }
    }

    return isChanged;
}

#undef SIMPLIFY

// just garbage value that has to not be equal to either +-1 or 0
const double GARBAGE_VALUE_FOR_PRECALC = 22022002;

//-----------------------------------------------------------------------------
//! Precalculates all expressions with constants (e.g. '2+19' -> '21').
//!
//! @param [in] root
//!
//! @return whether or not there have been any changes in the subtree.
//-----------------------------------------------------------------------------
bool precalcConstExprs(ETNode* root)
{
    if (root == nullptr) { return false; }

    ETNode* left   = root->left;
    ETNode* right  = root->right;

    ETNode* parent = root->parent;

    bool isChanged = precalcConstExprs(root->left) || precalcConstExprs(root->right);

    if (!isTypeOp(root)) { return isChanged; } 

    Operation operation = root->data.op;
    double    value     = GARBAGE_VALUE_FOR_PRECALC; 

    if (right->type == TYPE_NUMBER)
    {
        if ((operation == OP_ADD || operation == OP_SUB || operation == OP_MUL || operation == OP_DIV) && 
            isTypeNumber(left) && isTypeNumber(right) &&
            !isConstant(left->data.number) && !isConstant(right->data.number))
        {
            simplifyNode(root, TYPE_NUMBER, { evaluateBinary(operation, left->data.number, right->data.number) });
            isChanged = true;
        }
        else
        {
            if (isOperationUnary(operation) && isTypeNumber(root->right))
            {
                value = evaluateUnary(operation, root->right->data.number);
            }
            else if (!isOperationUnary(operation) && isTypeNumber(root->left) && isTypeNumber(root->right))
            {
                value = evaluateBinary(operation, root->left->data.number, root->right->data.number);
            }
        }
    }
    else if (isTrigOp(operation) && right->type == TYPE_OP && right->right->type == TYPE_NUMBER)
    {
        if (isOperationUnary(right->data.op)) 
        { 
            value = evaluateUnary(right->data.op, right->right->data.number); 
        }
        else if (right->left->type == TYPE_NUMBER)
        {
            value = evaluateBinary(right->data.op, right->left->data.number, right->right->data.number); 
        }

        value = evaluateUnary(operation, value);
    }

    if (dcompare(value,  GARBAGE_VALUE_FOR_PRECALC) != 0)
    {
        if (dcompare(value,  0.0) == 0) { simplifyNode(root, TYPE_NUMBER, { 0.0}); return true; }
        if (dcompare(value,  1.0) == 0) { simplifyNode(root, TYPE_NUMBER, { 1.0}); return true; }
        if (dcompare(value, -1.0) == 0) { simplifyNode(root, TYPE_NUMBER, {-1.0}); return true; }
    }

    return isChanged;
}