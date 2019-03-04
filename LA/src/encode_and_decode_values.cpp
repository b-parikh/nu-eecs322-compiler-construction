#include <LA.h>

namespace LA {
    
    /**************************************************Decoding**************************************************/
    std::vector<Item*> br_to_decode(Instruction* ip) {
        std::vector<Item*> items_to_decode;
        items_to_decode.push_back(ip->Items[0]);

        return items_to_decode;
    }

    std::vector<Item*> array_load_to_decode(Instruction* ip) {
        std::vector<Item*> items_to_decode;
        items_to_decode.insert(items_to_decode.end(),ip->arguments.begin(), ip->arguments.end());

        return items_to_decode;
    }

    std::vector<Item*> array_store_to_decode(Instruction* ip) {
        std::vector<Item*> items_to_decode;
        items_to_decode.insert(items_to_decode.end(),ip->arguments.begin(), ip->arguments.end());

        return items_to_decode;
    }

    std::vector<Item*> op_assign_to_decode(Instruction* ip) {
        std::vector<Item*> items_to_decode;
        items_to_decode.push_back(ip->Items[1]);
        items_to_decode.push_back(ip->Items[3]);

        return items_to_decode;
    }

    /*
     * Must be called before the instruction ip
     */
    std::vector<std::vector<std::string>> decode_items(Instruction* ip, std::vector<Item*> items_to_decode, std::string newLabel, int &varNameCounter) {
        std::vector<std::vector<std::string>> ret_vectors;
        std::vector<std::string> ret_strings;
        for(auto &itp : items_to_decode) {
            // if var, then no need to create new name for it; just assign the decoded value to the same name as the variable 
            if(itp->itemType == Atomic_Type::var) {
                ret_strings.insert(ret_strings.end(), {itp->labelName, "<-", itp->labelName, ">>", "1"});
                ret_vectors.push_back(ret_strings);
                ret_strings.clear();
            }
            else { // itemType is num (label isn't possible here)
                // initialize new variable 
                std::string newVarName = itp->labelName + "_" + newLabel + "_" + std::to_string(varNameCounter);
                varNameCounter++;

                ret_strings.insert(ret_strings.end(), {"int64", newVarName});
                ret_vectors.push_back(ret_strings);
                ret_strings.clear();

                // new variable holds decoded value
                ret_strings.insert(ret_strings.end(), {newVarName, "<-", itp->labelName, ">>", "1"});
                ret_vectors.push_back(ret_strings);
                ret_strings.clear();

                // modify Item that's within the instruction to use the new decoded variable
                itp->labelName = newVarName;
            }
        }

        return ret_vectors;
    }

    /**************************************************Encoding**************************************************/

    std::vector<Item*> op_assign_to_encode(Instruction* ip) {
        std::vector<Item*> items_to_encode;
        items_to_encode.push_back(ip->Items[0]);

        return items_to_encode;
    }

    /*
     * Must be called after the instruction ip
     */
    std::vector<std::vector<std::string>> encode_items(Instruction* ip, std::vector<Item*> items_to_encode, std::string newLabel, int &varNameCounter) {
        std::vector<std::vector<std::string>> ret_vectors;
        std::vector<std::string> ret_strings;
        for(auto &itp : items_to_encode) {
            ret_strings.insert(ret_strings.end(), {itp->labelName, "<-", itp->labelName, "<<", "1"});
            ret_vectors.push_back(ret_strings);
            ret_strings.clear();
            
            ret_strings.insert(ret_strings.end(), {itp->labelName, "<-", itp->labelName, "+", "1"});
            ret_vectors.push_back(ret_strings);
            ret_strings.clear();
        }
            
        return ret_vectors;
    }

}
