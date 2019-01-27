#pragma once

#include <L1.h>
#include <string>
namespace L1{

  void generate_code(Program p);
  std::string progNameModifier(std::string);
  void write_assignment(Instruction*, std::ofstream&);
}
