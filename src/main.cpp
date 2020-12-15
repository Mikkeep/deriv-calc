//-----------------------------------------------------------------------------
//! THANKS TO
//!
//! Mikhail Shishatsky
//!     - for being the best mentor of all time
//-----------------------------------------------------------------------------

#include <assert.h>
#include <ctype.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define UTB_DEFINITIONS
#include "utilib.h"
#include "../libs/log_generator.h"
#include "expression_tree.h"
#include "expression_loader.h"
#include "expression_simplifier.h"
#include "differentiation.h"
#include "taylor_expansion.h"
#include "funnyentific_paper.h"

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

    makeScientificPaper(exprTree.root);

    // latexDump(&exprTree);

    // ExprTree expansion = {};
    // // construct(&expansion);
    // // expansion.root = taylorExpansion(exprTree.root, 0, 5);
    // // simplifyTree(&expansion);

    // // graphDump(&expansion);
    // // latexDump(&expansion);

    // ExprTree derivTree = {};
    // construct(&derivTree);
    // derivTree.root = differentiate(exprTree.root);
    // simplifyTree(&derivTree);

    // graphDump(&derivTree);
    // latexDump(&derivTree);

    // destroy(&expansion);
    // destroy(&derivTree);
    destroy(&exprTree);

    LG_Close();

    return 0;
}