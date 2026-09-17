#include "matrix/Matrix.hpp"

#include <gtest/gtest.h>
#include <cmath>
#include <limits>
#include <stdexcept>

using Matrix = matrix::Matrix<double>;

TEST(MatrixPivotedLu, HandlesAZeroLeadingPivot) {
    Matrix a(2, 2);
    a.set(0, 1, 2);
    a.set(1, 0, 1); a.set(1, 1, 3);
    const auto [p, l, u] = a.luPartialPivoting();
    EXPECT_EQ(p * a, l * u);
    EXPECT_DOUBLE_EQ(p.get(0, 1), 1);
    EXPECT_DOUBLE_EQ(p.get(1, 0), 1);
    EXPECT_THROW((void)a.lu(), std::domain_error);
}

TEST(MatrixPivotedLu, ChoosesLargestMagnitudeAndSwapsPreviouslyComputedMultipliers) {
    Matrix a(3, 3);
    const double entries[3][3] = {{1, 3, 1}, {-4, 4, 2}, {2, 8, 5}};
    for (int row = 0; row < 3; ++row) {
        for (int col = 0; col < 3; ++col) {
            a.set(row, col, entries[row][col]);
        }
    }
    const auto original = a;
    auto [p, l, u] = a.luPartialPivoting();
    const auto permuted = p * a;
    const auto product = l * u;
    // Two swaps produce row order 1, 2, 0, so P differs from its transpose.
    EXPECT_DOUBLE_EQ(p.get(0, 1), 1);
    EXPECT_DOUBLE_EQ(p.get(1, 2), 1);
    EXPECT_DOUBLE_EQ(p.get(2, 0), 1);
    EXPECT_DOUBLE_EQ(u.get(0, 0), -4);
    EXPECT_DOUBLE_EQ(l.get(1, 0), -0.5);
    EXPECT_DOUBLE_EQ(l.get(2, 0), -0.25);
    for (int row = 0; row < 3; ++row) {
        EXPECT_DOUBLE_EQ(l.get(row, row), 1);
        double rowSum = 0, colSum = 0;
        for (int col = 0; col < 3; ++col) {
            EXPECT_NEAR(permuted.get(row, col), product.get(row, col), 1e-12);
            rowSum += p.get(row, col);
            colSum += p.get(col, row);
            if (col > row) EXPECT_DOUBLE_EQ(l.get(row, col), 0);
            if (row > col) {
                EXPECT_DOUBLE_EQ(u.get(row, col), 0);
                EXPECT_LE(std::abs(l.get(row, col)), 1.0);
            }
        }
        EXPECT_DOUBLE_EQ(rowSum, 1);
        EXPECT_DOUBLE_EQ(colSum, 1);
    }
    p.set(0, 0, 99); l.set(0, 0, 99); u.set(0, 0, 99);
    EXPECT_EQ(a, original);
}

TEST(MatrixPivotedLu, HandlesZeroColumnsAndSingularMatrices) {
    Matrix a(3, 3);
    a.set(0, 1, 1); a.set(0, 2, 2);
    a.set(1, 1, 2); a.set(1, 2, 4);
    const auto [p, l, u] = a.luPartialPivoting();
    EXPECT_EQ(p * a, l * u);
    EXPECT_DOUBLE_EQ(u.get(2, 2), 0);
    const Matrix zero(1, 1);
    const auto [zp, zl, zu] = zero.luPartialPivoting();
    EXPECT_EQ(zp * zero, zl * zu);
}

TEST(MatrixPivotedLu, PreservesIntegerDivisionChecksAndHandlesMinimumInteger) {
    matrix::Matrix<int> a(2, 2);
    a.set(0, 0, 1); a.set(1, 0, 2); a.set(1, 1, 1);
    EXPECT_THROW((void)a.luPartialPivoting(), std::domain_error);
    a.set(0, 0, std::numeric_limits<int>::lowest());
    a.set(1, 0, 0);
    const auto [p, l, u] = a.luPartialPivoting();
    EXPECT_EQ(p * a, l * u);
}

TEST(MatrixPivotedLu, RejectsNonSquareAndUninitializedMatrices) {
    const Matrix rectangle(2, 3), uninitialized;
    EXPECT_THROW((void)rectangle.luPartialPivoting(), std::invalid_argument);
    EXPECT_THROW((void)uninitialized.luPartialPivoting(), std::logic_error);
}
