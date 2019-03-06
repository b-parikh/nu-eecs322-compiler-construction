#include <check_array_access.h>

namespace LA {
    /*
     * Gets called before every array store and load
     * Prints instructions with call to length function and check if array access is not out-of-bounds
     * Only to be called prior to array load or array store
     */
    std::vector<std::vector<std::string>> check_array_access(Instruction* ip, std::string newVarLabel, int& varNameCounter, std::string newLabel, int& labelNameCounter) {
        std::vector<std::string> ret_strings;
        std::vector<std::vector<std::string>> ret_vectors;

        std::string arrName;
        if(ip->Type == InstructionType::assign_store_array)
            arrName = "%" + ip->Items[0]->labelName;
        else
            arrName = "%" + ip->Items[1]->labelName;

        // temp variable to be used in all array access checks
        std::string newTempVar = "%newTempVar_" + newVarLabel + std::to_string(varNameCounter++);
        ret_strings.insert(ret_strings.end(), {"int64", newTempVar});
        ret_vectors.push_back(ret_strings);
        ret_strings.clear();

        int indexCount = 0;

        for(auto& location : ip->array_access_location) {
            // find length of current dimension
            ret_strings.insert(ret_strings.end(), {newTempVar, "<-", "length", arrName, std::to_string(indexCount)});
            ret_vectors.push_back(ret_strings);
            ret_strings.clear();

            // check if array access is out-of-bound
//            ret_strings.insert(ret_strings.end(), {newTempVar, "<-", location->labelName, ">=", newTempVar});
//            ret_vectors.push_back(ret_strings);
//            ret_strings.clear();

            std::string brVar = "%nbrVar_" + newVarLabel + std::to_string(varNameCounter++);

            ret_strings.insert(ret_strings.end(), {"int64", brVar});
            ret_vectors.push_back(ret_strings);
            ret_strings.clear();

			ret_strings.insert(ret_strings.end(), {brVar, "<-", newTempVar, "<=", location->labelName});
            ret_vectors.push_back(ret_strings);
            ret_strings.clear();

            // branch conditional
            std::string array_error_label = ":array_error_" + newLabel + "_" + std::to_string(labelNameCounter++);
            std::string bound_check_passes_label = ":bound_check_passes_" + newLabel + "_" + std::to_string(labelNameCounter++);
            ret_strings.insert(ret_strings.end(), {"br", brVar, array_error_label, bound_check_passes_label});
            ret_vectors.push_back(ret_strings);
            ret_strings.clear();

            // array_error label + instruction
            ret_strings.push_back(array_error_label);
            ret_vectors.push_back(ret_strings);
            ret_strings.clear();

            ret_strings.insert(ret_strings.end(), {"call", "array-error", "(", arrName, ",", std::to_string(indexCount), ")"});
            ret_vectors.push_back(ret_strings);
            ret_strings.clear();

//            ret_strings.push_back("return");
//            ret_vectors.push_back(ret_strings);
//            ret_strings.clear();
//
            // to keep basic block formatting, add a br unconditional
            ret_strings.insert(ret_strings.end(), {"br", bound_check_passes_label});
            ret_vectors.push_back(ret_strings);
            ret_strings.clear();

            // not out of bound so continue to next dimension
            ret_strings.insert(ret_strings.end(), {bound_check_passes_label});
            ret_vectors.push_back(ret_strings);
            ret_strings.clear();

            // go to next index
            indexCount++;
        }

        return ret_vectors;

    }

}
