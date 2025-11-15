#ifndef MATRIX_H
#define MATRIX_H
#include <vector>
#include <stdexcept>

class Matrix {
    private:
        size_t rows;
        size_t cols;
        std::vector<std::vector<double>> data;    
    public:
    Matrix(size_t rows, size_t cols): rows(rows), cols(cols), data(rows, std::vector<double>(cols, 0)) {}
    std::vector<double>& operator[](size_t index) const;
    Matrix operator*(const Matrix& other) const;
    Matrix operator*(const double other) const;
    Matrix operator+(const Matrix& other) const;
    std::vector<double> EigenValues();
    std::string toString();
    size_t getCols(){ return cols; };
    size_t getRows(){ return rows; };
};
#endif // MATRIX_H