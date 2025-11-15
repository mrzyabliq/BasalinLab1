#include "Graph.h"

Graph::Graph(Circuit circuit, std::vector<std::string> outputs) {
  this->outputs = outputs;
  for (auto branch : circuit.components) {
    for (auto comp : branch) {
      vertices.insert(comp.terminalIn);
      vertices.insert(comp.terminalOut);
    }
  }
  nodeVertices = vertices.size() - 1;

  for (int vertex : vertices)
    graph[vertex] = std::vector<std::pair<int, Component>>();

  for (auto branch : circuit.components) {
    if (branch.empty()) continue;

    if (branch.size() == 1) {
      Component comp = branch[0];
      graph[comp.terminalIn].emplace_back(comp.terminalOut, comp);
    } else {
      int currentVertex = branch[0].terminalIn;

      for (size_t i = 0; i < branch.size(); ++i) {
        Component comp = branch[i];
        int nextVertex;

        if (i == branch.size() - 1) {
          nextVertex = comp.terminalOut;
        } else {
          nodeVertices += 1;
          nextVertex = nodeVertices;
          std::cout << "nextVertex: " << nextVertex << std::endl;
          vertices.insert(nextVertex);
          graph[nextVertex] = std::vector<std::pair<int, Component>>();
        }

        graph[currentVertex].emplace_back(nextVertex, comp);

        currentVertex = nextVertex;
      }
    }
  }

  selectTree();
  buildMatrix();
  buildBigMatrix();
}

void Graph::selectTree() {
  for (int vertex : vertices)
    tree[vertex] = std::vector<std::pair<int, Component>>();

  std::vector<std::pair<Component, std::pair<int, int>>> allEdges;

  for (const auto& [vertex, neighbors] : graph)
    for (const auto& [neighbor, comp] : neighbors)
      allEdges.emplace_back(comp, std::make_pair(vertex, neighbor));

  std::sort(allEdges.begin(), allEdges.end(), [](const auto& a, const auto& b) {
    return getPriority(a.first.type) < getPriority(b.first.type);
  });

  std::map<int, int> parent;
  for (int vertex : vertices) {
    parent[vertex] = vertex;
  }

  auto find = [&](int x) {
    while (parent[x] != x) {
      parent[x] = parent[parent[x]];
      x = parent[x];
    }
    return x;
  };

  auto unite = [&](int x, int y) {
    int rootX = find(x);
    int rootY = find(y);
    if (rootX != rootY) {
      parent[rootY] = rootX;
      return true;
    }
    return false;
  };

  for (const auto& [comp, edge] : allEdges) {
    int u = edge.first;
    int v = edge.second;

    if (unite(u, v))
      tree[u].emplace_back(v, comp);
    else
      chords.emplace_back(edge, comp);
  }

  std::sort(chords.begin(), chords.end(), [](const auto& a, const auto& b) {
    return getPriority(a.second.type) < getPriority(b.second.type);
  });
}

void Graph::buildMatrix() {
  std::vector<std::pair<std::pair<int, int>, Component>> treeBranches;

  for (const auto& [vertex, neighbors] : tree)
    for (const auto& [neighbor, comp] : neighbors)
      treeBranches.emplace_back(std::make_pair(vertex, neighbor), comp);

  size_t numChords = chords.size();
  size_t numTreeBranches = treeBranches.size();

  M = std::make_shared<Matrix>(numChords, numTreeBranches);

  for (size_t chordIdx = 0; chordIdx < chords.size(); ++chordIdx) {
    const auto& chord = chords[chordIdx];
    int chordStart = chord.first.first;
    int chordEnd = chord.first.second;

    std::vector<int> path = findPathInTree(chordStart, chordEnd);

    for (size_t branchIdx = 0; branchIdx < treeBranches.size(); ++branchIdx) {
      const auto& branch = treeBranches[branchIdx];
      int branchStart = branch.first.first;
      int branchEnd = branch.first.second;

      int sign =
          determineSign(chordStart, chordEnd, branchStart, branchEnd, path);
      (*M)[chordIdx][branchIdx] = sign;
    }
  }
}

std::vector<int> Graph::findPathInTree(int start, int end) {
  std::map<int, bool> visited;
  std::map<int, int> parent;
  std::queue<int> queue;

  for (int vertex : vertices) visited[vertex] = false;

  queue.push(start);
  visited[start] = true;
  parent[start] = -1;

  while (!queue.empty()) {
    int current = queue.front();
    queue.pop();

    if (current == end) break;

    if (tree.find(current) != tree.end())
      for (const auto& [neighbor, comp] : tree[current])
        if (!visited[neighbor]) {
          visited[neighbor] = true;
          parent[neighbor] = current;
          queue.push(neighbor);
        }

    for (const auto& [vertex, neighbors] : tree)
      for (const auto& [neighbor, comp] : neighbors)
        if (neighbor == current && !visited[vertex]) {
          visited[vertex] = true;
          parent[vertex] = current;
          queue.push(vertex);
        }
  }

  std::vector<int> path;
  int current = end;
  while (current != -1) {
    path.push_back(current);
    current = parent[current];
  }
  std::reverse(path.begin(), path.end());

  return path;
}

int Graph::determineSign(int chordStart, int chordEnd, int branchStart,
                         int branchEnd, const std::vector<int>& path) {
  for (size_t i = 0; i < path.size() - 1; ++i)
    if (path[i] == branchStart && path[i + 1] == branchEnd)
      return -1;
    else if (path[i] == branchEnd && path[i + 1] == branchStart)
      return 1;
  return 0;
}

void Graph::printTree() {
  std::cout << "TREE:" << std::endl;
  for (const auto& [vertex, neighbors] : tree)
    if (!neighbors.empty()) {
      std::cout << vertex << ": ";
      for (const auto& [neighbor, comp] : neighbors) {
        std::cout << "(" << comp.name << " ["
                  << componentTypeToString(comp.type) << "], " << neighbor
                  << ") ";
      }
      std::cout << std::endl;
    }
}

void Graph::printChords() {
  std::cout << "CHORDS:" << std::endl;
  for (const auto& [edge, comp] : chords)
    std::cout << edge.first << " - " << edge.second << ": " << comp.name << " ["
              << componentTypeToString(comp.type) << "]" << std::endl;
}

void Graph::printGraph() {
  std::cout << "GRAPH:" << std::endl;
  for (const auto& [vertex, neighbors] : graph)
    if (!neighbors.empty()) {
      std::cout << vertex << ": ";
      for (const auto& [neighbor, comp] : neighbors)
        std::cout << "(" << comp.name << " ["
                  << componentTypeToString(comp.type) << "], " << neighbor
                  << ") ";
      std::cout << std::endl;
    }
}

void Graph::printMatrixWithLabels() {
  if (!M) return;

  std::vector<std::pair<std::pair<int, int>, Component>> treeBranches;
  for (const auto& [vertex, neighbors] : tree)
    for (const auto& [neighbor, comp] : neighbors)
      treeBranches.emplace_back(std::make_pair(vertex, neighbor), comp);

  std::cout << "M matrix (" << M->getRows() << "x" << M->getCols() << "):\n";

  std::cout << "    ";
  for (size_t j = 0; j < treeBranches.size(); ++j)
    std::cout << treeBranches[j].second.name << "  ";
  std::cout << std::endl;

  for (size_t i = 0; i < M->getRows(); ++i) {
    std::string chordName = chords[i].second.name + ":";
    std::cout << chords[i].second.name << ":"
              << std::string(4 - chordName.length(), ' ');

    for (size_t j = 0; j < M->getCols(); ++j) std::cout << (*M)[i][j] << "  ";
    std::cout << std::endl;
  }
}

void Graph::printMSystem() {
  std::cout << "___" << std::endl;
  for (int i = 0; i < chords.size(); i++) {
    std::cout << "| U_" << chords[i].second.name << " = ";
    int treeIndex = 0;
    for (auto& [key, vec] : tree) {
      for (auto& [first, component] : vec) {
        int coef = (*M)[i][treeIndex];
        switch (coef) {
          case 1:
            std::cout << "- U_" << component.name << " ";
            break;
          case -1:
            std::cout << "+ U_" << component.name << " ";
            break;
          default:
            break;
        }
        treeIndex++;
      }
    }
    std::cout << std::endl;
  }
  std::cout << "|" << std::endl;
  int treeIndex = 0;
  for (auto& [key, vec] : tree) {
    for (auto& [first, component] : vec) {
      std::cout << "| I_" << component.name << " = ";
      for (int i = 0; i < chords.size(); i++) {
        int coef = (*M)[i][treeIndex];
        switch (coef) {
          case 1:
            std::cout << "+ I_" << chords[i].second.name << " ";
            break;
          case -1:
            std::cout << "- I_" << chords[i].second.name << " ";
            break;
          default:
            break;
        }
      }
      treeIndex++;
      std::cout << std::endl;
    }
  }
  std::cout << "¯¯¯¯" << std::endl;
}

std::map<std::string, int> Graph::getColsDict() {
  std::map<std::string, int> answer;
  graphCount = 0;
  for (auto& [key, vec] : graph)
    for (auto& [first, component] : vec) graphCount++;
  int treeIndex = 0;
  for (auto& [key, vec] : graph) {
    for (auto& [first, component] : vec) {
      if (component.type == ComponentType::Capacitor)
        stateVariables[treeIndex] = component;
      if (component.type == ComponentType::Inductor)
        stateVariables[treeIndex + graphCount] = component;
      if (contains(outputs, component.name))
        outputVariables[treeIndex + graphCount] = component;
      if (component.type == ComponentType::CurrentSource)
        sourceVariables[treeIndex + graphCount] = component;
      if (component.type == ComponentType::VoltageSource)
        sourceVariables[treeIndex] = component;

      answer["U_" + component.name] = treeIndex;
      answer["I_" + component.name] = treeIndex + graphCount;
      answer["dU_" + component.name + "/dt"] = treeIndex + graphCount * 2;
      answer["dI_" + component.name + "/dt"] = treeIndex + graphCount * 3;
      treeIndex++;
    }
  }
  return answer;
}

void Graph::buildBigMatrix() {
  auto compToInd = getColsDict();
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
  bigM = std::make_shared<Matrix>(rowsCount, compToInd.size());

  int currentRow = 0;
  for (int i = 0; i < chords.size(); i++) {
    (*bigM)[currentRow][compToInd["U_" + chords[i].second.name]] = -1;
    int treeIndex = 0;
    for (auto& [key, vec] : tree) {
      for (auto& [first, component] : vec) {
        if ((*M)[i][treeIndex] != 0.0) {
          (*bigM)[currentRow][compToInd["U_" + component.name]] =
              -(*M)[i][treeIndex];
        }
        treeIndex++;
      }
    }
    rowsNames.push_back(chords[i].second.name);
    currentRow++;
  }

  int treeIndex = 0;
  for (auto& [key, vec] : tree) {
    for (auto& [first, component] : vec) {
      (*bigM)[currentRow][compToInd["I_" + component.name]] = -1;
      for (int i = 0; i < chords.size(); i++) {
        if ((*M)[i][treeIndex] != 0) {
          (*bigM)[currentRow][compToInd["I_" + chords[i].second.name]] =
              (*M)[i][treeIndex];
        }
      }
      rowsNames.push_back(component.name);
      currentRow++;
      treeIndex++;
    }
  }

  for (auto resistor : resistors) {
    (*bigM)[currentRow][compToInd["U_" + resistor.name]] = -1;
    (*bigM)[currentRow][compToInd["I_" + resistor.name]] = resistor.value;
    rowsNames.push_back(resistor.name);
    currentRow++;
  }
  for (auto capacitor : capacitors) {
    (*bigM)[currentRow][compToInd["I_" + capacitor.name]] = -1;
    (*bigM)[currentRow][compToInd["dU_" + capacitor.name + "/dt"]] =
        capacitor.value;
    rowsNames.push_back(capacitor.name);
    currentRow++;
  }
  for (auto inductor : inductors) {
    (*bigM)[currentRow][compToInd["U_" + inductor.name]] = -1;
    (*bigM)[currentRow][compToInd["dI_" + inductor.name + "/dt"]] =
        inductor.value;
    rowsNames.push_back(inductor.name);
    currentRow++;
  }
}

void Graph::printBigM() {
  for (size_t i = 0; i < (*bigM).getRows(); ++i) {
    for (size_t j = 0; j < (*bigM).getCols(); ++j)
      std::cout << std::setw(6) << (*bigM)[i][j];
    std::cout << std::endl;
  }
}

Matrix Graph::selectMatrix(std::map<int, Component> toFind,
                           std::map<int, Component> toHelp,
                           std::vector<int> ignoreCols) {
  Matrix answer(toFind.size(), (*bigM).getCols());
  int ansIdx = 0;
  std::shared_ptr<Matrix> editBigM = std::make_shared<Matrix>(*bigM);

  for (auto& [key, value] : toFind) {
    for (int col : ignoreCols)
      for (int row = 0; row < (*editBigM).getRows(); row++)
        (*editBigM)[row][col] = 0;
    int workRow = -1;
    for (int i = 0; i < (*editBigM).getRows(); i++)
      if ((*editBigM)[i][key] != 0 && value.name == rowsNames[i]) {
        workRow = i;
        break;
      }
    std::vector<int> usedRows = {workRow};
    int colToEdit = getColToEdit(editBigM, workRow, key, toHelp);
    while (colToEdit != -1 && usedRows.size() < (*editBigM).getRows() - 1) {
      int rowForEdit = getRowForEdit(editBigM, colToEdit, usedRows);
      if (rowForEdit == -1)
        rowForEdit = getRandomRowForEdit(editBigM, colToEdit, workRow);
      double coef =
          (*editBigM)[workRow][colToEdit] / (*editBigM)[rowForEdit][colToEdit];
      for (int i = 0; i < (*editBigM).getCols(); i++)
        (*editBigM)[workRow][i] -= (*editBigM)[rowForEdit][i] * coef;
      usedRows.push_back(rowForEdit);
      colToEdit = getColToEdit(editBigM, workRow, key, toHelp);
    }

    for (int i = 0; i < (*editBigM).getCols(); i++) {
      (*editBigM)[workRow][i] /= (*editBigM)[workRow][key];
      answer[ansIdx][i] = (*editBigM)[workRow][i];
    }
    ansIdx++;
  }
  return answer;
}

int Graph::getColToEdit(std::shared_ptr<Matrix> M, int row, int col,
                        std::map<int, Component> cols) {
  for (int i = 0; i < (*M).getCols(); i++)
    if (i != col && cols.count(i) == 0 && (*M)[row][i] != 0) return i;
  return -1;
}
int Graph::getRowForEdit(std::shared_ptr<Matrix> M, int col,
                         std::vector<int> usedRows) {
  for (int i = 0; i < (*M).getRows(); i++)
    if (!contains(usedRows, i) && (*M)[i][col] != 0) return i;
  return -1;
}
int Graph::getRandomRowForEdit(std::shared_ptr<Matrix> M, int col,
                               int workRow) {
  std::srand(std::time(0));
  std::vector<int> potentialRows;
  for (int i = 0; i < (*M).getRows(); i++)
    if (i != workRow && (*M)[i][col] != 0) potentialRows.push_back(i);
  if (potentialRows.empty()) return -1;
  return potentialRows[std::rand() % potentialRows.size()];
}

StateSpaceSystem Graph::buildStateSpaceSystem() {
  std::map<int, Component> dxdt;
  for (auto& [key, value] : stateVariables) dxdt[key + graphCount * 2] = value;
  std::map<int, Component> xt;
  for (auto& [key, value] : stateVariables) xt[key] = value;
  xt.insert(sourceVariables.begin(), sourceVariables.end());
  auto selectedDxDt = selectMatrix(dxdt, xt, {});
  std::vector<int> ignore;
  for (auto& [key, value] : stateVariables)
    ignore.push_back(key + graphCount * 2);
  auto selectedYt = selectMatrix(outputVariables, xt, ignore);

  StateSpaceSystem answer;
  answer.A =
      std::make_shared<Matrix>(stateVariables.size(), stateVariables.size());
  answer.B =
      std::make_shared<Matrix>(stateVariables.size(), sourceVariables.size());
  answer.C =
      std::make_shared<Matrix>(stateVariables.size(), stateVariables.size());
  answer.D =
      std::make_shared<Matrix>(stateVariables.size(), sourceVariables.size());
  answer.V = std::make_shared<VMatrix>(1, sourceVariables.size());
  answer.X0 = std::make_shared<Matrix>(stateVariables.size(), 1);
  for (int i = 0; i < stateVariables.size(); i++) {
    int currentCol = 0;
    for (auto& [key, value] : stateVariables) {
      (*answer.A)[i][currentCol] = -selectedDxDt[i][key];
      currentCol++;
    }
    currentCol = 0;
    for (auto& [key, value] : sourceVariables) {
      (*answer.B)[i][currentCol] = -selectedDxDt[i][key];
      currentCol++;
    }
  }
  for (int i = 0; i < outputVariables.size(); i++) {
    int currentCol = 0;
    for (auto& [key, value] : stateVariables) {
      (*answer.C)[i][currentCol] = -selectedYt[i][key];
      currentCol++;
    }
    currentCol = 0;
    for (auto& [key, value] : sourceVariables) {
      (*answer.D)[i][currentCol] = selectedYt[i][key];
      currentCol++;
    }
  }

  int VIndex = 0;
  for (auto& [key, value] : sourceVariables) {
    double sourceValue = value.value;
    (*answer.V).setFunction(
        0, VIndex, [sourceValue](double t) -> double { return sourceValue; });
    VIndex++;
  }

  int X0Index = 0;
  for (auto& [key, value] : stateVariables) {
    std::string variableName = (value.type == ComponentType::Inductor)
                                   ? "I_" + value.name
                                   : "U_" + value.name;
    double varValue;
    std::cout << "Input start " << variableName << ": ";
    std::cin >> varValue;
    (*answer.X0)[X0Index][0] = varValue;
    X0Index++;
  }

  std::cout << "Input modelling time T: ";
  std::cin >> answer.T;
  std::cout << "Input step h: ";
  std::cin >> answer.h;

  return answer;
}
std::vector<std::string> Graph::getX() {
  std::vector<std::string> answer;
  for (auto& [key, component] : stateVariables) {
    switch (component.type) {
      case ComponentType::Capacitor:
        answer.push_back("U_" + component.name);
        break;
      case ComponentType::Inductor:
        answer.push_back("I_" + component.name);
        break;
      default:
        break;
    }
  }
  return answer;
}
std::vector<std::string> Graph::getY() {
  std::vector<std::string> answer;
  for (auto name : outputs) answer.push_back("I_" + name);
  return answer;
}
