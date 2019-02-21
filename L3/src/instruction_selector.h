#include <L3.h>
#include <L2.h>

namespace L3 {
    L2::Item* createL2Item(std::string labelName, L2::Type aType);
    L2::Item* choose_Arith_Oper(Instruction* ip);


    L2::Function* instruction_selection(Function* fp);
}

