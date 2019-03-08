#pragma once

#include <string>
#include <vector>
#include <map>

namespace LB {

    enum class Atomic_Type{label, var, num};
    enum class VarType{int64_type, arr_type, tuple_type, code_type, void_type};
    enum class InstructionType{assign, assign_operator, call, call_assign, return_empty, return_value, br_unconditional, if_statement, while_statement, init_var, assign_load_array, assign_store_array, assign_new_array, assign_new_tuple, assign_length, label, print,while_instruction, continue_instruction, break_instruction};
    enum class CalleeType{var, label, no_callee};
    enum class Oper{gr, geq, le, leq, eq, shift_left, shift_right, plus, minus, multiply, bw_and, nop};
//    enum class Compare_Operator{gr, geq, le, leq, eq, nop};
//    enum class Arith_Operator{shift_left, shift_right, plus, minus, multiply, bw_and, nop};

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
    };

    /*
     * Instruction interface.
     */
    struct Instruction{
        /*
         * Common to all instructions
         */
        InstructionType Type;
        std::vector<Item*> Items;

//        Arith_Operator Arith_Oper = Arith_Operator::nop; // for arithmetic instructions
//        Compare_Operator Comp_Oper = Compare_Operator::nop; // for compare instructions
        
        Oper Operator = Oper::nop; // for arithmetic and compare instructions

        CalleeType calleeType = CalleeType::no_callee; // for call instructions
        std::vector<Item*> arguments; // for call instructions and for new array instructions

        std::vector<Item*> array_access_location; // which index of array is being accessed
    };

    /*
     * Scope interface.
     */

    struct Scope {
        std::vector<Instruction*> Instructions;
        int level;
        Scope* parent_scope = nullptr; // if nullptr, then first level scope
        std::vector<Scope*> children_scopes;
        std::map<std::string, Item*> varName_to_Item;
    };
  
    /*
     * Function interface.
     */
    struct Function{
        std::string name;
        VarType returnType;
        std::vector<Item*> arguments;
        Scope* func_scope; // function level scope
        std::map<std::string, Item*> varName_to_Item;
        int numDimensions; // if return type is arr
    };
  
    /*
     * Program.
     */
    struct Program{
        std::string entryPointLabel;
        std::vector<Function *> functions;
    };
}

