#include "matrix/Matrix.hpp"

#include <gtest/gtest.h>
#include <stdexcept>

TEST(MatrixAugment, JoinsAllColumnsAndLeavesInputsUnchanged) {
    matrix::Matrix<int> left(2, 2);
    matrix::Matrix<int> right(2, 3);
    for (int row = 0; row < 2; ++row) {
        for (int col = 0; col < 2; ++col) {
            left.set(row, col, row * 10 + col);
        }
        for (int col = 0; col < 3; ++col) {
            right.set(row, col, 100 + row * 10 + col);
        }
    }
    const auto originalLeft = left;
    const auto originalRight = right;
    auto result = left.augment(right);

    ASSERT_EQ(result.getNumRows(), 2U);
    ASSERT_EQ(result.getNumCols(), 5U);
    for (int row = 0; row < 2; ++row) {
        for (int col = 0; col < 2; ++col) {
            EXPECT_EQ(result.get(row, col), left.get(row, col));
        }
        for (int col = 0; col < 3; ++col) {
            EXPECT_EQ(result.get(row, 2 + col), right.get(row, col));
        }
    }
    result.set(0, 0, -1);
    result.set(0, 2, -2);
    EXPECT_EQ(left, originalLeft);
    EXPECT_EQ(right, originalRight);
}

TEST(MatrixAugment, SupportsSelfAugmentationAndDoubleEntries) {
    matrix::Matrix<double> value(1, 1);
    value.set(0, 0, 0.25);
    const auto result = value.augment(value);
    ASSERT_EQ(result.getNumCols(), 2U);
    EXPECT_DOUBLE_EQ(result.get(0, 0), 0.25);
    EXPECT_DOUBLE_EQ(result.get(0, 1), 0.25);
}

TEST(MatrixAugment, RejectsDifferentRowCountsAndUninitializedInputs) {
    const matrix::Matrix<int> left(2, 1), right(3, 1), uninitialized;
    EXPECT_THROW((void)left.augment(right), std::invalid_argument);
    EXPECT_THROW((void)left.augment(uninitialized), std::logic_error);
    EXPECT_THROW((void)uninitialized.augment(left), std::logic_error);
}
