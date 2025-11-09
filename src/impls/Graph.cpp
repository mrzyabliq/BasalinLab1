#include "Graph.h"

Graph::Graph(Circuit circuit) {
  for (auto branch : circuit.components) {
    for (auto comp : branch) {
      vertices.insert(comp.terminalIn);
      vertices.insert(comp.terminalOut);
    }
  }
  nodeVertices = vertices.size();
  numVertices = nodeVertices;

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
          nextVertex = nodeVertices + i;
          vertices.insert(nextVertex);
          graph[nextVertex] = std::vector<std::pair<int, Component>>();
        }

        graph[currentVertex].emplace_back(nextVertex, comp);

        currentVertex = nextVertex;
      }
    }
  }

  numVertices = vertices.size();

  selectTree();
  buildMatrix();
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

      bool isOnPath = isBranchOnPath(branchStart, branchEnd, path);

      if (isOnPath) {
        int sign =
            determineSign(chordStart, chordEnd, branchStart, branchEnd, path);
        (*M)[chordIdx][branchIdx] = sign;
      } else {
        (*M)[chordIdx][branchIdx] = 0;
      }
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

bool Graph::isBranchOnPath(int branchStart, int branchEnd,
                           const std::vector<int>& path) {
  for (size_t i = 0; i < path.size() - 1; ++i)
    if ((path[i] == branchStart && path[i + 1] == branchEnd) ||
        (path[i] == branchEnd && path[i + 1] == branchStart))
      return true;
  return false;
}

int Graph::determineSign(int chordStart, int chordEnd, int branchStart,
                         int branchEnd, const std::vector<int>& path) {
  for (size_t i = 0; i < path.size() - 1; ++i)
    if (path[i] == branchStart && path[i + 1] == branchEnd)
      return isSameDirection(chordStart, chordEnd, path) ? -1 : 1;
    else if (path[i] == branchEnd && path[i + 1] == branchStart)
      return isSameDirection(chordStart, chordEnd, path) ? 1 : -1;
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

std::string Graph::getMatrixWithLabels() {
  if (!M) return "Matrix is empty";

  std::vector<std::pair<std::pair<int, int>, Component>> treeBranches;
  for (const auto& [vertex, neighbors] : tree)
    for (const auto& [neighbor, comp] : neighbors)
      treeBranches.emplace_back(std::make_pair(vertex, neighbor), comp);

  std::string result = "M matrix (" + std::to_string(M->getRows()) + "x" +
                       std::to_string(M->getCols()) + "):\n";

  result += "    ";
  for (size_t j = 0; j < treeBranches.size(); ++j)
    result += treeBranches[j].second.name + "  ";
  result += "\n";

  for (size_t i = 0; i < M->getRows(); ++i) {
    std::string chordName = chords[i].second.name + ":";
    result += chordName + std::string(4 - chordName.length(), ' ');

    for (size_t j = 0; j < M->getCols(); ++j) {
      double value = (*M)[i][j];
      result += (value == static_cast<int>(value))
                    ? std::to_string(static_cast<int>(value))
                    : std::to_string(value);
      result += "  ";
    }
    result += "\n";
  }

  return result;
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