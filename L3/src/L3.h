#pragma once

#include <vector>
#include <string>

namespace L3 {

    enum class Compare_Operator{gr, geq, le, leq, eq};
    enum class Arith_Operator{shift_left, shift_right, plus, minus, multiply, bw_and};
    enum class Atomic_Type{num, label, var};
    enum class CalleeType {print, allocate, array_error, var, label};
    enum class InstructionType{assign, assign_compare, assign_load, assign_arithmetic, assign_store, return_empty, return_value, label, br_unconditional, br_conditional, call, call_assign};

    struct Item {
        Atomic_Type Type; 
        std::string labelName;
    };
  
    /*
     * Instruction interface.
     */
    struct Instruction{
        std::vector<Item*> Items;
        InstructionType Type;
    };
  
    // simple assignment (%b <- %a)
    struct Instruction_assign : Instruction {};

    struct Instruction_assign_arithmetic : Instruction {
        Arith_Operator Oper;
    };

    struct Instruction_assign_compare : Instruction {
        Compare_Operator Oper;
    };

    struct Instruction_assign_load : Instruction {};

    struct Instruction_assign_store : Instruction {};

    struct Instruction_return_empty : Instruction {};
    
    struct Instruction_return_value : Instruction {};

    struct Instruction_label : Instruction {};

    struct Instruction_br_unconditional : Instruction {};
    
    struct Instruction_br_conditional : Instruction {};

    struct Instruction_call : Instruction {
        CalleeType calleeType;
        std::vector<Item*> arguments;
    };
    
    struct Instruction_call_assign : Instruction {
        CalleeType calleeType;
        std::vector<Item*> arguments;
    };
  
    /*
     * Function.
     */
    struct Function{
        std::string name;
        bool isMain = false;
        std::vector<Item*> arguments;
        std::vector<Instruction *> instructions;
    };
  
    /*
     * Program.
     */
    struct Program{
        std::string entryPointLabel;
        std::vector<Function *> functions;
        std::vector<Item>  spill_extras;
    };

}
