#include <utils.h>

namespace L2 {
    std::string varNameModifier(Item i) {
        if(i.type == Type::var)
            (i.labelName).erase(0, 1);
        return i.labelName;
    }

}