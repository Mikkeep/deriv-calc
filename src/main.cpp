#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#define UTB_DEFINITIONS
#include "utilib.h"
#include "../libs/log_generator.h"
#include "expression_tree.h"
#include "expression_loader.h"
#include "expression_simplifier.h"
#include "differentiation.h"

int main(int argc, char* argv[])
{
    LG_Init();

    ExprTree exprTree = {};
    construct(&exprTree);

    if (!loadExpression(&exprTree, argv[1])) 
    { 
        LG_LogMessage("loadExpression() returned false.", LG_STYLE_CLASS_ERROR);

        destroy(&exprTree);
        LG_Close();

        return -1; 
    }

    ExprTree derivTree = {};
    construct(&derivTree);

    derivTree.root = differentiate(exprTree.root);
    simplifyTree(&derivTree);

    latexDump(&derivTree);

    destroy(&exprTree);
    destroy(&derivTree);

    LG_Close();
}