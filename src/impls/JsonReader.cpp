#include "JsonReader.h"

bool JsonReader::loadFromFile(std::string file_path) {
  std::ifstream config_file(file_path);
  if (!config_file.is_open()) {
    std::cerr << "Error: Cannot open json file: " << file_path << std::endl;
    return false;
  }

  try {
    json config = json::parse(config_file);
    parseCircuit(config);
    parseOutputs(config);
    std::cout << "Circuit configuration loaded successfully!" << std::endl;
    return true;
  } catch (std::exception e) {
    std::cerr << "Error parsing JSON file: " << e.what() << std::endl;
    return false;
  }
}

void JsonReader::printCircuitAndOutputs() {
  std::cout << "=== Circuit Configuration ===" << std::endl;
  std::cout << "\n=== Outputs to Monitor ===" << std::endl;
  for (auto output : outputs_) std::cout << output << " ";
  std::cout << std::endl;
}

void JsonReader::parseCircuit(json config) {
  circuit_ = Circuit();
  for (const auto& branch_array : config["branches"]) {
    std::vector<Component> branch_components;

    for (const auto& comp_json : branch_array) {
      Component comp;
      comp.name = comp_json["name"];
      comp.type = stringToComponentType(comp_json["type"]);
      comp.value = comp_json["value"];
      comp.terminalIn = comp_json["terminalIn"];
      comp.terminalOut = comp_json["terminalOut"];

      branch_components.push_back(comp);

      std::cout << "  Component: " << comp.name << " (" << comp_json["type"]
                << ")"
                << " value: " << comp.value << " nodes: " << comp.terminalIn
                << "->" << comp.terminalOut << std::endl;
    }

    circuit_.addBranch(branch_components);
    std::cout << "Added branch with " << branch_components.size()
              << " components" << std::endl;
  }
}

void JsonReader::parseOutputs(json config) {
  outputs_ = config["outputs"].get<std::vector<std::string>>();
}