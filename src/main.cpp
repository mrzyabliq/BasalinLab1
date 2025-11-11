#include "headers/Matrix.h"
#include "headers/VMatrix.h"
#include "headers/EulerSolver.h"
#include "headers/Circuit.h"
#include "headers/Graph.h"
#include <iostream>
#include <iomanip>
int main() {

    Circuit circuit;
    circuit.addBranch({
        {"L", ComponentType::Inductor, 0.1, 0, 1},
        {"R1", ComponentType::Resistor, 1000.0, 0, 1}
    });
    circuit.addBranch({
        {"C",  ComponentType::Capacitor, 1e-6, 0, 1}
    });
    circuit.addBranch({
        {"R2", ComponentType::Resistor, 2000.0, 0, 1}
    });
    circuit.addBranch({
        {"J",  ComponentType::CurrentSource, 0.001, 1, 0},
    });

    std::vector<std::string> outputs = {"C", "R2"};

    Graph graph(circuit, outputs);
    graph.printGraph();
    graph.printTree();
    graph.printChords();
    graph.printMatrixWithLabels();
    graph.printMSystem();
    graph.printBigM();
    auto system = graph.buildStateSpaceSystem();
    std::cout << (*system.A).toString() << std::endl;
    std::cout << (*system.B).toString() << std::endl;
    std::cout << (*system.C).toString() << std::endl;
    std::cout << (*system.D).toString() << std::endl;
    std::cout << "------------------------------------" << std::endl;

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

    std::cout << A.toString() << std::endl;
    std::cout << B.toString() << std::endl;
    std::cout << C.toString() << std::endl;
    std::cout << D.toString() << std::endl;
    
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
    
    auto results = EulerSolver::Solve((*system.A), (*system.B), C, D, X0, V, h, T);
    
    auto Xstring = graph.getX();
    auto Ystring = graph.getY();
    EulerSolver::PrintResults(results, Xstring, Ystring);
    return 0;
}
