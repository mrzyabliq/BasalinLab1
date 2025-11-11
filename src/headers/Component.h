#ifndef COMPONENT_H
#define COMPONENT_H
#include <string>

enum class ComponentType {
  VoltageSource,
  CurrentSource,
  Resistor,
  Capacitor,
  Inductor
};

struct Component {
  std::string name;
  ComponentType type;
  double value;
  int terminalIn;
  int terminalOut;

  Component() : name(""), type(), value(0.0), terminalIn(-1), terminalOut(-1) {}

  Component(std::string n, ComponentType t, double v, int in, int out)
      : name(n), type(t), value(v), terminalIn(in), terminalOut(out) {}
};

inline int getPriority(ComponentType type) {
  switch (type) {
    case ComponentType::VoltageSource:
      return 1;
    case ComponentType::Capacitor:
      return 2;
    case ComponentType::Resistor:
      return 3;
    case ComponentType::Inductor:
      return 4;
    case ComponentType::CurrentSource:
      return 5;
    default:
      return 6;
  }
}

inline std::string componentTypeToString(ComponentType type) {
  switch (type) {
    case ComponentType::VoltageSource:
      return "V";
    case ComponentType::CurrentSource:
      return "I";
    case ComponentType::Resistor:
      return "R";
    case ComponentType::Capacitor:
      return "C";
    case ComponentType::Inductor:
      return "L";
    default:
      return "?";
  }
}

#endif  // COMPONENT_H