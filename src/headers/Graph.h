#ifndef GRAPH_H
#define GRAPH_H
#include <algorithm>
#include <cmath>
#include <iomanip>
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
#include "VMatrix.h"

struct StateSpaceSystem {
  std::shared_ptr<Matrix> A;
  std::shared_ptr<Matrix> B;
  std::shared_ptr<Matrix> C;
  std::shared_ptr<Matrix> D;
  std::shared_ptr<VMatrix> V;
  std::shared_ptr<Matrix> X0;

  double h;
  double T;
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
  std::shared_ptr<Matrix> bigM;
  std::vector<std::string> outputs;

  template <typename T>
  bool contains(const std::vector<T>& vec, const T& value) {
    return std::find(vec.begin(), vec.end(), value) != vec.end();
  }

  void selectTree();
  void buildMatrix();
  std::vector<int> findPathInTree(int start, int end);
  int determineSign(int chordStart, int chordEnd, int branchStart,
                    int branchEnd, const std::vector<int>& path);
  std::map<std::string, int> getColsDict();
  void buildBigMatrix();
  Matrix selectMatrix(std::map<int, Component> toFind,
                      std::map<int, Component> toHelp,
                      std::vector<int> ignoreCols);
  int getColToEdit(std::shared_ptr<Matrix> M, int row, int col,
                   std::map<int, Component> cols);
  int getRowForEdit(std::shared_ptr<Matrix> M, int col,
                    std::vector<int> usedRows);


  std::map<int, Component> stateVariables;
  std::map<int, Component> outputVariables;
  std::map<int, Component> sourceVariables;

  int graphCount;
public:
  Graph(Circuit circuit, std::vector<std::string> outputs);
  void printTree();
  void printChords();
  void printGraph();
  void printMatrixWithLabels();
  void printMSystem();
  void printBigM();
  StateSpaceSystem buildStateSpaceSystem();
  std::vector<std::string> getX();
  std::vector<std::string> getY();
};

#endif  // GRAPH_H
