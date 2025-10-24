#ifndef VMATRIX_H
#define VMATRIX_H
#include "Matrix.h"
#include <functional>
#include <string>
class VMatrix {
    private:
        size_t rows;
        size_t cols;
        std::vector<std::vector<std::function<double(double t)>>> data;
    public:
        VMatrix(size_t rows, size_t cols): rows(rows), cols(cols), data(rows, std::vector<std::function<double(double t)>>(cols, nullptr)) {}
        Matrix calc(double t);
        void setFunction(size_t i, size_t j, std::function<double(double t)> func);
};
#endif // VMATRIX_H