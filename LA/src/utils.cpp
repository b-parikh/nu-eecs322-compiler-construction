#include <utils.h>

namespace LA {
    // prepends '%' if variable, else returns labelName
    std::string varNameModifier(Item* itp) {
        std::string newString = itp->labelName;
        if(itp->itemType == Atomic_Type::var) {
            newString = "%" + newString;
        }
        
        return newString;

    }
}
