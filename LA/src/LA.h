#pragma once

#include <string>
#include <vector>
#include <map>

namespace LA {

    /*
     * Item base class
     * variable child class
     * label child class
     */
    enum class Atomic_Type{label, var, num};
    enum class VarType{int64_type, arr_type, tuple_type, code_type, void_type};
    enum class InstructionType{assign, assign_arithmetic, assign_compare, call, call_assign, return_empty, return_value, br_unconditional, br_conditional, init_var, assign_load_array, assign_store_array, assign_new_array, assign_new_tuple, assign_length, label, print};
    enum class CalleeType{var, label, no_callee};
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
        //std::vector<int> dimensionSize; ( this isn't used )
    };

    /*
     * Instruction interface.
     */
    struct Instruction{
        std::vector<Item*> Items;
        InstructionType Type;

        Arith_Operator Arith_Oper = Arith_Operator::nop; // for arithmetic instructions
        Compare_Operator Comp_Oper = Compare_Operator::nop; // for compare instructions

        CalleeType calleeType = CalleeType::no_callee; // for call instructions
        std::vector<Item*> arguments; // for call instructions and for new array instructions

        std::vector<Item*> array_access_location; // which index of array is being accessed
    };
  
    /*
     * Function.
     */
    struct Function{
        std::string name;
        VarType returnType;
        std::vector<Item*> arguments;
        std::vector<Instruction*> instructions;
        int numDimensions;
    };
  
    /*
     * Program.
     */
    struct Program{
        std::string entryPointLabel;
        std::vector<Function *> functions;
    };
}

