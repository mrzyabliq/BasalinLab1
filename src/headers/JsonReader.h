#ifndef JSONREADER_H
#define JSONREADER_H
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "Circuit.h"
#include "json.hpp"

using json = nlohmann::json;

class JsonReader {
 private:
  Circuit circuit_;
  std::vector<std::string> outputs_;
  std::vector<std::string> stateVars_;
  std::vector<std::string> actives_;

 public:
  JsonReader(std::string filePath) { loadFromFile(filePath); }
  Circuit getCircuit() { return circuit_; }
  std::vector<std::string> getOutputs() { return outputs_; }
  std::vector<std::string> getStateVars() { return stateVars_; }
  std::vector<std::string> getActives() { return actives_; }
  void printCircuitAndOutputs();

 private:
  bool loadFromFile(std::string file_path);
  void parseCircuit(json config);
  void parseOutputs(json config);
  void parseStateVars(json config);
  void parseActives(json config);
};

#endif  // JSONREADER_H
