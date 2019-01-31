#pragma once

#include <vector>
#include <unordered_set>

namespace L2 {
  
  typedef std::unordered_set<std::string> set_of_str;

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
    set_of_str gen_set;
    set_of_str kill_set;
    set_of_str in_set;
    set_of_str out_set;
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
  };

  /*
   * Program.
   */
  struct Program{
    std::string entryPointLabel;
    std::vector<Function *> functions;
  };

}
