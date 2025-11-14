#ifndef EULERSOLVER_H
#define EULERSOLVER_H
#include <iomanip>
#include <iostream>

#include "Matrix.h"
#include "VMatrix.h"
using output = std::vector<std::tuple<double, Matrix, Matrix>>;
class EulerSolver {
 public:
  static output Solve(Matrix A, Matrix B, Matrix C, Matrix D, Matrix X0,
                      VMatrix V, double h, double T) {
    output results;
    Matrix Xk = X0;
    for (double t = 0; t <= T; t += h) {
      Matrix Y = C * Xk + D * V.calc(t);
      results.push_back({t, Xk, Y});
      Xk = Xk + (A * Xk + B * V.calc(t)) * h;
    }
    return results;
  }
  static void PrintResults(output results, std::vector<std::string> Xstring,
                           std::vector<std::string> Ystring) {
    std::cout << "Modelling results:" << std::endl;
    std::cout << std::setw(10) << "t, s";
    for (auto x_name : Xstring) {
      std::cout << std::setw(9) << x_name;
      if (x_name[0] == 'U')
        std::cout << ", V";
      else if (x_name[0] == 'I')
        std::cout << ", A";
    }
    for (auto y_name : Ystring) {
      std::cout << std::setw(9) << y_name << ", A";
    }
    std::cout << std::endl;
    std::cout << std::string(58, '-') << std::endl;

    for (size_t i = 0; i < results.size(); i += 1000) {
      double t = std::get<0>(results[i]);
      Matrix& X = std::get<1>(results[i]);
      Matrix& Y = std::get<2>(results[i]);
      std::cout << std::setw(10) << t;
      for (size_t xi = 0; xi < X.getRows(); ++xi) {
        std::cout << std::fixed << std::setprecision(6) << std::setw(12)
                  << X[xi][0];
      }
      for (size_t yi = 0; yi < Y.getRows(); ++yi) {
        std::cout << std::fixed << std::setprecision(6) << std::setw(12)
                  << Y[yi][0];
      }
      std::cout << std::endl;
    }

    if (!results.empty()) {
      auto& last_result = results.back();
      double t_end = std::get<0>(last_result);
      Matrix& X_end = std::get<1>(last_result);
      Matrix& Y_end = std::get<2>(last_result);
      std::cout << std::endl
                << "Stable condition (t = " << t_end << " s):" << std::endl;
      for (size_t xi = 0; xi < X_end.getRows(); ++xi) {
        std::cout << Xstring[xi] << " = " << X_end[xi][0];
        if (Xstring[xi][0] == 'U')
          std::cout << " V" << std::endl;
        else if (Xstring[xi][0] == 'I')
          std::cout << " A" << std::endl;
      }
      for (size_t yi = 0; yi < Y_end.getRows(); ++yi) {
        std::cout << Ystring[yi] << " = " << Y_end[yi][0] << " A" << std::endl;
      }
    }
  }
};
#endif  // EULERSOLVER_H