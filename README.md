# MatrixClassDemo

Simple sequential matrix class with a configurable element type.

## Build and test

```sh
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
```

## FetchContent

```cmake
include(FetchContent)
FetchContent_Declare(
    MatrixClassDemo
    GIT_REPOSITORY https://github.com/CSE-625-Fall-2026/MatrixClassDemo.git
    GIT_TAG v3.0.0
)
FetchContent_MakeAvailable(MatrixClassDemo)

target_link_libraries(your_target PRIVATE MatrixClassDemo::matrix)
```

## Example

```cpp
#include <matrix/Matrix.hpp>

matrix::Matrix<double> value(2, 3);
value.set(0, 0, 42.0);
```

The element type must support the operations used by the matrix operation. For
example, addition requires `+=`, subtraction requires `-=`, and multiplication
requires multiplication and `+=`.

## Augmentation

```cpp
auto combined = a.augment(b); // [a | b]
```

Both matrices must be initialized and have the same number of rows. The result
copies every column of `a`, followed by every column of `b`, into one matrix.
Neither input changes.

## Powers

```cpp
auto fifth = a.power(5);
auto identity = a.power(0);
```

The matrix must be initialized and square. Successive squaring computes a new
matrix using logarithmically many matrix multiplications. Negative exponents
throw `std::invalid_argument`; the original matrix is unchanged.

## Reduced row echelon form

```cpp
auto reduced = a.augment(b).rref();
```

`rref()` returns a new matrix using basic Gauss-Jordan elimination: find a
nonzero pivot, swap rows if needed, make the pivot one, and eliminate that
column from the other rows. It works with rectangular matrices and zero rows.

Use `double`, `RationalNumber<int>`, or `RationalNumber<ArbitraryInteger>` when
fractions are needed. Integer types throw `std::domain_error` if an intermediate
division would truncate. The implementation uses exact zero comparisons;
floating-point roundoff can affect pivot selection. Rational types give exact
results within the range of their underlying integer type.

## LU decomposition

```cpp
auto [L, U] = a.lu(); // std::tuple<Matrix<T>, Matrix<T>>; a = L * U
```

For an initialized square matrix, `lu()` returns a lower triangular matrix with
ones on its diagonal and an upper triangular matrix. It copies the input and
uses ordinary elimination, storing the multipliers in `L`.

This simple version does not pivot: it throws `std::domain_error` when a row
swap is required. A zero pivot with only zeros below it needs no elimination.
The same division and floating-point limitations as `rref()` apply.

Applications using custom number types must include their headers and link
their libraries. MatrixClassDemo itself does not depend on those libraries.

## Versions

| Version | Addition |
| --- | --- |
| `v3.0.0` | `rref()` and `lu()` returning new matrices |
| `v2.2.0` | `power(int exp)` using successive squaring |
| `v2.1.0` | `augment()` and a workflow that runs all unit tests on every push |
| `v2.0.0` | Templated matrix arithmetic |
