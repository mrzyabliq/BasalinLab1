#include "headers/Matrix.h"
#include "headers/VMatrix.h"
#include "headers/EulerSolver.h"
#include <iostream>
#include <iomanip>
int main() {
    
    double R1 = 1000.0;
    double R2 = 2000.0;
    double L = 0.1;
    double C_val = 1e-6;
    double J = 0.001;
    
    Matrix A(2, 2);
    A[0][0] = -1.0/(C_val * R2); A[0][1] = -1.0/C_val;
    A[1][0] = 1.0/L;             A[1][1] = -R1/L;
    
    Matrix B(2, 1);
    B[0][0] = 1.0/C_val;
    B[1][0] = 0.0;
    
    Matrix C(2, 2);
    C[0][0] = 1.0/R2;  C[0][1] = 0.0;
    C[1][0] = -1.0/R2; C[1][1] = -1.0;
    
    Matrix D(2, 1);
    D[0][0] = 0.0;
    D[1][0] = 1.0;
    
    Matrix X0(2, 1);
    X0[0][0] = J * R2;
    X0[1][0] = 0.0;
    
    VMatrix V(1, 1);
    V.setFunction(0, 0, [J](double t) -> double {
        return J;
    });
    
    double h = 1e-6;
    double T = 0.01;
    
    std::cout << "Parameters:" << std::endl;
    std::cout << "R1 = " << R1 << " Om, R2 = " << R2 << " Om" << std::endl;
    std::cout << "L = " << L << " Hn, C = " << C_val << " F" << std::endl;
    std::cout << "J = " << J << " A" << std::endl;
    std::cout << "Start conditions: u_C(0) = " << X0[0][0] << " V, i_L(0) = " << X0[1][0] << " A" << std::endl;
    std::cout << "Step h = " << h << " s, modelling time T = " << T << " s" << std::endl;
    std::cout << std::endl;
    
    auto results = EulerSolver::Solve(A, B, C, D, X0, V, h, T);
    
    std::cout << "Modelling results:" << std::endl;
    std::cout << std::setw(10) << "t, s" 
              << std::setw(12) << "u_C, V" 
              << std::setw(12) << "i_L, A" 
              << std::setw(12) << "i2, A" 
              << std::setw(12) << "i3, A" 
              << std::endl;
    std::cout << std::string(58, '-') << std::endl;
    
    for (size_t i = 0; i < results.size(); i += 1000) {
        double t = std::get<0>(results[i]);
        Matrix& X = std::get<1>(results[i]);
        Matrix& Y = std::get<2>(results[i]);
        
        std::cout << std::fixed << std::setprecision(6)
                  << std::setw(10) << t
                  << std::setw(12) << X[0][0]
                  << std::setw(12) << X[1][0]
                  << std::setw(12) << Y[0][0]
                  << std::setw(12) << Y[1][0]
                  << std::endl;
    }
    
    if (!results.empty()) {
        auto& last_result = results.back();
        double t_end = std::get<0>(last_result);
        Matrix& X_end = std::get<1>(last_result);
        Matrix& Y_end = std::get<2>(last_result);
        
        std::cout << std::endl << "Stable condition (t = " << t_end << " s):" << std::endl;
        std::cout << "u_C = " << X_end[0][0] << " V" << std::endl;
        std::cout << "i_L = " << X_end[1][0] << " A" << std::endl;
        std::cout << "i2 = " << Y_end[0][0] << " A" << std::endl;
        std::cout << "i3 = " << Y_end[1][0] << " A" << std::endl;
    }
    
    return 0;
}
