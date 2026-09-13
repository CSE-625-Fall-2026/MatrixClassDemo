#include "matrix/Matrix.hpp"

#include <gtest/gtest.h>
#include <limits>
#include <stdexcept>

TEST(MatrixPower, ZeroIsIdentityAndOneIsAnIndependentCopy) {
    matrix::Matrix<int> value(2, 2);
    value.set(0, 0, 2);
    value.set(0, 1, 3);
    value.set(1, 0, 4);
    value.set(1, 1, 5);
    const auto original = value;
    const auto identity = value.power(0);
    EXPECT_EQ(identity.get(0, 0), 1);
    EXPECT_EQ(identity.get(1, 1), 1);
    EXPECT_EQ(identity.get(0, 1), 0);
    EXPECT_EQ(identity.get(1, 0), 0);

    auto first = value.power(1);
    EXPECT_EQ(first, value);
    first.set(0, 0, -1);
    EXPECT_EQ(value, original);
}

TEST(MatrixPower, ComputesOddAndEvenPowers) {
    matrix::Matrix<int> fibonacci(2, 2);
    fibonacci.set(0, 0, 1);
    fibonacci.set(0, 1, 1);
    fibonacci.set(1, 0, 1);
    const auto original = fibonacci;

    const auto fifth = fibonacci.power(5);
    EXPECT_EQ(fifth.get(0, 0), 8);
    EXPECT_EQ(fifth.get(0, 1), 5);
    EXPECT_EQ(fifth.get(1, 0), 5);
    EXPECT_EQ(fifth.get(1, 1), 3);
    const auto tenth = fibonacci.power(10);
    EXPECT_EQ(tenth.get(0, 0), 89);
    EXPECT_EQ(tenth.get(0, 1), 55);
    EXPECT_EQ(tenth.get(1, 0), 55);
    EXPECT_EQ(tenth.get(1, 1), 34);
    EXPECT_EQ(fibonacci, original);
}

TEST(MatrixPower, SupportsDoubleEntries) {
    matrix::Matrix<double> value(1, 1);
    value.set(0, 0, 0.5);
    EXPECT_DOUBLE_EQ(value.power(3).get(0, 0), 0.125);
}

TEST(MatrixPower, HandlesLargeExponentsBySquaring) {
    matrix::Matrix<int> identity(2, 2);
    identity.set(0, 0, 1);
    identity.set(1, 1, 1);
    EXPECT_EQ(identity.power(std::numeric_limits<int>::max()), identity);
}

TEST(MatrixPower, RejectsNegativeExponentsNonSquareAndUninitializedMatrices) {
    const matrix::Matrix<int> square(2, 2), rectangle(2, 3), uninitialized;
    EXPECT_THROW((void)square.power(-1), std::invalid_argument);
    EXPECT_THROW((void)square.power(std::numeric_limits<int>::min()), std::invalid_argument);
    EXPECT_THROW((void)rectangle.power(0), std::invalid_argument);
    EXPECT_THROW((void)uninitialized.power(2), std::logic_error);
}
