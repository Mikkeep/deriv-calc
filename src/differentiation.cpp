#include <assert.h>
#include "differentiation.h"

#define LEFT  root->left
#define RIGHT root->right

#define UNARY_OP(operation, arg) *newNode(TYPE_OP, { .op = OP_##operation }, NULL,             (ETNode*) &(arg))
#define BINARY_OP(operation)     *newNode(TYPE_OP, { .op = OP_##operation }, (ETNode*) &tree1, (ETNode*) &tree2)

// #define ADD(left, right) BINARY_OP(ADD)
// #define SUB(left, right) BINARY_OP(SUB)
// #define MUL(left, right) BINARY_OP(MUL)
// #define DIV(left, right) BINARY_OP(DIV)
// #define POW(left, right) BINARY_OP(POW)

#define LOG(arg) UNARY_OP(LOG, arg)
#define EXP(arg) UNARY_OP(EXP, arg)

#define SIN(arg) UNARY_OP(SIN, arg)
#define COS(arg) UNARY_OP(COS, arg)
#define TAN(arg) UNARY_OP(TAN, arg)

#define NUM(num) (*newNode(TYPE_NUMBER, { num }, nullptr, nullptr))

#define dL (*differentiate(LEFT))
#define dR (*differentiate(RIGHT))

#define L (*copyTree(LEFT))
#define R (*copyTree(RIGHT))

#define RETURN(arg) return &(arg)

ETNode& operator + (const ETNode& tree1, const ETNode& tree2)
{
    return BINARY_OP(ADD);
}

ETNode& operator - (const ETNode& tree1, const ETNode& tree2)
{
    return BINARY_OP(SUB);
}

ETNode& operator * (const ETNode& tree1, const ETNode& tree2)
{
    return BINARY_OP(MUL);
}

ETNode& operator / (const ETNode& tree1, const ETNode& tree2)
{
    return BINARY_OP(DIV);
}

ETNode& operator ^ (const ETNode& tree1, const ETNode& tree2)
{
    return BINARY_OP(POW);
}

ETNode* differentiate(ETNode* root)
{
    assert(root != nullptr);

    Operation operation = isTypeOp(root) ? root->data.op : OP_INVALID;

    // TODO: make at least a bit more eye-pleasing
    if (root->type == TYPE_NUMBER || (isTypeOp(root) && !hasVariable(root, 'x')))
    {
        return newNode(TYPE_NUMBER, { 0.0 }, nullptr, nullptr);
    }

    if (root->type == TYPE_VAR)
    {
        return newNode(TYPE_NUMBER, { (double) (root->data.var == 'x') }, nullptr, nullptr);
    }

    assert(isTypeOp(root));

    switch (operation)
    {
        case OP_ADD: RETURN(dL + dR);
        case OP_SUB: RETURN(dL - dR);

        case OP_MUL: RETURN((dL * R) + (L * dR));
        case OP_DIV: RETURN((dL * R - L * dR) / (R ^ NUM(2)));

        case OP_POW: if (!hasVariable(root->right, 'x')) { RETURN(R * (L ^ (R - NUM(1))) * dL);            }
                     else                                { RETURN((L ^ R) * (dR * LOG(L) + (R / L) * dL)); } 

        case OP_LOG: RETURN((NUM(1) / R) * dR);
        case OP_EXP: RETURN(EXP(R) * dR);    

        case OP_SIN: RETURN(COS(R) * dR);
        case OP_COS: RETURN(NUM(-1) * SIN(R) * dR);
        case OP_TAN: RETURN((NUM(1) / (COS(R) ^ NUM(2))) * dR);

        default:     return nullptr;
    }

    return nullptr;
}