#pragma once

#include <vector>
#include <unordered_set>
#include <unordered_map>

namespace L2 {
  
  typedef std::unordered_set<std::string> set_of_str;
  typedef std::vector<std::string> vector_of_str;
  typedef std::unordered_map<std::string, set_of_str> str_to_set; 

  enum class Type{num, label, reg, mem, var, runtime, arith_oper, inc_dec_oper, compare_oper, shift_oper, assign_oper}; 
  enum class Color{red, blue, green, yellow, white, pink, black, orange, purple, gray, brown, cyan, violet, mint, tan, not_assigned};
  enum class REG{r12, r13, r14, r15, rbp, rbx,  r8, r9, r10, r11, rax, rcx, rdi, rdx, rsi, no_reg}; 
  

  struct Item {
      std::string labelName; 
      L2::Type type; 
  };

  /*
   * Instruction interface.
   */
  struct Instruction{
    int identifier;
    std::vector<Item> items;

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
    int64_t arguments;
    int64_t locals;
    std::vector<Instruction *> instructions;
    str_to_set IG; 
    /*
     * IG_nodes is for use in spilling 
     * will take all IG (string -> set) maps and put into this vector as part of each Node
     */
    std::vector<Node> IG_nodes;
    //std::vector<Node> nodes_to_spill;
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
