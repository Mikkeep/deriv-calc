#pragma once
#include "differentiation.h"

ETNode* factorial       (size_t value);
ETNode* taylorExpansion (ETNode* exprRoot, int atPoint, size_t maxPower);