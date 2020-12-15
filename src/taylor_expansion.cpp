#include "assert.h"
#include "stdlib.h"
#include "taylor_expansion.h"
#include "expression_simplifier.h"

ETNode* taylorExpansion(ETNode* exprRoot, int atPoint, size_t maxPower)
{
    assert(exprRoot != nullptr);

    ETNode* expansion    = copyTree(exprRoot);
    substitute(expansion, 'x', atPoint);

    ETNode* derivative   = copyTree(exprRoot);
    ETNode* derivAtPoint = nullptr;

    size_t factorial = 1;

    for (size_t i = 1; i <= maxPower; i++)
    {
        factorial *= i;

        derivAtPoint = differentiate(derivative);
        simplifyTree(derivAtPoint);

        destroySubtree(derivative);
        derivative = copyTree(derivAtPoint);

        substitute(derivAtPoint, 'x', atPoint);
        simplifyTree(derivAtPoint);

        expansion = &(*expansion + (*derivAtPoint) * ((VAR('x') - NUM(atPoint)) ^ NUM(i)) / NUM(factorial));
    }

    return expansion;
}