#pragma once

#include <string>
#include <vector>

namespace IR {

    /*
     * Item base class
     * variable child class
     * label child class
     */
    class enum ItemType{label, var};
    class enum VarType{int64_type, arr_type, tuple_type, code_type, void_type};

    struct Item {
        ItemType itemType;
        std::string labelName;
    };

    struct var_Item : Item {
        VarType varType;
    };

    struct label_Item : Item {};

    struct int64_Var : var_Item {
        int value;
    };
    
    struct tuple_Var : var_Item {
        int numDimensions = 1;
        int dimensionSize;
    };

    struct arr_Var : var_Item {
        int numDimensions;
        std::vector<int> dimensionSize;
    };

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
        std::vector<Item*> arguments; // for call instructions
    };
  
    struct basic_block {
        label_Item* starting_label;
        std::vector<Instruction*> instructions;
        std::vector<label_Item> end_label;
        std::<basic_block*> next_block;
    };

    /*
     * Function.
     */
    struct Function{
        std::string name;
        VarType returnType;
        std::vector<var_Item*> arguments;
        std::vector<basic_block*> blocks;
    };
  
    /*
     * Program.
     */
    struct Program{
        std::string entryPointLabel;
        std::vector<Function *> functions;
    };

}

