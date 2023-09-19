#include "src/math_syntax.h"

float main() {
    char a = "b";
    isVariable(a);
    Operation oper = OP_ADD;
    double const1 = 1.0;
    double const2 = 2.0;
    evaluateBinary(oper, const1, const2);
    return 0;
}
