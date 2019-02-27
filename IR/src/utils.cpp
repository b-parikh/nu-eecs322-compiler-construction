#include <utils.h>

namespace IR {
    // prepends '%' if variable, else returns labelName
    std::string varNameModifier(Item* itp) {
        std::string newString = itp->labelName;
        if(itp->itemType == Atomic_Type::var) {
            newString = "%" + newString;
        }
        
        return newString;

    }

    L3::Item* createL3Item(L3::Atomic_Type Type, std::string label) {
        L3::Item* newItem = new L3::Item();
        newItem->Type = Type;
        newItem->labelName = label;
        return newItem;
    }



}
