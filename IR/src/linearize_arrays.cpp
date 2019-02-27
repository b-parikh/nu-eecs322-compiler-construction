#include <L3.h>
#include <utils.h>
#include <linearize_arrays.h>
/*
 * Accessing length of a dimension
 * Accessing array element (RHS vs LHS)
 * Allocating an array
 */

namespace IR {

// creates a new variable and doesn't overwrite varToDecode with the decoded value
//std::vector<std::string> decode(Item* itemToDecode, std::string labelToAppend, int &counter) {
//    std::vector<std::string> InstructionToReturn;
//    std::string percent = itemToDecode->itemType == Atomic_Type::var ? "%" : ""; // if num don't include "%"
//    InstructionToReturn.push_back("%" + itemToDecode->labelName + labelToAppend + std::to_string(counter));
//    InstructionToReturn.push_back("<-");
//    InstructionToReturn.push_back(percent + itemToDecode->labelName);
//    InstructionToReturn.push_back(">>");
//    InstructionToReturn.push_back("1");
//
//    return InstructionToReturn;
//}
//
//// doesn't create a new variable and overwrites varToEncode with the encoded value
//std::vector<std::vector<std::string>> encode(Item* varToEncode, std::string labelToAppend, int &counter) {
//    std::vector<std::vector<std::string>> InstructionsToReturn;
//    std::vector<std::string> shiftI;
//    shiftI.push_back("%" + varToEncode->labelName);
//    shiftI.push_back("<-");
//    shiftI.push_back(varToEncode->labelName);
//    shiftI.push_back("<<");
//    shiftI.push_back("1");
//    shiftI.push_back("%" + varToEncode->labelName);
//
//    std::vector<std::string> addI;
//    addI.push_back("%" + varToEncode->labelName);
//    addI.push_back("<-");
//    addI.push_back("%" + varToEncode->labelName);
//    addI.push_back("+");
//    addI.push_back("1");
//
//    InstructionsToReturn.push_back(shiftI);
//    InstructionsToReturn.push_back(addI);
//
//    return InstructionsToReturn;
//}
//
//std::vector<std::vector<std::string>> calculateArrayLength(Instruction* ip, std::string labelToAppend) {
//    std::vector<std::vector<std::string>> iToRet;
//    int varNameCounter = 0;
//
//    for(auto &arg : ip->arguments) {
//        
//
//        
//
//
    /* 
     * %l <- length %ar %dimID
     * %dimID is var or number
     * varNameModifier prepends '%' to a variable name; else returns the Item's name as a string
     */
    std::vector<std::vector<std::string>> length_translation(Instruction* ip, std::string newLabel, int &labelCounter) {
        Item* destination = ip->Items[0];
        Item* arrayName = ip->Items[1];
        Item* dimID = ip->Items[2];
    
        std::vector<std::string> ret_strings;
        std::vector<std::vector<std::string>> ret_vectors;
        std::string offsetVarString = "%" + newLabel + std::to_string(labelCounter);
        labelCounter++;

        // %l <- %ar + 16 (start of dimension sizes)
        ret_strings.insert(ret_strings.end(), {destination->labelName, "<-", arrayName->labelName, "+", "16"});
        ret_vectors.push_back(ret_strings);
        ret_strings.clear();

        // %offset <- 8 * %dimID
        ret_strings.insert(ret_strings.end(), {offsetVarString, "<-", "8", "*", dimID->labelName});
        ret_vectors.push_back(ret_strings);
        ret_strings.clear();

        // %l <- %l + %offset
        ret_strings.insert(ret_strings.end(), {destination->labelName, "<-", destination->labelName, "+", offsetVarString});
        ret_vectors.push_back(ret_strings);
        ret_strings.clear();

        // %l <- load %l
        ret_strings.insert(ret_strings.end(), {destination->labelName, "<-",  "load", destination->labelName});
        ret_vectors.push_back(ret_strings);

        return ret_vectors;
    }

    // can assume ip->Type is InstructionType::assign_new_array
    std::vector<std::vector<std::string>> new_array_translation(Instruction* ip, std::string newLabel, int &varNameCounter) {
        // find the item holding the dimensionSize vector
        std::vector<Item*> allDimensions = ip->arguments;
        std::vector<std::string> ret_strings;
        std::vector<std::vector<std::string>> ret_vectors;

        std::vector<std::string> decodedVarNames;
        // decode each dimension
        for(auto d : allDimensions) {
            std::string newVarName = "%" + newLabel + "_" + std::to_string(varNameCounter);
            ret_strings.insert(ret_strings.end(), {newVarName, "<-", d->labelName, ">>", "1"});
            ret_vectors.push_back(ret_strings);
            ret_strings.clear();

            decodedVarNames.push_back(newVarName); // for later use when multiplying to find arr size
            varNameCounter++;
        }

        // find the size of the array 
        std::string arr_size = "%" + newLabel + "_" + std::to_string(varNameCounter);
        varNameCounter++;

        ret_strings.insert(ret_strings.end(), {arr_size, "<-", decodedVarNames[0]}); // sure to have at least 1 dim
        ret_vectors.push_back(ret_strings); 
        ret_strings.clear();

        // multiply size of each dimension
        for(int d = 1; d < decodedVarNames.size(); ++d) {
            ret_strings.insert(ret_strings.end(), {arr_size, "<-", arr_size, "*", decodedVarNames[d]});         
            ret_vectors.push_back(ret_strings);
            ret_strings.clear();
        }

        // add overhead necessary for # of dim and size of each dim
        int overhead = 1; // # of dim
        overhead += allDimensions.size();

        ret_strings.insert(ret_strings.end(), {arr_size, "<-", arr_size, "+", std::to_string(overhead)});
        ret_vectors.push_back(ret_strings);
        ret_strings.clear();

        // encode final size of array
        ret_strings.insert(ret_strings.end(), {arr_size, "<-", arr_size, "<<", "1"});
        ret_vectors.push_back(ret_strings);
        ret_strings.clear();

        ret_strings.insert(ret_strings.end(), {arr_size, "<-", arr_size, "+", "1"});
        ret_vectors.push_back(ret_strings);
        ret_strings.clear();

        // call allocate
        Item* arr_var = ip->Items[0];
        ret_strings.insert(ret_strings.end(), {arr_var->labelName, "<-", "call", "allocate", "(", arr_size, ",", "1", ")"});
        ret_vectors.push_back(ret_strings);
        ret_strings.clear();

        // store # of dim at arr + 8
        std::string num_dim_loc_in_arr = "%" + newLabel + "_" + std::to_string(varNameCounter);
        varNameCounter++;

        ret_strings.insert(ret_strings.end(), {num_dim_loc_in_arr, "<-", arr_var->labelName, "+", "8"});
        ret_vectors.push_back(ret_strings);
        ret_strings.clear();

        ret_strings.insert(ret_strings.end(), {"store", num_dim_loc_in_arr, "<-", std::to_string(allDimensions.size())});
        ret_vectors.push_back(ret_strings);
        ret_strings.clear();

        // loop through all dimensions and store the encoded size of each dimension into the array
        int mem_loc = 16; // array size @ 0; number of dimensions @ 8
        for(auto& itp : allDimensions) {
            std::string tempVar = "%" + newLabel + "_" + std::to_string(varNameCounter);
            varNameCounter++;

            ret_strings.insert(ret_strings.end(), {tempVar, "<-", arr_var->labelName, "+", std::to_string(mem_loc)});
            ret_vectors.push_back(ret_strings);
            ret_strings.clear();

            ret_strings.insert(ret_strings.end(), {"store", tempVar, "<-", itp->labelName});
            ret_vectors.push_back(ret_strings);
            ret_strings.clear();

            mem_loc += 8;
        }

        return ret_vectors;
    }

    std::vector<std::vector<std::string>> new_tuple(Instruction* ip) {
		std::string destination = ip->Items[0]->labelName;
        std::string arrayLen = ip->arguments[0]->labelName;
    
        std::vector<std::string> ret_strings;
        std::vector<std::vector<std::string>> ret_vectors;
		
		ret_strings.insert(ret_strings.end(), {destination, "<-", "call", "allocate(", arrayLen, ",", "1)"});

		ret_vectors.push_back(ret_strings);

		return ret_vectors;
	}
}
