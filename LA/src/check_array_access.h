#pragma once

#include <LA.h>

namespace LA {

    std::vector<std::vector<std::string>> check_array_access(Instruction* ip, std::string newVarLabel, int& varNameCounter, std::string newLabel, int& labelNameCounter);
}
