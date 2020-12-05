#include <assert.h>
#include "math_syntax.h"
#include "differentiation.h"

#define LEFT  root->left
#define RIGHT root->right

#define dL (*differentiate(LEFT))
#define dR (*differentiate(RIGHT))
#define L  (*copyTree(LEFT))
#define R  (*copyTree(RIGHT))

#define RETURN(arg) return &(arg)

ETNode* differentiate(ETNode* root)
{
    assert(root != nullptr);

    Operation operation = isTypeOp(root) ? root->data.op : OP_INVALID;

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