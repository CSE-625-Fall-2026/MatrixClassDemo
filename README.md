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
    GIT_TAG v2.1.0
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

## Versions

| Version | Addition |
| --- | --- |
| `v2.1.0` | `augment()` and a workflow that runs all unit tests on every push |
| `v2.0.0` | Templated matrix arithmetic |
