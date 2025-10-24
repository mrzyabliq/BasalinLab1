#include "VMatrix.h"
Matrix VMatrix::calc(double t) {
    Matrix result(rows, cols);
    for (size_t i = 0; i < rows; ++i) {
        for (size_t j = 0; j < cols; ++j) {
            if (data[i][j]) {
                result[i][j] = data[i][j](t);
            }
            else {
                throw std::runtime_error("Function at position (" + std::to_string(i) + ", " + std::to_string(j) + ") is not defined.");
            }
        }
    }
    return result;
}
void VMatrix::setFunction(size_t i, size_t j, std::function<double(double t)> func) {
    if (i >= rows || j >= cols) {
        throw std::out_of_range("Index out of range");
    }
    data[i][j] = func;
}