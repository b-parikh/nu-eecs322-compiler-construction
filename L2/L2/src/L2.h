#pragma once

#include <vector>
#include <unordered_set>
#include <unordered_map>

namespace L2 {
  
  typedef std::unordered_set<std::string> set_of_str;
  typedef std::vector<std::string> vector_of_str;
  typedef std::unordered_map<std::string, set_of_str> str_to_set;

  enum class Type{num, label, reg, mem, var, runtime, arith_oper, inc_dec_oper, compare_oper, shift_oper, assign_oper};

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
   * Instruction with registers allocated.
   */
  struct L1_Instruction{
      std::vector<Item> items;
  };

  /*
   * Will be used to generate L1 code.
   */
  struct L1_Function{
      std::string name;
      std::vector<L1_Instruction*> instructions;
      int arguments;
      int locals;
  }

  /*
   * Function.
   */
  struct Function{
    std::string name;
    int64_t arguments;
    int64_t locals;
    std::vector<Instruction *> instructions;
    str_to_set IG; 
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