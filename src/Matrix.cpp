#include "matrix/Matrix.hpp"

#include <limits>
#include <stdexcept>
#include <utility>

namespace matrix {

Matrix::Matrix(size_type rows, size_type cols) {
    setNumRows(rows);
    setNumCols(cols);
    initialize();
}

Matrix::Matrix(const Matrix& other)
    : rows_(other.rows_),
      cols_(other.cols_),
      initialized_(other.initialized_) {
    if (!initialized_) {
        return;
    }

    entries_ = allocateRows(rows_, cols_);
    for (size_type row = 0; row < rows_; ++row) {
        for (size_type col = 0; col < cols_; ++col) {
            entries_[row][col] = other.entries_[row][col];
        }
    }
}

Matrix::Matrix(Matrix&& other) noexcept
    : rows_(other.rows_),
      cols_(other.cols_),
      initialized_(other.initialized_),
      entries_(other.entries_) {
    other.rows_ = 0;
    other.cols_ = 0;
    other.initialized_ = false;
    other.entries_ = nullptr;
}

Matrix& Matrix::operator=(const Matrix& other) {
    if (this != &other) {
        Matrix copy(other);
        swap(copy);
    }
    return *this;
}

Matrix& Matrix::operator=(Matrix&& other) noexcept {
    if (this != &other) {
        deleteRows(entries_, rows_);

        rows_ = other.rows_;
        cols_ = other.cols_;
        initialized_ = other.initialized_;
        entries_ = other.entries_;

        other.rows_ = 0;
        other.cols_ = 0;
        other.initialized_ = false;
        other.entries_ = nullptr;
    }
    return *this;
}

Matrix::~Matrix() {
    deleteRows(entries_, rows_);
}

void Matrix::setNumRows(size_type rows) {
    if (initialized_) {
        throw std::logic_error("cannot change rows after initialization");
    }
    if (rows == 0) {
        throw std::invalid_argument("number of rows must be greater than zero");
    }
    rows_ = rows;
}

void Matrix::setNumCols(size_type cols) {
    if (initialized_) {
        throw std::logic_error("cannot change columns after initialization");
    }
    if (cols == 0) {
        throw std::invalid_argument("number of columns must be greater than zero");
    }
    cols_ = cols;
}

void Matrix::initialize() {
    if (initialized_) {
        throw std::logic_error("matrix is already initialized");
    }
    if (rows_ == 0 || cols_ == 0) {
        throw std::logic_error("set both matrix dimensions before initialization");
    }
    if (rows_ > std::numeric_limits<size_type>::max() / cols_) {
        throw std::length_error("matrix dimensions are too large");
    }

    entries_ = allocateRows(rows_, cols_);
    initialized_ = true;
}

Matrix::size_type Matrix::getNumRows() const noexcept {
    return rows_;
}

Matrix::size_type Matrix::getNumCols() const noexcept {
    return cols_;
}

bool Matrix::isInitialized() const noexcept {
    return initialized_;
}

Matrix::value_type Matrix::get(size_type row, size_type col) const {
    return at(row, col);
}

void Matrix::set(size_type row, size_type col, value_type value) {
    at(row, col) = value;
}

Matrix::value_type& Matrix::at(size_type row, size_type col) {
    requireInitialized();
    if (row >= rows_ || col >= cols_) {
        throw std::out_of_range("matrix index is outside its dimensions");
    }
    return entries_[row][col];
}

const Matrix::value_type& Matrix::at(size_type row, size_type col) const {
    requireInitialized();
    if (row >= rows_ || col >= cols_) {
        throw std::out_of_range("matrix index is outside its dimensions");
    }
    return entries_[row][col];
}

Matrix Matrix::operator+(const Matrix& rhs) const {
    Matrix result(*this);
    result += rhs;
    return result;
}

Matrix Matrix::operator-(const Matrix& rhs) const {
    Matrix result(*this);
    result -= rhs;
    return result;
}

Matrix Matrix::operator*(const Matrix& rhs) const {
    requireInitialized();
    rhs.requireInitialized();
    if (cols_ != rhs.rows_) {
        throw std::invalid_argument(
            "left matrix columns must equal right matrix rows for multiplication"
        );
    }

    Matrix result(rows_, rhs.cols_);
    for (size_type row = 0; row < rows_; ++row) {
        for (size_type col = 0; col < rhs.cols_; ++col) {
            value_type sum = 0.0;
            for (size_type inner = 0; inner < cols_; ++inner) {
                sum += at(row, inner) * rhs.at(inner, col);
            }
            result.set(row, col, sum);
        }
    }
    return result;
}

Matrix& Matrix::operator+=(const Matrix& rhs) {
    requireSameDimensions(rhs);
    for (size_type row = 0; row < rows_; ++row) {
        for (size_type col = 0; col < cols_; ++col) {
            entries_[row][col] += rhs.entries_[row][col];
        }
    }
    return *this;
}

Matrix& Matrix::operator-=(const Matrix& rhs) {
    requireSameDimensions(rhs);
    for (size_type row = 0; row < rows_; ++row) {
        for (size_type col = 0; col < cols_; ++col) {
            entries_[row][col] -= rhs.entries_[row][col];
        }
    }
    return *this;
}

Matrix& Matrix::operator*=(const Matrix& rhs) {
    *this = *this * rhs;
    return *this;
}

bool Matrix::operator==(const Matrix& rhs) const noexcept {
    if (rows_ != rhs.rows_ || cols_ != rhs.cols_ ||
        initialized_ != rhs.initialized_) {
        return false;
    }
    if (!initialized_) {
        return true;
    }
    for (size_type row = 0; row < rows_; ++row) {
        for (size_type col = 0; col < cols_; ++col) {
            if (entries_[row][col] != rhs.entries_[row][col]) {
                return false;
            }
        }
    }
    return true;
}

bool Matrix::operator!=(const Matrix& rhs) const noexcept {
    return !(*this == rhs);
}

Matrix::value_type** Matrix::allocateRows(size_type rows, size_type cols) {
    value_type** entries = new value_type*[rows]{};
    try {
        for (size_type row = 0; row < rows; ++row) {
            entries[row] = new value_type[cols]{};
        }
    } catch (...) {
        deleteRows(entries, rows);
        throw;
    }
    return entries;
}

void Matrix::deleteRows(value_type** entries, size_type rows) noexcept {
    if (entries == nullptr) {
        return;
    }
    for (size_type row = 0; row < rows; ++row) {
        delete[] entries[row];
    }
    delete[] entries;
}

void Matrix::swap(Matrix& other) noexcept {
    using std::swap;
    swap(rows_, other.rows_);
    swap(cols_, other.cols_);
    swap(initialized_, other.initialized_);
    swap(entries_, other.entries_);
}

void Matrix::requireInitialized() const {
    if (!initialized_) {
        throw std::logic_error("matrix has not been initialized");
    }
}

void Matrix::requireSameDimensions(const Matrix& rhs) const {
    requireInitialized();
    rhs.requireInitialized();
    if (rows_ != rhs.rows_ || cols_ != rhs.cols_) {
        throw std::invalid_argument("matrix dimensions must match");
    }
}

}
