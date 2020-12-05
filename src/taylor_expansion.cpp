#include "assert.h"
#include "stdlib.h"
#include "taylor_expansion.h"

ETNode* factorial(size_t value)
{
    ETNode* fact  = &NUM(1);

    for (size_t i = 2; i <= value; i++)
    {
        fact = &(*fact * NUM(i));
    }

    return fact;
}

ETNode* taylorExpansion(ETNode* exprRoot, int atPoint, size_t maxPower)
{
    assert(exprRoot != nullptr);

    ETNode* expansion    = copyTree(exprRoot);
    ETNode* derivative   = copyTree(exprRoot);
    ETNode* derivAtPoint = nullptr;

    for (size_t i = 1; i <= maxPower; i++)
    {
        derivAtPoint = differentiate(derivative);

        destroySubtree(derivative);
        derivative = copyTree(derivAtPoint);

        expansion = &(*expansion + (*derivAtPoint) / *factorial(i) * ((VAR('x') - NUM(atPoint)) ^ NUM(i)));
    }

    return expansion;
}