#pragma once

#include <vector>
#include <unordered_set>

namespace L2 {
  
  typedef std::unordered_set<std::string> set_of_str;

  enum class Type{num, label, reg, mem, var, runtime, oper};

  set_of_str CALLER_SAVED_REGISTERS =
    {"r8", "r9", "r10", "r11", "rax", "rcx", "rdi", "rdx", "rsi"};
 
  set_of_str CALLEE_SAVED_REGISTERS =
    {"r12", "r13", "r14", "r15", "rbp", "rbx"};

  set_of_str ARGUMENT_REGISTERS =
    {"rdi", "rsi", "rdx", "rcx", "r8", "r9"};

  set_of_str RESULT_REGISTERS =
    {"rax"};

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
