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
    GIT_TAG v2.2.0
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

Tests also fetch ArbitraryInteger and RationalNumber to check these template
types. Applications only need to fetch and link the number libraries they use.

## Versions

| Version | Addition |
| --- | --- |
| `v2.2.0` | `power(int exp)` using successive squaring |
| `v2.1.0` | `augment()` and a workflow that runs all unit tests on every push |
| `v2.0.0` | Templated matrix arithmetic |
