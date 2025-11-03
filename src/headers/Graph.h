#ifndef GRAPH_H
#define GRAPH_H
#include <algorithm>
#include <iostream>
#include <map>
#include <memory>
#include <queue>
#include <set>
#include <utility>
#include <vector>

#include "Circuit.h"
#include "Component.h"
#include "Matrix.h"

struct StateSpaceSystem {
  std::shared_ptr<Matrix> A;
  std::shared_ptr<Matrix> B;
  std::shared_ptr<Matrix> C;
  std::shared_ptr<Matrix> D;
  std::shared_ptr<Matrix> X0;
  std::shared_ptr<Matrix> V;

  std::vector<std::string> state_variables;
  std::vector<std::string> input_variables;
  std::vector<std::string> output_variables;
};

class Graph {
 private:
  int numVertices;
  int nodeVertices;
  std::set<int> vertices;
  std::map<int, std::vector<std::pair<int, Component>>> graph;
  std::map<int, std::vector<std::pair<int, Component>>> tree;
  std::vector<std::pair<std::pair<int, int>, Component>> chords;
  std::shared_ptr<Matrix> M;

  void selectTree();
  void buildMatrix();
  std::vector<int> findPathInTree(int start, int end);
  bool isBranchOnPath(int branchStart, int branchEnd,
                      const std::vector<int>& path);
  int determineSign(int chordStart, int chordEnd, int branchStart,
                    int branchEnd, const std::vector<int>& path);
  bool isSameDirection(int chordStart, int chordEnd,
                       const std::vector<int>& path) {
    return (path.front() == chordStart && path.back() == chordEnd);
  }
  std::pair<std::vector<std::string>, std::vector<std::string>>
  identifyStateAndInputVariables() {
    std::vector<std::string> stateVars;
    std::vector<std::string> inputVars;

    for (const auto& [edge, comp] : chords) {
      if (comp.type == ComponentType::Inductor) {
        stateVars.push_back("I_" + comp.name);
      } else if (comp.type == ComponentType::Capacitor) {
        stateVars.push_back("U_" + comp.name);
      }
    }

    for (const auto& [vertex, neighbors] : tree) {
      for (const auto& [neighbor, comp] : neighbors) {
        if (comp.type == ComponentType::Capacitor) {
          stateVars.push_back("U_" + comp.name);
        }
      }
    }

    for (const auto& [edge, comp] : chords) {
      if (comp.type == ComponentType::VoltageSource) {
        inputVars.push_back("E_" + comp.name);
      } else if (comp.type == ComponentType::CurrentSource) {
        inputVars.push_back("J_" + comp.name);
      }
    }

    for (const auto& [vertex, neighbors] : tree) {
      for (const auto& [neighbor, comp] : neighbors) {
        if (comp.type == ComponentType::VoltageSource) {
          inputVars.push_back("E_" + comp.name);
        } else if (comp.type == ComponentType::CurrentSource) {
          inputVars.push_back("J_" + comp.name);
        }
      }
    }

    return {stateVars, inputVars};
  }

  std::vector<std::string> defineOutputVariables() {
    std::vector<std::string> outputs;

    auto [stateVars, inputVars] = identifyStateAndInputVariables();
    outputs = stateVars;

    for (const auto& [edge, comp] : chords) {
      if (comp.type == ComponentType::Resistor) {
        outputs.push_back("U_" + comp.name);
      }
    }

    for (const auto& [vertex, neighbors] : tree) {
      for (const auto& [neighbor, comp] : neighbors) {
        if (comp.type == ComponentType::Resistor) {
          outputs.push_back("U_" + comp.name);
        }
      }
    }

    return outputs;
  }

  void fillStateSpaceMatrices(StateSpaceSystem& system) {
    // Это упрощённая реализация - в реальности нужен более сложный алгоритм

    size_t n = system.A->getRows();
    size_t m = system.B->getCols();

    // Собираем информацию о компонентах
    std::map<std::string, double> resistances;
    std::map<std::string, double> inductances;
    std::map<std::string, double> capacitances;

    // Заполняем словари параметров компонентов
    fillComponentParameters(resistances, inductances, capacitances);

    for (size_t i = 0; i < system.state_variables.size();
         ++i) {
      const auto& state_var = system.state_variables[i];

      if (state_var.find("I_") == 0) {
        std::string comp_name = state_var.substr(2);
        double L = inductances[comp_name];

        for (size_t j = 0; j < n; ++j) {
          if (i == j) {
            (*system.A)[i][j] = -1.0;
          }
        }
      } else if (state_var.find("U_") == 0) {
        std::string comp_name = state_var.substr(2);
        double C = capacitances[comp_name];

        for (size_t j = 0; j < n; ++j) {
          if (i == j) {
            (*system.A)[i][j] = -1.0;
          }
        }
      }
    }

    fillInputMatrix(system, resistances, inductances, capacitances);

    for (size_t i = 0; i < system.C->getRows() && i < n; ++i) {
      (*system.C)[i][i] = 1.0;
    }
  }

  void fillComponentParameters(std::map<std::string, double>& resistances,
                               std::map<std::string, double>& inductances,
                               std::map<std::string, double>& capacitances) {
    for (const auto& [edge, comp] : chords) {
      switch (comp.type) {
        case ComponentType::Resistor:
          resistances[comp.name] = comp.value;
          break;
        case ComponentType::Inductor:
          inductances[comp.name] = comp.value;
          break;
        case ComponentType::Capacitor:
          capacitances[comp.name] = comp.value;
          break;
        default:
          break;
      }
    }

    for (const auto& [vertex, neighbors] : tree) {
      for (const auto& [neighbor, comp] : neighbors) {
        switch (comp.type) {
          case ComponentType::Resistor:
            resistances[comp.name] = comp.value;
            break;
          case ComponentType::Inductor:
            inductances[comp.name] = comp.value;
            break;
          case ComponentType::Capacitor:
            capacitances[comp.name] = comp.value;
            break;
          default:
            break;
        }
      }
    }
  }

  void fillInputMatrix(StateSpaceSystem& system,
                    const std::map<std::string, double>& resistances,
                    const std::map<std::string, double>& inductances,
                    const std::map<std::string, double>& capacitances) {    
    for (size_t i = 0; i < system.state_variables.size(); ++i) {
        const auto& state_var = system.state_variables[i];
        
        for (size_t j = 0; j < system.input_variables.size(); ++j) {
            const auto& input_var = system.input_variables[j];
            
            if (state_var.find("I_") == 0 && input_var.find("E_") == 0) {
                std::string comp_name = state_var.substr(2);
                if (inductances.find(comp_name) != inductances.end()) {
                    double L = inductances.at(comp_name);
                    (*system.B)[i][j] = 1.0 / L;
                }
            }
            else if (state_var.find("U_") == 0 && input_var.find("J_") == 0) {
                std::string comp_name = state_var.substr(2);
                if (capacitances.find(comp_name) != capacitances.end()) {
                    double C = capacitances.at(comp_name);
                    (*system.B)[i][j] = 1.0 / C;
                }
            }
        }
    }
}

  void initializeStateAndInput(StateSpaceSystem& system) {
    for (size_t i = 0; i < system.X0->getRows(); ++i) {
      (*system.X0)[i][0] = 0.0;
    }

    for (size_t i = 0; i < system.V->getRows(); ++i) {
      (*system.V)[i][0] = 1.0;
    }
  }

 public:
  Graph(Circuit circuit);
  void printTree();
  void printChords();
  void printGraph();
  Matrix getMatrix() const { return (*M); }
  std::string getMatrixWithLabels();

  StateSpaceSystem buildStateSpaceSystem() {
    StateSpaceSystem system;

    auto [stateVars, inputVars] = identifyStateAndInputVariables();
    system.state_variables = stateVars;
    system.input_variables = inputVars;

    system.output_variables = defineOutputVariables();

    size_t n = stateVars.size();
    size_t m = inputVars.size();
    size_t k = system.output_variables.size();

    system.A = std::make_shared<Matrix>(n, n);
    system.B = std::make_shared<Matrix>(n, m);
    system.C = std::make_shared<Matrix>(k, n);
    system.D = std::make_shared<Matrix>(k, m);
    system.X0 = std::make_shared<Matrix>(n, 1);
    system.V = std::make_shared<Matrix>(m, 1);

    fillStateSpaceMatrices(system);

    initializeStateAndInput(system);

    return system;
  }

  void printStateSpaceSystem(const StateSpaceSystem& system) {
    std::cout << "СИСТЕМА УРАВНЕНИЙ СОСТОЯНИЯ:" << std::endl;
    std::cout << "dX/dt = A·X + B·V" << std::endl;
    std::cout << "Y = C·X + D·V" << std::endl << std::endl;

    std::cout << "Переменные состояния (" << system.state_variables.size()
              << "):" << std::endl;
    for (const auto& var : system.state_variables) {
      std::cout << var << " ";
    }
    std::cout << std::endl << std::endl;

    std::cout << "Входные воздействия (" << system.input_variables.size()
              << "):" << std::endl;
    for (const auto& var : system.input_variables) {
      std::cout << var << " ";
    }
    std::cout << std::endl << std::endl;

    std::cout << "Матрица A (" << system.A->getRows() << "×"
              << system.A->getCols() << "):" << std::endl;
    std::cout << system.A->toString() << std::endl;

    std::cout << "Матрица B (" << system.B->getRows() << "×"
              << system.B->getCols() << "):" << std::endl;
    std::cout << system.B->toString() << std::endl;

    std::cout << "Начальные условия X0:" << std::endl;
    std::cout << system.X0->toString() << std::endl;
  }
};

#endif  // GRAPH_H