#ifndef CIRCUIT_H
#define CIRCUIT_H
#include <vector>
#include "Component.h"

struct Circuit {
    std::vector<std::vector<Component>> components;

    void addBranch(std::vector<Component> comps){
        components.push_back(comps);
    }
};
#endif // CIRCUIT_H