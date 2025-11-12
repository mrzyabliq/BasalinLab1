#include "headers/Matrix.h"
#include "headers/VMatrix.h"
#include "headers/EulerSolver.h"
#include "headers/Circuit.h"
#include "headers/Graph.h"
#include <iostream>
#include <iomanip>
int main() {
    double R1 = 1000.0;
    double R2 = 2000.0;
    double L = 0.1;
    double C_val = 1e-6;
    double J = 0.001;

    Circuit circuit;
    circuit.addBranch({
        {"L", ComponentType::Inductor, L, 0, 1},
        {"R1", ComponentType::Resistor, R1, 0, 1}
    });
    circuit.addBranch({
        {"C",  ComponentType::Capacitor, C_val, 0, 1}
    });
    circuit.addBranch({
        {"R2", ComponentType::Resistor, R2, 0, 1}
    });
    circuit.addBranch({
        {"J",  ComponentType::CurrentSource, J, 1, 0},
    });

    std::vector<std::string> outputs = {"R2", "C"};

    Graph graph(circuit, outputs);
    graph.printGraph();
    graph.printTree();
    graph.printChords();
    graph.printMatrixWithLabels();
    graph.printMSystem();
    //graph.printBigM();
    auto system = graph.buildStateSpaceSystem();
    std::cout << "------------------------------------" << std::endl;
    std::cout << "Matrix A:\n" << (*system.A).toString() << std::endl;
    std::cout << "Matrix B:\n" << (*system.B).toString() << std::endl;
    std::cout << "Matrix C:\n" << (*system.C).toString() << std::endl;
    std::cout << "Matrix D:\n" << (*system.D).toString() << std::endl;
    std::cout << "------------------------------------" << std::endl;
    
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
    
    auto results = EulerSolver::Solve((*system.A), (*system.B), (*system.C), (*system.D), X0, V, h, T);
    
    auto Xstring = graph.getX();
    auto Ystring = graph.getY();
    EulerSolver::PrintResults(results, Xstring, Ystring);
    return 0;
}
