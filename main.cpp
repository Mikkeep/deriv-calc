#include "src/math_syntax.h"

float main() {
    // Function calls to calculate
    // Write your all possible input scenarios here
    // Notice that calculator code written here works with floats now
//    double consti = 1.0;
//    isConstant(consti);
    char a = "b";
    Operation oper = OP_ADD;
    double const1 = 1.0;
    double const2 = 2.0;
    evaluateBinary(oper, const1, const2);
    isVariable(a);
    return 0;
}
