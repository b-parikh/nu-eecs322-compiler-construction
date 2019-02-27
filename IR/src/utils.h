#pragma once

#include <IR.h>
#include <string>
#include <L3.h>

namespace IR {
    std::string varNameModifier(Item* itp);
    L3::Item* createL3Item(L3::Atomic_Type Type, std::string label);
}
