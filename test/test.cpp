#include <assert.h>
#include "../src/math_syntax.h"
#include <cifuzz/cifuzz.h>
#include <fuzzer/FuzzedDataProvider.h>

FUZZ_TEST_SETUP() {
  // Perform any one-time setup required by the FUZZ_TEST function.
}

FUZZ_TEST(const uint8_t *data, size_t size) {
      FuzzedDataProvider fuzzed_data(data, size);
      int first_fuzz = fuzzed_data.ConsumeIntegral<int>();
      int second_fuzz = fuzzed_data.ConsumeIntegral<int>();
      Operation stringi = OP_ADD;
      std::string random_string = fuzzed_data.ConsumeRandomLengthString();
      evaluateBinary(stringi, first_fuzz, second_fuzz);
      Operation stringi2 = OP_TAN;
      isArithmeticOp(stringi2);
      isTrigOp(stringi);
      isVariable(random_string[0]);
}
