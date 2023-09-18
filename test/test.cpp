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
      double correct = first_fuzz;
      Operation stringi = OP_ADD;
      std::string random_string = fuzzed_data.ConsumeRandomLengthString();
//      stringi = random_string[0];
      evaluateBinary(stringi, first_fuzz, second_fuzz);
//      isVariable(random_string[0]);
//      isConstant(correct);
}
