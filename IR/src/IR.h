#pragma once

#include <string>
#include <vector>
#include <map>

namespace IR {

    /*
     * Item base class
     * variable child class
     * label child class
     */
    enum class Atomic_Type{label, var, num};
    enum class VarType{int64_type, arr_type, tuple_type, code_type, void_type};
    enum class InstructionType{assign, assign_arithmetic, assign_compare, call, call_assign, return_empty, return_value, br_unconditional, br_conditional, init_var, assign_load_array, assign_store_array, assign_new_array, assign_new_tuple, assign_length, label};
    enum class CalleeType{print,array_error,var,label, no_callee};
    enum class Compare_Operator{gr, geq, le, leq, eq, nop};
    enum class Arith_Operator{shift_left, shift_right, plus, minus, multiply, bw_and, nop};

    struct Item {
        // common to all Items
        Atomic_Type itemType;
        std::string labelName;

        // for variables
        VarType varType;

        // for label_item
        // nothing extra
       
        // for int64_Var
        int value;

        // for tuple_Var and arr_Var
        int numDimensions;
        std::vector<int> dimensionSize;
    };

//    struct var_Item : Item {
//        VarType varType;
//    };
//
//    struct label_Item : Item {};
//
//    struct int64_Var : var_Item {
//        int value;
//    };
//    
//    struct tuple_Var : var_Item {
//        int numDimensions = 1;
//        int dimensionSize;
//    };
//
//    struct arr_Var : var_Item {
//        int numDimensions;
//        std::vector<int> dimensionSize;
//    };

    /*
     * Instruction interface.
     */
    struct Instruction{
        std::vector<Item*> Items;
        InstructionType Type;

        Instruction* predecessors;
        std::vector<Instruction*> successors;

        Arith_Operator Arith_Oper = Arith_Operator::nop; // for arithmetic instructions
        Compare_Operator Comp_Oper = Compare_Operator::nop; // for compare instructions

        CalleeType calleeType = CalleeType::no_callee; // for call instructions
        std::vector<Item*> arguments; // for call instructions and for new array instructions

        std::vector<Item*> array_access_location;

    };
  
    struct Basic_block {
        Item* starting_label;
        std::vector<Instruction*> instructions;
        std::vector<Item*> end_label;
        std::vector<Basic_block*> next_block;
        bool isEmpty = false;
    };

    /*
     * Function.
     */
    struct Function{
        std::string name;
        VarType returnType;
        std::vector<Item*> arguments;
        std::vector<Basic_block*> blocks;
        std::map<std::string, Basic_block*> label_to_block;
    };
  
    /*
     * Program.
     */
    struct Program{
        std::string entryPointLabel;
        std::vector<Function *> functions;
        Basic_block* empty_block; // allocated in build_label_block_map; final return block
    };

}

