#include "matrix/Matrix.hpp"

#include <gtest/gtest.h>

#include <stdexcept>

namespace {

matrix::Matrix makeTwoByTwo(double a, double b, double c, double d) {
    matrix::Matrix result(2, 2);
    result.set(0, 0, a);
    result.set(0, 1, b);
    result.set(1, 0, c);
    result.set(1, 1, d);
    return result;
}

TEST(MatrixConstruction, DimensionsConstructorInitializesToZero) {
    const matrix::Matrix matrix(2, 3);

    EXPECT_TRUE(matrix.isInitialized());
    EXPECT_EQ(matrix.getNumRows(), 2U);
    EXPECT_EQ(matrix.getNumCols(), 3U);
    EXPECT_DOUBLE_EQ(matrix.get(1, 2), 0.0);
}

TEST(MatrixConstruction, DefaultConstructorSupportsDeferredInitialization) {
    matrix::Matrix matrix;

    EXPECT_FALSE(matrix.isInitialized());
    matrix.setNumRows(2);
    matrix.setNumCols(3);
    matrix.initialize();

    EXPECT_TRUE(matrix.isInitialized());
    EXPECT_EQ(matrix.getNumRows(), 2U);
    EXPECT_EQ(matrix.getNumCols(), 3U);
}

TEST(MatrixConstruction, InitializationRequiresBothPositiveDimensions) {
    matrix::Matrix matrix;
    matrix.setNumRows(2);

    EXPECT_THROW(matrix.initialize(), std::logic_error);
    EXPECT_THROW(matrix.setNumCols(0), std::invalid_argument);
    EXPECT_THROW(matrix::Matrix(0, 2), std::invalid_argument);
}

TEST(MatrixConstruction, DimensionsCannotChangeAfterInitialization) {
    matrix::Matrix matrix(2, 3);

    EXPECT_THROW(matrix.setNumRows(4), std::logic_error);
    EXPECT_THROW(matrix.setNumCols(4), std::logic_error);
    EXPECT_THROW(matrix.initialize(), std::logic_error);
}

TEST(MatrixEntries, ValuesCanBeSetAndRead) {
    matrix::Matrix matrix(2, 2);

    matrix.set(1, 0, 7.5);
    matrix.at(0, 1) = -2.0;

    EXPECT_DOUBLE_EQ(matrix.get(1, 0), 7.5);
    EXPECT_DOUBLE_EQ(matrix.at(0, 1), -2.0);
}

TEST(MatrixEntries, InvalidAccessThrows) {
    matrix::Matrix matrix(2, 2);

    EXPECT_THROW((void)matrix.get(2, 0), std::out_of_range);
    EXPECT_THROW(matrix.set(0, 2, 1.0), std::out_of_range);

    matrix::Matrix uninitialized;
    EXPECT_THROW((void)uninitialized.get(0, 0), std::logic_error);
}

TEST(MatrixArithmetic, AddsAndSubtractsMatrices) {
    const auto left = makeTwoByTwo(1.0, 2.0, 3.0, 4.0);
    const auto right = makeTwoByTwo(5.0, 6.0, 7.0, 8.0);

    EXPECT_EQ(left + right, makeTwoByTwo(6.0, 8.0, 10.0, 12.0));
    EXPECT_EQ(right - left, makeTwoByTwo(4.0, 4.0, 4.0, 4.0));
}

TEST(MatrixArithmetic, MultipliesRectangularMatrices) {
    matrix::Matrix left(2, 3);
    left.set(0, 0, 1.0);
    left.set(0, 1, 2.0);
    left.set(0, 2, 3.0);
    left.set(1, 0, 4.0);
    left.set(1, 1, 5.0);
    left.set(1, 2, 6.0);

    matrix::Matrix right(3, 2);
    right.set(0, 0, 7.0);
    right.set(0, 1, 8.0);
    right.set(1, 0, 9.0);
    right.set(1, 1, 10.0);
    right.set(2, 0, 11.0);
    right.set(2, 1, 12.0);

    const matrix::Matrix result = left * right;

    EXPECT_EQ(result.getNumRows(), 2U);
    EXPECT_EQ(result.getNumCols(), 2U);
    EXPECT_DOUBLE_EQ(result.get(0, 0), 58.0);
    EXPECT_DOUBLE_EQ(result.get(0, 1), 64.0);
    EXPECT_DOUBLE_EQ(result.get(1, 0), 139.0);
    EXPECT_DOUBLE_EQ(result.get(1, 1), 154.0);
}

TEST(MatrixArithmetic, CompoundAssignmentsUpdateTheLeftOperand) {
    matrix::Matrix value = makeTwoByTwo(1.0, 2.0, 3.0, 4.0);
    const matrix::Matrix increment = makeTwoByTwo(2.0, 2.0, 2.0, 2.0);

    value += increment;
    EXPECT_EQ(value, makeTwoByTwo(3.0, 4.0, 5.0, 6.0));

    value -= increment;
    EXPECT_EQ(value, makeTwoByTwo(1.0, 2.0, 3.0, 4.0));

    value *= makeTwoByTwo(1.0, 0.0, 0.0, 1.0);
    EXPECT_EQ(value, makeTwoByTwo(1.0, 2.0, 3.0, 4.0));
}

TEST(MatrixArithmetic, CopyAssignmentCopiesValues) {
    const matrix::Matrix original = makeTwoByTwo(1.0, 2.0, 3.0, 4.0);
    matrix::Matrix copy;

    copy = original;

    EXPECT_EQ(copy, original);
}

TEST(MatrixArithmetic, CopiesHaveIndependentRows) {
    const matrix::Matrix original = makeTwoByTwo(1.0, 2.0, 3.0, 4.0);
    matrix::Matrix copy(original);

    copy.set(0, 0, 10.0);

    EXPECT_DOUBLE_EQ(original.get(0, 0), 1.0);
    EXPECT_DOUBLE_EQ(copy.get(0, 0), 10.0);
}

TEST(MatrixArithmetic, IncompatibleDimensionsThrow) {
    const matrix::Matrix twoByTwo(2, 2);
    const matrix::Matrix twoByThree(2, 3);
    const matrix::Matrix fourByOne(4, 1);

    EXPECT_THROW(twoByTwo + twoByThree, std::invalid_argument);
    EXPECT_THROW(twoByTwo - twoByThree, std::invalid_argument);
    EXPECT_THROW(twoByThree * fourByOne, std::invalid_argument);
}

}
