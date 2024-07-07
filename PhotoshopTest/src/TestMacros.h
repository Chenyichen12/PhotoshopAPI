#include "doctest.h"

#include <vector>


// Neat little implementation by DzedCPT https://stackoverflow.com/questions/41160846/test-floating-point-stdvector-with-c-catch
// That only adds one assertion for each vec rather than for each CHECK
#define CHECK_VEC_ALMOST_EQUAL(x, y) \
    REQUIRE(x.size() == y.size()); \
    for (size_t i = 0; i < x.size(); ++i) { \
		if (x[i] != doctest::Approx(y[i])) { \
			CHECK(x[i] == doctest::Approx(y[i])); \
		} \
    }


// Verbose CHECK implementation that will output exactly what position we failed in for vectors
#define CHECK_VEC_VERBOSE(x, y) \
    REQUIRE(x.size() == y.size()); \
    for (size_t i = 0; i < x.size(); ++i) { \
		if (x[i] != y[i]) { \
			CHECK_MESSAGE(x[i] == y[i], "Failed vector index: ", i); \
		} \
    }