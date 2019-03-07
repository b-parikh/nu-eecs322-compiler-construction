#include <check_array_allocation.h>

namespace LA {
   
    // if the init instruction is for array or tuple, then initialize the array or tuple to 0
    // can assume this is init instruction and ip->Items[0] is the type in question
    std::vector<std::vector<std::string>> initialize_arrays_and_tuples(Instruction* ip, std::string newVarLabel, int& varNameCounter, std::string newLabel, int& labelNameCounter) {
        std::vector<std::string> ret_strings;
        std::vector<std::vector<std::string>> ret_vectors;
        VarType currVarType = ip->Items[0]->varType;
        if(currVarType == VarType::arr_type || currVarType == VarType::tuple_type) {
            std::string varName = "%" + ip->Items[0]->labelName;
            ret_strings.insert(ret_strings.end(), {varName, "<-", "0"});
            ret_vectors.push_back(ret_strings);
        }

        return ret_vectors;
    }

    // before each array store or load, check to see if array is equal to 0 to ensure it has been allocated to
    std::vector<std::vector<std::string>> check_array_allocation(Instruction* ip, std::string newVarLabel, int& varNameCounter, std::string newLabel, int& labelNameCounter) {
        std::vector<std::string> ret_strings;
        std::vector<std::vector<std::string>> ret_vectors;

        std::string arrName;
        if(ip->Type == InstructionType::assign_store_array)
            arrName = "%" + ip->Items[0]->labelName;
        else // load
            arrName = "%" + ip->Items[1]->labelName;

        std::string equalsZeroVar = "%equalsZeroVar_" + newVarLabel + "_" + std::to_string(varNameCounter);
        varNameCounter++;

        std::string arrErrLabel = ":arrErr_" + newLabel + std::to_string(labelNameCounter);
        labelNameCounter++;

        std::string contLabel =  ":contLabel_" + newLabel + std::to_string(labelNameCounter);
        labelNameCounter++;

        ret_strings.insert(ret_strings.end(), {"int64", equalsZeroVar});
        ret_vectors.push_back(ret_strings);
        ret_strings.clear();

        ret_strings.insert(ret_strings.end(), {equalsZeroVar, "<-", arrName, "=", "0"});
        ret_vectors.push_back(ret_strings);
        ret_strings.clear();

        ret_strings.insert(ret_strings.end(), {"br", equalsZeroVar, arrErrLabel, contLabel});
        ret_vectors.push_back(ret_strings);
        ret_strings.clear();

        ret_strings.insert(ret_strings.end(), {arrErrLabel});
        ret_vectors.push_back(ret_strings);
        ret_strings.clear();

        ret_strings.insert(ret_strings.end(), {"call array-error (0, 0)"});
        ret_vectors.push_back(ret_strings);
        ret_strings.clear();

        // to preserve basic block structure
        ret_strings.insert(ret_strings.end(), {"br", contLabel});
        ret_vectors.push_back(ret_strings);
        ret_strings.clear();

        ret_strings.insert(ret_strings.end(), {contLabel});
        ret_vectors.push_back(ret_strings);
        ret_strings.clear();

        return ret_vectors;
    }
} 
