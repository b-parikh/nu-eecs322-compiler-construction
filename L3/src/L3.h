#pragma once

#include <vector>

namespace L3 {
    
    struct Item {
        enum Atomic_Type{num, label, var};
        Atomic_Type Type; 
        union {
            struct number {
                std::string toString;
                int64_t value;
            };
  
            std::string labelName;
        }
    };
  
    /*
     * Instruction interface.
     */
    struct Instruction{
        std::vector<Item> Items;
    };
  
    struct Instruction_assign : Instruction {};

    struct Instruction_assign_arithmetic : Instruction {
        enum Arith_Operator{shift_left, shift_right, plus, minus, multiply, bw_and};
        Arith_Operator Oper;
    };

    struct Instruction_assign_compare : Instruction {
        enum Compare_Operator{gr, geq, le, leq, eq};
        Compare_Operator* Oper;
    };

    struct Instruction_assign_load : Instruction {};

    struct Instruction_assign_store : Instruction {};

    struct Instruction_return_empty : Instruction {};
    
    struct Instruction_return_value : Instruction {};

    struct Instruction_label : Instruction {};

    struct Instruction_br_unconditional : Instruction {};
    
    struct Instruction_br_conditional : Instruction {};

    struct Instruction_call : Instruction {
        enum CalleeType {print, allocate, array-error, var, label};
        CalleeType Type;
        std::vector<Item*> arguments;
    };
    
    struct Instruction_call_assign : Instruction {
        enum CalleeType {print, allocate, array-error, var, label};
        CalleeType Type;
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
