# MatrixClassDemo

Simple sequential matrix class using `double` values.

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
    GIT_TAG v1.0.0
)
FetchContent_MakeAvailable(MatrixClassDemo)

target_link_libraries(your_target PRIVATE MatrixClassDemo::matrix)
```

## Example

```cpp
#include <matrix/Matrix.hpp>

matrix::Matrix value(2, 3);
value.set(0, 0, 42.0);
```
