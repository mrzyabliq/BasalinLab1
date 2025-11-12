#include <iomanip>
#include <iostream>

#include "headers/Circuit.h"
#include "headers/EulerSolver.h"
#include "headers/Graph.h"
#include "headers/JsonReader.h"
#include "headers/Matrix.h"
#include "headers/VMatrix.h"
#include "json.hpp"

using json = nlohmann::json;

int main() {
  JsonReader reader("/home/maksi/BasalinLab1/inputCircuit.json");
  Circuit circuit = reader.getCircuit();
  std::vector<std::string> outputs = reader.getOutputs();

  Graph graph(circuit, outputs);
  graph.printGraph();
  graph.printTree();
  graph.printChords();
  graph.printMatrixWithLabels();
  graph.printMSystem();
  auto system = graph.buildStateSpaceSystem();
  std::cout << "------------------------------------" << std::endl;
  std::cout << "Matrix A:\n" << (*system.A).toString() << std::endl;
  std::cout << "Matrix B:\n" << (*system.B).toString() << std::endl;
  std::cout << "Matrix C:\n" << (*system.C).toString() << std::endl;
  std::cout << "Matrix D:\n" << (*system.D).toString() << std::endl;
  std::cout << "Matrix X0:\n" << (*system.X0).toString() << std::endl; // example U_C = 2.0, I_L = 0.0
  std::cout << "Modeling time: " << system.T << " with step " << system.h << std::endl; // example T = 0.01, h = 1e-6
  std::cout << "------------------------------------" << std::endl;

  auto results = EulerSolver::Solve((*system.A), (*system.B), (*system.C),
                                    (*system.D), (*system.X0), (*system.V), system.h, system.T);

  auto Xstring = graph.getX();
  auto Ystring = graph.getY();
  EulerSolver::PrintResults(results, Xstring, Ystring);
  return 0;
}
