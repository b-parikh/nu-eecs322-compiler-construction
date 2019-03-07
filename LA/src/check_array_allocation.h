#pragma once

#include <LA.h>

namespace LA {

    /* Initialize an array/tuple variable to 0 at declaration.
     * Before every array/tuple access, check if array/tuple isn't 0
     * If it's 0, call array-error(0,0)
     * (Will work for both arrays and tuples)
     */
    std::vector<std::vector<std::string>> initialize_arrays_and_tuples(Instruction* ip, std::string newVarLabel, int& varNameCounter, std::string newLabel, int& labelNameCounter);

    std::vector<std::vector<std::string>> check_array_allocation(Instruction* ip, std::string newVarLabel, int& varNameCounter, std::string newLabel, int& labelNameCounter);

}
