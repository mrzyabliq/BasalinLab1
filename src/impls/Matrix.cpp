#include "Matrix.h"
#include <algorithm>
#include <Eigen/Dense>
Matrix Matrix::operator*(const Matrix& other) const {
    if (cols != other.rows) throw std::invalid_argument("Matrix dimensions do not allow multiplication");
    Matrix result(rows, other.cols);
    for (size_t i = 0; i < rows; ++i) {
        for (size_t k = 0; k < cols; ++k) {
            double aik = data[i][k];
            for (size_t j = 0; j < other.cols; ++j)
                result[i][j] += aik * other.data[k][j];
        }
    }
    return result;
} 
Matrix Matrix::operator*(const double other) const {
    
    Matrix result(rows, cols);
    for (size_t i = 0; i < rows; ++i) {
        for (size_t k = 0; k < cols; ++k) {
            result[i][k] = data[i][k] * other;
        }
    }
    return result;
}
Matrix Matrix::operator+(const Matrix& other) const {
    if (rows != other.rows || cols != other.cols) throw std::invalid_argument("Matrix dimensions do not allow addition");
    Matrix result(rows, cols);
    for (size_t i = 0; i < rows; ++i) {
        for (size_t k = 0; k < cols; ++k) {
            result[i][k] = data[i][k] + other.data[i][k];
        }
    }
    return result;
} 
std::vector<double>& Matrix::operator[](size_t index) const {
    if (index >= rows) throw std::out_of_range("Row index out of range");
    return const_cast<std::vector<double>&>(data[index]);
}
std::vector<double> Matrix::EigenValues(){
    if (rows != cols) throw std::invalid_argument("Eigenvalues can only be computed for square matrices");

    Eigen::MatrixXd eigenMatrix(rows, cols);
    for(size_t i = 0; i < rows; ++i) {
        for(size_t j = 0; j < cols; ++j) {
            eigenMatrix(i, j) = data[i][j];
        }
    }

    Eigen::EigenSolver<Eigen::MatrixXd> solver(eigenMatrix);
    Eigen::VectorXcd eigenvals = solver.eigenvalues();
    
    std::vector<double> result(rows);
    for(size_t i = 0; i < rows; ++i) {
        result[i] = std::real(eigenvals(i));
    }
    sort(result.begin(), result.end());
    return result;
}
std::string Matrix::toString() {
    std::string matrixString;
    for (size_t i = 0; i < rows; ++i) {
        for (size_t j = 0; j < cols; ++j) {
            matrixString += std::to_string(data[i][j]);
            if (j < cols - 1) matrixString += " ";
        }
        matrixString += "\n";
    }
    return matrixString;
}

