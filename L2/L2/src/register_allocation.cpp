#include <register_allocation.h>

namespace L2 {

    /*
     * Receives a function and returns a struct 
     * with vector of instructions
     */
    const set_of_str GP_REGISTERS( {"r10", "r11", "r12", "r13", "r14", "r15", "r8", "r9", "rax", "rbp", "rbx", "rcx", "rdi", "rcx", "rdi", "rdx", "rsi"} );

    std::vector<L1_Instruction> register_allocation(Function* fp) {
        bool vars_spilled = false;
        do {
            analyze(fp);
            generate_IG(fp);
            bool vars_spilled = color_graph(fp);
        } while(vars_spilled);
    
    }

    bool color_graph(Function *fp) { // IG is required as well, and fp includes IG
        str_to_set IG = fp->IG;
        for(auto const& node : IG) {
            if(GP_registers.find(node.first) != GP_registers.end()) // the node is register
                continue;
            else {
                //spill
                Item spill_var;
                spill_var.labelName = node.first;
                spill_var.Type = Type::var;
                Item spill_str;
                spill_str = node.first;
                spill(*fp, spill_var, spill_str);

                

        

}

