#include <limits>
#include <stdexcept>
#include <type_traits>
#include <utility>

namespace matrix {

template<typename T>
Matrix<T>::Matrix(size_type rows, size_type cols) {
    setNumRows(rows);
    setNumCols(cols);
    initialize();
}

template<typename T>
Matrix<T>::Matrix(const Matrix& other)
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

template<typename T>
Matrix<T>::Matrix(Matrix&& other) noexcept
    : rows_(other.rows_),
      cols_(other.cols_),
      initialized_(other.initialized_),
      entries_(other.entries_) {
    other.rows_ = 0;
    other.cols_ = 0;
    other.initialized_ = false;
    other.entries_ = nullptr;
}

template<typename T>
Matrix<T>& Matrix<T>::operator=(const Matrix& other) {
    if (this != &other) {
        Matrix copy(other);
        swap(copy);
    }
    return *this;
}

template<typename T>
Matrix<T>& Matrix<T>::operator=(Matrix&& other) noexcept {
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

template<typename T>
Matrix<T>::~Matrix() {
    deleteRows(entries_, rows_);
}

template<typename T>
void Matrix<T>::setNumRows(size_type rows) {
    if (initialized_) {
        throw std::logic_error("cannot change rows after initialization");
    }
    if (rows == 0) {
        throw std::invalid_argument("number of rows must be greater than zero");
    }
    rows_ = rows;
}

template<typename T>
void Matrix<T>::setNumCols(size_type cols) {
    if (initialized_) {
        throw std::logic_error("cannot change columns after initialization");
    }
    if (cols == 0) {
        throw std::invalid_argument("number of columns must be greater than zero");
    }
    cols_ = cols;
}

template<typename T>
void Matrix<T>::initialize() {
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

template<typename T>
typename Matrix<T>::size_type Matrix<T>::getNumRows() const noexcept {
    return rows_;
}

template<typename T>
typename Matrix<T>::size_type Matrix<T>::getNumCols() const noexcept {
    return cols_;
}

template<typename T>
bool Matrix<T>::isInitialized() const noexcept {
    return initialized_;
}

template<typename T>
typename Matrix<T>::value_type Matrix<T>::get(size_type row, size_type col) const {
    return at(row, col);
}

template<typename T>
void Matrix<T>::set(size_type row, size_type col, value_type value) {
    at(row, col) = value;
}

template<typename T>
typename Matrix<T>::value_type& Matrix<T>::at(size_type row, size_type col) {
    requireInitialized();
    if (row >= rows_ || col >= cols_) {
        throw std::out_of_range("matrix index is outside its dimensions");
    }
    return entries_[row][col];
}

template<typename T>
const typename Matrix<T>::value_type& Matrix<T>::at(
    size_type row,
    size_type col
) const {
    requireInitialized();
    if (row >= rows_ || col >= cols_) {
        throw std::out_of_range("matrix index is outside its dimensions");
    }
    return entries_[row][col];
}

template<typename T>
Matrix<T> Matrix<T>::augment(const Matrix& rhs) const {
    requireInitialized();
    rhs.requireInitialized();
    if (rows_ != rhs.rows_) {
        throw std::invalid_argument("augmented matrices must have the same number of rows");
    }
    if (cols_ > std::numeric_limits<size_type>::max() - rhs.cols_) {
        throw std::length_error("augmented matrix has too many columns");
    }

    Matrix result(rows_, cols_ + rhs.cols_);
    for (size_type row = 0; row < rows_; ++row) {
        for (size_type col = 0; col < cols_; ++col) {
            result.set(row, col, get(row, col));
        }
        for (size_type col = 0; col < rhs.cols_; ++col) {
            result.set(row, cols_ + col, rhs.get(row, col));
        }
    }
    return result;
}

template<typename T>
Matrix<T> Matrix<T>::power(int exp) const {
    requireInitialized();
    if (rows_ != cols_) {
        throw std::invalid_argument("matrix powers require a square matrix");
    }
    if (exp < 0) {
        throw std::invalid_argument("matrix exponent must be nonnegative");
    }

    Matrix result(rows_, cols_);
    for (size_type row = 0; row < rows_; ++row) {
        result.set(row, row, value_type{1});
    }
    Matrix factor(*this);
    while (exp > 0) {
        if (exp % 2 == 1) {
            result *= factor;
        }
        exp /= 2;
        if (exp > 0) {
            factor *= factor;
        }
    }
    return result;
}

template<typename T>
Matrix<T> Matrix<T>::rref() const {
    requireInitialized();
    Matrix result(*this);
    size_type pivotRow = 0;
    for (size_type col = 0; col < cols_ && pivotRow < rows_; ++col) {
        size_type pivot = pivotRow;
        while (pivot < rows_ && result.get(pivot, col) == value_type{}) {
            ++pivot;
        }
        if (pivot == rows_) {
            continue;
        }
        std::swap(result.entries_[pivotRow], result.entries_[pivot]);

        const value_type divisor = result.get(pivotRow, col);
        for (size_type j = col + 1; j < cols_; ++j) {
            result.set(pivotRow, j, divide(result.get(pivotRow, j), divisor));
        }
        result.set(pivotRow, col, value_type{1});

        for (size_type row = 0; row < rows_; ++row) {
            if (row == pivotRow) {
                continue;
            }
            const value_type factor = result.get(row, col);
            for (size_type j = col + 1; j < cols_; ++j) {
                result.at(row, j) -= factor * result.get(pivotRow, j);
            }
            result.set(row, col, value_type{});
        }
        ++pivotRow;
    }
    return result;
}

template<typename T>
std::tuple<Matrix<T>, Matrix<T>> Matrix<T>::lu() const {
    requireInitialized();
    if (rows_ != cols_) {
        throw std::invalid_argument("LU decomposition requires a square matrix");
    }
    Matrix lower(rows_, cols_);
    Matrix upper(*this);
    for (size_type row = 0; row < rows_; ++row) {
        lower.set(row, row, value_type{1});
    }

    for (size_type col = 0; col < cols_; ++col) {
        if (upper.get(col, col) == value_type{}) {
            for (size_type row = col + 1; row < rows_; ++row) {
                if (upper.get(row, col) != value_type{}) {
                    throw std::domain_error("LU decomposition requires a row swap");
                }
            }
            continue;
        }
        for (size_type row = col + 1; row < rows_; ++row) {
            const value_type factor = divide(upper.get(row, col), upper.get(col, col));
            lower.set(row, col, factor);
            upper.set(row, col, value_type{});
            for (size_type j = col + 1; j < cols_; ++j) {
                upper.at(row, j) -= factor * upper.get(col, j);
            }
        }
    }
    return std::make_tuple(std::move(lower), std::move(upper));
}

template<typename T>
std::tuple<Matrix<T>, Matrix<T>, Matrix<T>> Matrix<T>::luPartialPivoting() const {
    requireInitialized();
    if (rows_ != cols_) {
        throw std::invalid_argument("LU decomposition requires a square matrix");
    }
    Matrix permutation(rows_, cols_);
    Matrix lower(rows_, cols_);
    Matrix upper(*this);
    for (size_type row = 0; row < rows_; ++row) {
        permutation.set(row, row, value_type{1});
        lower.set(row, row, value_type{1});
    }

    // Compare absolute magnitudes without overflowing on the smallest signed integer.
    const auto largerMagnitude = [](const value_type& a, const value_type& b) {
        if (a < value_type{}) {
            return b < value_type{} ? a < b : a < -b;
        }
        return b < value_type{} ? -a < b : a > b;
    };

    for (size_type col = 0; col < cols_; ++col) {
        size_type pivot = col;
        for (size_type row = col + 1; row < rows_; ++row) {
            if (largerMagnitude(upper.get(row, col), upper.get(pivot, col))) {
                pivot = row;
            }
        }
        if (upper.get(pivot, col) == value_type{}) {
            continue; // The remaining column is already zero.
        }
        std::swap(upper.entries_[col], upper.entries_[pivot]);
        std::swap(permutation.entries_[col], permutation.entries_[pivot]);
        for (size_type j = 0; j < col; ++j) {
            std::swap(lower.at(col, j), lower.at(pivot, j));
        }

        for (size_type row = col + 1; row < rows_; ++row) {
            const value_type factor = divide(upper.get(row, col), upper.get(col, col));
            lower.set(row, col, factor);
            upper.set(row, col, value_type{});
            for (size_type j = col + 1; j < cols_; ++j) {
                upper.at(row, j) -= factor * upper.get(col, j);
            }
        }
    }
    return std::make_tuple(std::move(permutation), std::move(lower), std::move(upper));
}

template<typename T>
typename Matrix<T>::value_type Matrix<T>::divide(
    const value_type& numerator, const value_type& denominator
) {
    if constexpr (std::is_integral_v<T> && std::is_signed_v<T>) {
        if (numerator == std::numeric_limits<T>::lowest() && denominator == T{-1}) {
            throw std::overflow_error("matrix division exceeds the element type's range");
        }
    }
    const value_type quotient = numerator / denominator;
    if constexpr (!std::is_floating_point_v<T>) {
        if (quotient * denominator != numerator) {
            throw std::domain_error("matrix division requires fractions; use double or RationalNumber");
        }
    }
    return quotient;
}

template<typename T>
Matrix<T> Matrix<T>::operator+(const Matrix& rhs) const {
    Matrix result(*this);
    result += rhs;
    return result;
}

template<typename T>
Matrix<T> Matrix<T>::operator-(const Matrix& rhs) const {
    Matrix result(*this);
    result -= rhs;
    return result;
}

template<typename T>
Matrix<T> Matrix<T>::operator*(const Matrix& rhs) const {
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
            value_type sum{};
            for (size_type inner = 0; inner < cols_; ++inner) {
                sum += at(row, inner) * rhs.at(inner, col);
            }
            result.set(row, col, sum);
        }
    }
    return result;
}

template<typename T>
Matrix<T>& Matrix<T>::operator+=(const Matrix& rhs) {
    requireSameDimensions(rhs);
    for (size_type row = 0; row < rows_; ++row) {
        for (size_type col = 0; col < cols_; ++col) {
            entries_[row][col] += rhs.entries_[row][col];
        }
    }
    return *this;
}

template<typename T>
Matrix<T>& Matrix<T>::operator-=(const Matrix& rhs) {
    requireSameDimensions(rhs);
    for (size_type row = 0; row < rows_; ++row) {
        for (size_type col = 0; col < cols_; ++col) {
            entries_[row][col] -= rhs.entries_[row][col];
        }
    }
    return *this;
}

template<typename T>
Matrix<T>& Matrix<T>::operator*=(const Matrix& rhs) {
    *this = *this * rhs;
    return *this;
}

template<typename T>
bool Matrix<T>::operator==(const Matrix& rhs) const {
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

template<typename T>
bool Matrix<T>::operator!=(const Matrix& rhs) const {
    return !(*this == rhs);
}

template<typename T>
typename Matrix<T>::value_type** Matrix<T>::allocateRows(
    size_type rows,
    size_type cols
) {
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

template<typename T>
void Matrix<T>::deleteRows(value_type** entries, size_type rows) noexcept {
    if (entries == nullptr) {
        return;
    }
    for (size_type row = 0; row < rows; ++row) {
        delete[] entries[row];
    }
    delete[] entries;
}

template<typename T>
void Matrix<T>::swap(Matrix& other) noexcept {
    using std::swap;
    swap(rows_, other.rows_);
    swap(cols_, other.cols_);
    swap(initialized_, other.initialized_);
    swap(entries_, other.entries_);
}

template<typename T>
void Matrix<T>::requireInitialized() const {
    if (!initialized_) {
        throw std::logic_error("matrix has not been initialized");
    }
}

template<typename T>
void Matrix<T>::requireSameDimensions(const Matrix& rhs) const {
    requireInitialized();
    rhs.requireInitialized();
    if (rows_ != rhs.rows_ || cols_ != rhs.cols_) {
        throw std::invalid_argument("matrix dimensions must match");
    }
}

}
