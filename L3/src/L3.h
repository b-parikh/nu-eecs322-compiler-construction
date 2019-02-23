#pragma once

#include <vector>
#include <string>
#include <unordered_set>
#include <unordered_map>

namespace L3 {

    typedef std::unordered_set<std::string> set_of_str;
    typedef std::vector<std::string> vector_of_str;
    typedef std::unordered_map<std::string, set_of_str> str_to_set; 

    enum class Compare_Operator{gr, geq, le, leq, eq, nop};
    enum class Arith_Operator{shift_left, shift_right, plus, minus, multiply, bw_and, nop};
    enum class Atomic_Type{num, label, var, runtime}; // runtime for use in trees
    enum class CalleeType {print, allocate, array_error, var, label, no_callee};
    enum class InstructionType{assign, assign_compare, assign_load, assign_arithmetic, assign_store, return_empty, return_value, label, br_unconditional, br_conditional, call, call_assign};

    struct Item {
        Atomic_Type Type; 
        std::string labelName;
    };
  
    /*
     * Instruction interface.
     */
    struct Instruction{
        //virtual ~Instruction() {}; // polymorphic magic
        std::vector<Item*> Items;
        InstructionType Type;

        Instruction* predecessors;
        std::vector<Instruction*> successors;

        Arith_Operator Arith_Oper = Arith_Operator::nop; // for arithmetic instructions
        Compare_Operator Comp_Oper = Compare_Operator::nop; // for compare instructions

        CalleeType calleeType = CalleeType::no_callee; // for call instructions
        std::vector<Item*> arguments; // for call instructions

        set_of_str gen_set;
        set_of_str kill_set;
        set_of_str in_set;
        set_of_str out_set;
    };
  
    // simple assignment (%b <- %a)
//    struct Instruction_assign : Instruction {};
//
//    struct Instruction_assign_arithmetic : Instruction {
//        Arith_Operator Oper;
//    };
//
//    struct Instruction_assign_compare : Instruction {
//        Compare_Operator Oper;
//    };
//
//    struct Instruction_assign_load : Instruction {};
//
//    struct Instruction_assign_store : Instruction {};
//
//    struct Instruction_return_empty : Instruction {};
//    
//    struct Instruction_return_value : Instruction {};
//
//    struct Instruction_label : Instruction {};
//
//    struct Instruction_br_unconditional : Instruction {};
//    
//    struct Instruction_br_conditional : Instruction {};
//
//    struct Instruction_call : Instruction {
//        CalleeType calleeType;
//        std::vector<Item*> arguments;
//    };
//    
//    struct Instruction_call_assign : Instruction {
//        CalleeType calleeType;
//        std::vector<Item*> arguments;
//    };
  
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
