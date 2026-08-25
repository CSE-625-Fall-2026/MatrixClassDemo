#ifndef MATRIX_CLASS_DEMO_MATRIX_HPP
#define MATRIX_CLASS_DEMO_MATRIX_HPP

#include <cstddef>

namespace matrix {

class Matrix {
public:
    using size_type = std::size_t;
    using value_type = double;

    Matrix() = default;
    Matrix(size_type rows, size_type cols);

    Matrix(const Matrix& other);
    Matrix(Matrix&& other) noexcept;
    Matrix& operator=(const Matrix& other);
    Matrix& operator=(Matrix&& other) noexcept;
    ~Matrix();

    void setNumRows(size_type rows);
    void setNumCols(size_type cols);
    void initialize();

    [[nodiscard]] size_type getNumRows() const noexcept;
    [[nodiscard]] size_type getNumCols() const noexcept;
    [[nodiscard]] bool isInitialized() const noexcept;

    [[nodiscard]] value_type get(size_type row, size_type col) const;
    void set(size_type row, size_type col, value_type value);

    value_type& at(size_type row, size_type col);
    const value_type& at(size_type row, size_type col) const;

    Matrix operator+(const Matrix& rhs) const;
    Matrix operator-(const Matrix& rhs) const;
    Matrix operator*(const Matrix& rhs) const;

    Matrix& operator+=(const Matrix& rhs);
    Matrix& operator-=(const Matrix& rhs);
    Matrix& operator*=(const Matrix& rhs);

    [[nodiscard]] bool operator==(const Matrix& rhs) const noexcept;
    [[nodiscard]] bool operator!=(const Matrix& rhs) const noexcept;

private:
    size_type rows_{0};
    size_type cols_{0};
    bool initialized_{false};
    value_type** entries_{nullptr};

    static value_type** allocateRows(size_type rows, size_type cols);
    static void deleteRows(value_type** entries, size_type rows) noexcept;
    void swap(Matrix& other) noexcept;
    void requireInitialized() const;
    void requireSameDimensions(const Matrix& rhs) const;
};

}

#endif
