#include <assert.h>
#include "src/math_syntax.h"
#include <cifuzz/cifuzz.h>
#include <fuzzer/FuzzedDataProvider.h>

FUZZ_TEST_SETUP() {
  // Perform any one-time setup required by the FUZZ_TEST function.
}

FUZZ_TEST(const uint8_t *data, size_t size) {
	FuzzedDataProvider fuzzed_data(data, size);
	float first_fuzz = fuzzed_data.ConsumeFloatingPoint<float>();
	float second_fuzz = fuzzed_data.ConsumeFloatingPoint<float>();
	std::string random_string = fuzzed_data.ConsumeRandomLengthString();

	calculator(first_fuzz, random_string[0], second_fuzz);
}
