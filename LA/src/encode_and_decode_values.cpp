#include <encode_and_decode_values.h>

namespace LA {
    
    /**************************************************Encoding**************************************************/

    /*
     * Don't need to do encoding for the following instruction types:
     * - return_empty
     * - br_unconditional
     * - init_var
     * - assign_load_array; can assume that if number is recalled, it will be encoded already
     * - assign_length
     * - label
     */ 

    std::vector<Item*> assign_encode(Instruction* ip) {
        std::vector<Item*> items_to_encode;

        // if Items[1] is var, assume it's been encoded already.
        // else if Items[1] is label, don't encode int64 %some_var
        // else (Items[1] is number) encode
        if(Items[1]->itemType == Atomic_Type::num)
            items_to_encode.push_back(ip->Items[1]);

        return items_to_encode;
    }

    // handles assign_arithmetic and assign_compare
    std::vector<Item*> assign_to_encode(Instruction* ip) {
        std::vector<Item*> items_to_encode;
        items_to_encode.push_back(ip->Items[0]);

        return items_to_encode;
    }
    
    // handles call and call_assign
    std::vector<Item*> call_encode(Instruction* ip) {
            if(arg->itemType == Atomic_Type::num)
                items_to_encode.push_back(arg);
        }

        return items_to_encode;
    }

    std::vector<Item*> return_value_encode(Instruction* ip) {
        std::vector<Item*> items_to_encode;
        if(Items[1]->itemType == Atomic_Type::num)
            items_to_encode.push_back(ip->Items[0]);

        return items_to_encode;
    }

    std::vector<Item*> br_conditional_encode(Instruction* ip) {
        std::vector<Item*> items_to_encode;
        // possible that first arg to br is number as grammer specifies first arg as "t"
        if(ip->Items[0]->itemType == Atomic_Type::num)
            items_to_encode.push_back(ip->Items[0]);

        return items_to_encode;
    }

    std::vector<Item*> assign_store_array_encode(Instruction* ip) {
        std::vector<Item*> items_to_encode;
        if(ip->Items[1]->itemType == Atomic_Type::num)
            items_to_encode.push_back(ip->Items[0]);

        return items_to_encode;
    }

    std::vector<Item*> assign_new_array_encode(Instruction* ip) {
        std::vector<Item*> items_to_encode;
        // assume vars have been encoded already
        for(auto& arg : ip->arguments) {
            if(arg->itemType == Atomic_Type::num)
                items_to_encode.push_back(ip->Items[0]);
        }
        return items_to_encode;
    }
    
    std::vector<Item*> assign_new_tuple_encode(Instruction* ip) {
        std::vector<Item*> items_to_encode;
        // assume vars have been encoded already
        for(auto& arg : ip->arguments) {
            if(arg->itemType == Atomic_Type::num)
                items_to_encode.push_back(ip->Items[0]);
        }
        return items_to_encode;
    }

    std::vector<Item*> print_encode(Instruction* ip) {
        std::vector<Item*> items_to_encode;
        if(ip->Items[0]->itemType == Atomic_Type::num)
            items_to_encode.push_back(ip->Items[0]);

        return items_to_encode;
    }

    std::vector<std::vector<std::string>> encode_all_items(Instruction* ip, std::vector<Item*> items_to_encode, std::string newLabel, int &varNameCounter) {
        std::vector<std::vector<std::string>> ret_vectors;
        std::vector<std::string> ret_strings;
        for(auto &itp : items_to_encode) {
            // don't create new variable to encode already present variable
            if(itp->itemType == Atomic_Type::var) {
                ret_strings.insert(ret_strings.end(), {itp->labelName, "<-", itp->labelName, "<<", "1"});
                ret_vectors.push_back(ret_strings);
                ret_strings.clear();

                ret_strings.insert(ret_strings.end(), {itp->labelname, "<-", itp->labelname, "+", "1"});
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
                ret_strings.insert(ret_strings.end(), {newVarName, "<-", itp->labelName, "<<", "1"});
                ret_vectors.push_back(ret_strings);
                ret_strings.clear();

                ret_strings.insert(ret_strings.end(), {newVarName, "<-", newVarName, "+", "1"});
                ret_vectors.push_back(ret_strings);
                ret_strings.clear();

                // modify Item that's within the instruction to use the new encoded variable
                itp->labelName = newVarName;
            }
        }
            
        return ret_vectors;
    }

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

    /**************************************************Decoding**************************************************/

    /*
     * Don't need to do decoding for the following instruction types:
     * - assign
     * -
     */ 

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

    std::vector<Item*> array_length_to_decode(Instruction* ip) {
        std::vector<Item*> items_to_decode;
        items_to_decode.push_back(ip->Items[2]);

        return items_to_decode;
    }

    std::vector<Item*> assign_to_decode(Instruction* ip) {
        std::vector<Item*> items_to_decode;
        items_to_decode.push_back(ip->Items[1]);
        items_to_decode.push_back(ip->Items[3]);

        return items_to_decode;
    }

    std::vector<std::vector<std::string>> decode_items(Instruction* ip, std::vector<Item*> items_to_decode, std::string newLabel, int &varNameCounter) {
        std::vector<std::vector<std::string>> ret_vectors;
        std::vector<std::string> ret_strings;
        for(auto &itp : items_to_decode) {
            // create new variable to hold decoded version of current variable 
            // all variables being decoded are int64 type
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

        return ret_vectors;
    }

}
