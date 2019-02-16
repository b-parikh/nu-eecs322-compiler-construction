#pragma once

#include <vector>

namespace L3 {
  
    typedef std::unordered_set<std::string> set_of_str;
    typedef std::vector<std::string> vector_of_str;
    typedef std::unordered_map<std::string, set_of_str> str_to_set; 
  
    enum class Type{num, label, reg, mem, var, runtime, arith_oper, inc_dec_oper, compare_oper, shift_oper, assign_oper}; 
    enum class Color{red, blue, green, yellow, white, pink, black, orange, purple, gray, brown, cyan, violet, mint, tan, not_assigned};
    enum class REG{r12, r13, r14, r15, rbp, rbx,  r8, r9, r10, r11, rax, rcx, rdi, rdx, rsi}; 
    
    /*
     * What about operators such as << and >>. We will need to do string comparion to check which operator they are. 
     * Instead what if they had an enum,
     */

    struct Item {
        L2::Type type; 
        union {
            struct number {
                std::string toString;
                int64_t value;
            }
  
            std::string labelName;
        }
    };
  
    /*
     * Instruction interface.
     */
    struct Instruction{
        int identifier;
        std::vector<Item> Items;
    
        std::vector<Instruction *> successors; // for liveness analysis
        set_of_str gen_set;
        set_of_str kill_set;
    
        set_of_str in_set;
        set_of_str prev_in_set;
        
        set_of_str out_set;
        set_of_str prev_out_set;
    
        bool reg_var_assignment = false;
        std::string shifting_var_or_reg = "";
    };
  
    struct Instruction_assign : Instruction {  
        Item* destination;
        Item* source;
    };

    struct Instruction_arithmetic : Instruction {
        Item* destination;
        Item* leftOperand;
        Item* Operator;
        Item* rightOperand;
    };

    struct Instruction_compare : Instruction {
        Item* destination;
        Item* leftOperand;
        Item* comparator;
        Item* rightOperand;
    };

    struct Instruction_load : Instruction {
        Item* destination;
        Item* source;
    };

    struct Instruction_store : Instruction {
        Item* destination;
        Item* source;
    };

    struct Instruction_return_empty : Instruction {};
    
    struct Instruction_return_value : Instruction {
        Item* value;
    };

    struct Instruction_label : Instruction {
        Item* label;
    };

    struct Instruction_call : Instruction {
        Item* callee;
        Item* destination = nullptr;
        std::vector<Item*> arguments;
    };


    /*
     * IG node for graph coloring.
     */
    struct Node{
        std::string name;
        set_of_str neighbors;
        REG color; 
        bool isReg = false;
        bool spilled = false;
        bool colored = false;
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
