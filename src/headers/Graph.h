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
#include <iomanip>

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
  std::map<std::string, int> stateVariables;

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

 public:
  Graph(Circuit circuit);
  void printTree();
  void printChords();
  void printGraph();
  Matrix getMatrix() const { return (*M); }
  std::string getMatrixWithLabels();
  void printMSystem();
  std::map<std::string, int> getAnnotToIndDict() {
    std::map<std::string, int> answer;
    int count = 0;
    for (auto& [key, vec] : graph)
      for (auto& [first, component] : vec) count++;
    int treeIndex = 0;
    for (auto& [key, vec] : graph) {
      for (auto& [first, component] : vec) {
        if(component.type == ComponentType::Capacitor)
          stateVariables["U_" + component.name] = treeIndex;
        if(component.type == ComponentType::Inductor)
          stateVariables["I_" + component.name] = treeIndex + count;
        answer["U_" + component.name] = treeIndex;
        answer["I_" + component.name] = treeIndex + count;
        answer["dU_" + component.name + "/dt"] = treeIndex + count * 2;
        answer["dI_" + component.name + "/dt"] = treeIndex + count * 3;
        treeIndex++;
      }
    }
    return answer;
  }
  Matrix selectBigMatrix() {
    auto annotToInd = getAnnotToIndDict();
    std::vector<Component> resistors;
    std::vector<Component> capacitors;
    std::vector<Component> inductors;
    for (auto& [key, vec] : graph) {
      for (auto& [first, component] : vec) {
        switch (component.type) {
          case ComponentType::Resistor:
            resistors.push_back(component);
            break;
          case ComponentType::Capacitor:
            capacitors.push_back(component);
            break;
          case ComponentType::Inductor:
            inductors.push_back(component);
            break;
          default:
            break;
        }
      }
    }
    int rowsCount = M->getRows() + M->getCols() + resistors.size() +
                    capacitors.size() + inductors.size();
    Matrix answer(rowsCount, annotToInd.size());

    for (auto& [key, value] : stateVariables) {
        std::cout << key << ": " << value << std::endl;
    }
    

    int currentRow = 0;
    for (int i = 0; i < chords.size(); i++) {
      answer[currentRow][annotToInd["U_" + chords[i].second.name]] = -1;
      int treeIndex = 0;
      for (auto& [key, vec] : tree) {
        for (auto& [first, component] : vec) {
          if ((*M)[i][treeIndex] != 0.0) {
            answer[currentRow][annotToInd["U_" + component.name]] =
                -(*M)[i][treeIndex];
          }
          treeIndex++;
        }
      }
      currentRow++;
    }

    int treeIndex = 0;
    for (auto& [key, vec] : tree) {
      for (auto& [first, component] : vec) {
        answer[currentRow][annotToInd["I_" + component.name]] = -1;
        for (int i = 0; i < chords.size(); i++){
          if ((*M)[i][treeIndex] != 0){
            answer[currentRow][annotToInd["I_" + chords[i].second.name]] = (*M)[i][treeIndex];
          }
        }
        currentRow++;
        treeIndex++;
      }
    }

    for (auto resistor: resistors){
      answer[currentRow][annotToInd["U_" + resistor.name]] = -1;
      answer[currentRow][annotToInd["I_" + resistor.name]] = resistor.value;
      currentRow++;
    }
    for (auto capacitor: capacitors){
      answer[currentRow][annotToInd["I_" + capacitor.name]] = -1;
      answer[currentRow][annotToInd["dU_" + capacitor.name + "/dt"]] = capacitor.value;
      currentRow++;
    }
    for (auto inductor: inductors){
      answer[currentRow][annotToInd["U_" + inductor.name]] = -1;
      answer[currentRow][annotToInd["dI_" + inductor.name + "/dt"]] = inductor.value;
      currentRow++;
    }
    return answer;
  }
};

#endif  // GRAPH_H