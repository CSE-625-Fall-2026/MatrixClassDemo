#include "matrix/Matrix.hpp"

#include <gtest/gtest.h>
#include <stdexcept>

TEST(MatrixLu, ReturnsTriangularFactorsThatReconstructTheOriginal) {
    matrix::Matrix<double> value(3, 3);
    const double entries[3][3] = {{2, 3, 1}, {4, 7, 7}, {-2, 4, 5}};
    for (int row = 0; row < 3; ++row) {
        for (int col = 0; col < 3; ++col) {
            value.set(row, col, entries[row][col]);
        }
    }
    const auto original = value;
    auto [lower, upper] = value.lu();
    const auto product = lower * upper;
    for (int row = 0; row < 3; ++row) {
        EXPECT_DOUBLE_EQ(lower.get(row, row), 1.0);
        for (int col = 0; col < 3; ++col) {
            EXPECT_NEAR(product.get(row, col), value.get(row, col), 1e-12);
            if (col > row) {
                EXPECT_DOUBLE_EQ(lower.get(row, col), 0.0);
            }
            if (row > col) {
                EXPECT_DOUBLE_EQ(upper.get(row, col), 0.0);
            }
        }
    }
    EXPECT_DOUBLE_EQ(lower.get(1, 0), 2.0);
    EXPECT_DOUBLE_EQ(lower.get(2, 0), -1.0);
    EXPECT_DOUBLE_EQ(lower.get(2, 1), 7.0);
    lower.set(0, 0, 99.0);
    upper.set(0, 0, 99.0);
    EXPECT_EQ(value, original);
}

TEST(MatrixLu, SupportsFractionalMultipliers) {
    matrix::Matrix<double> value(2, 2);
    value.set(0, 0, 2); value.set(0, 1, 1);
    value.set(1, 0, 1); value.set(1, 1, 3);
    const auto [lower, upper] = value.lu();
    EXPECT_DOUBLE_EQ(lower.get(1, 0), 0.5);
    EXPECT_DOUBLE_EQ(upper.get(1, 1), 2.5);
    EXPECT_EQ(lower * upper, value);
}

TEST(MatrixLu, HandlesZeroColumnsAndDecomposableSingularMatrices) {
    matrix::Matrix<double> value(2, 2);
    value.set(0, 0, 1); value.set(0, 1, 2);
    value.set(1, 0, 2); value.set(1, 1, 4);
    const auto [lower, upper] = value.lu();
    EXPECT_EQ(lower * upper, value);
    EXPECT_DOUBLE_EQ(upper.get(1, 1), 0.0);

    matrix::Matrix<double> zeroColumn(2, 2);
    zeroColumn.set(0, 1, 1);
    zeroColumn.set(1, 1, 2);
    const auto [l, u] = zeroColumn.lu();
    EXPECT_EQ(l * u, zeroColumn);

    const matrix::Matrix<double> zero(1, 1);
    const auto [one, zeroUpper] = zero.lu();
    EXPECT_DOUBLE_EQ(one.get(0, 0), 1.0);
    EXPECT_EQ(zeroUpper, zero);
}

TEST(MatrixLu, RejectsRequiredRowSwapsAndInvalidDimensions) {
    matrix::Matrix<double> swapRequired(2, 2);
    swapRequired.set(0, 1, 1);
    swapRequired.set(1, 0, 1);
    EXPECT_THROW((void)swapRequired.lu(), std::domain_error);

    const matrix::Matrix<double> rectangle(2, 3), uninitialized;
    EXPECT_THROW((void)rectangle.lu(), std::invalid_argument);
    EXPECT_THROW((void)uninitialized.lu(), std::logic_error);
}

TEST(MatrixLu, IntegerDivisionMustBeExact) {
    matrix::Matrix<int> value(2, 2);
    value.set(0, 0, 2); value.set(0, 1, 1);
    value.set(1, 0, 4); value.set(1, 1, 3);
    const auto [lower, upper] = value.lu();
    EXPECT_EQ(lower * upper, value);
    value.set(1, 0, 1);
    EXPECT_THROW((void)value.lu(), std::domain_error);
    EXPECT_EQ(value.get(1, 0), 1);
}
