#pragma once

#include <L2.h>
#include <utils.h>

namespace L2 {

    void analyze(Function* f);
    // void compute_gen_and_kill(Instruction i);
    // bool regOrVar(Item i);

    /// create analyze wrapper for liveness_only
    // void gk_assignment(Instruction &i);
}
