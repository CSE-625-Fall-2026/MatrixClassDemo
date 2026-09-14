#include "matrix/Matrix.hpp"
#include "ArbitraryInteger.hpp"
#include "RationalNumber.hpp"

#include <gtest/gtest.h>
#include <stdexcept>

TEST(MatrixNumberTypes, AugmentsAndPowersArbitraryIntegers) {
    matrix::Matrix<ArbitraryInteger> a(1, 1), b(1, 1);
    a.set(0, 0, ArbitraryInteger("100000000000000000000"));
    b.set(0, 0, 3);
    const auto combined = a.augment(b);
    EXPECT_EQ(combined.get(0, 0), a.get(0, 0));
    EXPECT_EQ(combined.get(0, 1), ArbitraryInteger(3));
    EXPECT_EQ(a.power(2).get(0, 0).toString(), "10000000000000000000000000000000000000000");
}

TEST(MatrixNumberTypes, ReducesRationalMatricesExactly) {
    using R = RationalNumber<int>;
    matrix::Matrix<R> value(2, 3);
    value.set(0, 0, R(2)); value.set(0, 2, R(1));
    value.set(1, 1, R(3)); value.set(1, 2, R(1));
    const auto reduced = value.rref();
    EXPECT_EQ(reduced.get(0, 0), R(1));
    EXPECT_EQ(reduced.get(1, 1), R(1));
    EXPECT_EQ(reduced.get(0, 2), R(1, 2));
    EXPECT_EQ(reduced.get(1, 2), R(1, 3));
    EXPECT_TRUE(reduced.rref() == reduced);
}

TEST(MatrixNumberTypes, SupportsRationalsWithArbitraryIntegerStorage) {
    using R = RationalNumber<ArbitraryInteger>;
    matrix::Matrix<R> value(1, 2);
    const ArbitraryInteger large("100000000000000000000");
    value.set(0, 0, R(large));
    value.set(0, 1, R(1));
    const auto reduced = value.rref();
    EXPECT_EQ(reduced.get(0, 0), R(1));
    EXPECT_EQ(reduced.get(0, 1), R(ArbitraryInteger(1), large));
}

TEST(MatrixNumberTypes, RejectsTruncatingArbitraryIntegerDivision) {
    matrix::Matrix<ArbitraryInteger> value(1, 2);
    value.set(0, 0, 2);
    value.set(0, 1, 1);
    EXPECT_THROW((void)value.rref(), std::domain_error);
    value.set(0, 1, 4);
    EXPECT_EQ(value.rref().get(0, 1), ArbitraryInteger(2));
}
