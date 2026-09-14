#include "matrix/Matrix.hpp"

#include <gtest/gtest.h>
#include <limits>
#include <stdexcept>

TEST(MatrixRref, ReducesAnAugmentedSystemWithoutChangingInput) {
    matrix::Matrix<double> coefficients(3, 3), answers(3, 1);
    const double entries[3][3] = {{2, 1, -1}, {-3, -1, 2}, {-2, 1, 2}};
    for (int row = 0; row < 3; ++row) {
        for (int col = 0; col < 3; ++col) {
            coefficients.set(row, col, entries[row][col]);
        }
    }
    answers.set(0, 0, 8);
    answers.set(1, 0, -11);
    answers.set(2, 0, -3);
    const auto augmented = coefficients.augment(answers);
    const auto original = augmented;
    const auto reduced = augmented.rref();

    ASSERT_EQ(reduced.getNumRows(), 3U);
    ASSERT_EQ(reduced.getNumCols(), 4U);
    for (int row = 0; row < 3; ++row) {
        for (int col = 0; col < 3; ++col) {
            EXPECT_NEAR(reduced.get(row, col), row == col ? 1.0 : 0.0, 1e-12);
        }
    }
    EXPECT_NEAR(reduced.get(0, 3), 2.0, 1e-12);
    EXPECT_NEAR(reduced.get(1, 3), 3.0, 1e-12);
    EXPECT_NEAR(reduced.get(2, 3), -1.0, 1e-12);
    EXPECT_EQ(augmented, original);
}

TEST(MatrixRref, SwapsRowsSkipsZeroColumnsAndKeepsZeroRowsAtBottom) {
    matrix::Matrix<double> value(3, 4);
    value.set(1, 1, 2);
    value.set(1, 2, 4);
    value.set(1, 3, 6);
    value.set(2, 1, 1);
    value.set(2, 2, 2);
    value.set(2, 3, 3);
    matrix::Matrix<double> expected(3, 4);
    expected.set(0, 1, 1);
    expected.set(0, 2, 2);
    expected.set(0, 3, 3);
    EXPECT_EQ(value.rref(), expected);
    EXPECT_EQ(expected.rref(), expected);
}

TEST(MatrixRref, HandlesZeroAndTallMatrices) {
    const matrix::Matrix<double> zero(2, 3);
    EXPECT_EQ(zero.rref(), zero);
    matrix::Matrix<double> tall(3, 1);
    tall.set(1, 0, 2.0);
    tall.set(2, 0, 4.0);
    const auto reduced = tall.rref();
    EXPECT_DOUBLE_EQ(reduced.get(0, 0), 1.0);
    EXPECT_DOUBLE_EQ(reduced.get(1, 0), 0.0);
    EXPECT_DOUBLE_EQ(reduced.get(2, 0), 0.0);
}

TEST(MatrixRref, IntegerDivisionMustBeExact) {
    matrix::Matrix<int> exact(2, 2);
    exact.set(0, 0, 1); exact.set(0, 1, 2);
    exact.set(1, 0, 3); exact.set(1, 1, 7);
    const auto reduced = exact.rref();
    EXPECT_EQ(reduced.get(0, 0), 1);
    EXPECT_EQ(reduced.get(0, 1), 0);
    EXPECT_EQ(reduced.get(1, 0), 0);
    EXPECT_EQ(reduced.get(1, 1), 1);

    matrix::Matrix<int> fractions(1, 2);
    fractions.set(0, 0, 2); fractions.set(0, 1, 1);
    EXPECT_THROW((void)fractions.rref(), std::domain_error);
    EXPECT_EQ(fractions.get(0, 1), 1);

    matrix::Matrix<int> overflow(1, 2);
    overflow.set(0, 0, -1);
    overflow.set(0, 1, std::numeric_limits<int>::lowest());
    EXPECT_THROW((void)overflow.rref(), std::overflow_error);
}

TEST(MatrixRref, RejectsUninitializedMatrices) {
    const matrix::Matrix<double> value;
    EXPECT_THROW((void)value.rref(), std::logic_error);
}
