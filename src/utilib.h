#pragma once

#include <math.h>

static const double DOUBLE_PRECISION = 1e-6;

int compare(double num1, double num2);

#ifndef UTB_DECLARATIONS_ONLY

int compare(double num1, double num2)
{
    if (fabs(num1 - num2) < DOUBLE_PRECISION) { return 0; }

    if (num1 > num2) { return 1; }

    return -1;
}

#undef UTB_DECLARATIONS_ONLY
#endif