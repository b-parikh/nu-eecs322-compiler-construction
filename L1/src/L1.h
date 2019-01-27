#pragma once

#include <vector>

namespace L1 {

  struct Item {
    std::string labelName;
  };

  /*
   * Instruction interface.
   */
  struct Instruction{
    int identifier;
    std::vector<Item> items;
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
