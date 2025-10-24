#ifndef EULERSOLVER_H
#define EULERSOLVER_H
#include "Matrix.h"
#include "VMatrix.h"
using output = std::vector<std::tuple<double, Matrix, Matrix>>;
class EulerSolver {
    public:
    static output Solve(Matrix A, Matrix B, Matrix C, Matrix D, Matrix X0, VMatrix V, double h, double T) {
        output results;
        Matrix Xk = X0;
        for(double t = 0; t <= T; t += h) {
            Matrix Y = C * Xk + D * V.calc(t);
            results.push_back({t, Xk, Y});
            Xk = Xk + (A * Xk + B * V.calc(t)) * h;
        }
        return results;

    }
};
#endif // EULERSOLVER_H