#pragma once

#include <L2.h>
#include <utils.h>
#include <string>

namespace L2 {
    Function spill(Function f, Item spill_var, Item spill_str);
    void spill_wrapper(Program p);
}
