#pragma once

#include <vector>
#include <unordered_set>
#include <unordered_map>

namespace L2 {
  
  typedef std::unordered_set<std::string> set_of_str;
  typedef std::vector<std::string> vector_of_str;
  typedef std::unordered_map<std::string, set_of_str> str_to_set;

  enum class Type{num, label, reg, mem, var, runtime, oper};

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
    std::vector<Instruction *> successors;
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
   * Instructions.
   */
  struct Instruction_ret : Instruction{
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
  };

  /*
   * Program.
   */
  struct Program{
    std::string entryPointLabel;
    std::vector<Function *> functions;
  };

}
