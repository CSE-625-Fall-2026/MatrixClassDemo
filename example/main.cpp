#include "matrix/Matrix.hpp"

#include <iostream>

int main() {
    matrix::Matrix<double> left(2, 2);
    left.set(0, 0, 1.0);
    left.set(0, 1, 2.0);
    left.set(1, 0, 3.0);
    left.set(1, 1, 4.0);

    matrix::Matrix<double> identity;
    identity.setNumRows(2);
    identity.setNumCols(2);
    identity.initialize();
    identity.set(0, 0, 1.0);
    identity.set(1, 1, 1.0);

    const matrix::Matrix<double> result = left * identity;
    std::cout << "result(1, 0) = " << result.get(1, 0) << '\n';
}
