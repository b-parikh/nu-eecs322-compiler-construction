#include <utils.h>
#include <linearize_arrays.h>

/*
 * Code generation for the following array operations:
 * (1) Accessing length of a dimension
 * (2) Storing into array
 * (3) Loading into array
 * (4) Creating a new Array
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

        // encode # of dim before storing
        std::string num_dim_enc = "%" + newLabel + "_" + std::to_string(varNameCounter);
        varNameCounter++;

        ret_strings.insert(ret_strings.end(), {num_dim_enc, "<-", std::to_string(allDimensions.size()), "<<", "1"});
        ret_vectors.push_back(ret_strings);
        ret_strings.clear();

        ret_strings.insert(ret_strings.end(), {num_dim_enc, "<-", num_dim_enc, "+", "1"});
        ret_vectors.push_back(ret_strings);
        ret_strings.clear();
        
        // store # of dim at arr + 8
        std::string num_dim_loc_in_arr = "%" + newLabel + "_" + std::to_string(varNameCounter);
        varNameCounter++;
        
        ret_strings.insert(ret_strings.end(), {num_dim_loc_in_arr, "<-", arr_var->labelName, "+", "8"});
        ret_vectors.push_back(ret_strings);
        ret_strings.clear();

        ret_strings.insert(ret_strings.end(), {"store", num_dim_loc_in_arr, "<-", num_dim_enc});
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

            ret_strings.insert(ret_strings.end(), {"store", tempVar, "<-", itp->labelName, "\n"});
            ret_vectors.push_back(ret_strings);
            ret_strings.clear();

            mem_loc += 8;
        }

        return ret_vectors;
    }


    std::vector<std::vector<std::string>> array_load_and_store_translation(Instruction* ip, std::string newLabel, int &varNameCounter) {
        std::vector<std::string> ret_strings;
        std::vector<std::vector<std::string>> ret_vectors;
        std::vector<Item*> accessInfo = ip->array_access_location;

        std::string base_offset = "%" + newLabel + "_base_offset_" + std::to_string(varNameCounter);
        varNameCounter++;
        ret_strings.insert(ret_strings.end(), {base_offset, "<-", "16"});
        ret_vectors.push_back(ret_strings);
        ret_strings.clear();

        std::string num_dim_offset = "%" + newLabel + "_num_dim_offset_" + std::to_string(varNameCounter);
        varNameCounter++;
        ret_strings.insert(ret_strings.end(), {num_dim_offset, "<-", std::to_string(accessInfo.size()), "*", "8"});
        ret_vectors.push_back(ret_strings);
        ret_strings.clear();

        // create base offset
        ret_strings.insert(ret_strings.end(), {base_offset, "<-", base_offset, "+", num_dim_offset});
        ret_vectors.push_back(ret_strings);
        ret_strings.clear();

        // secondary offset based on array dimensions, which we can access at runtime
        std::vector<std::string> dimSizes_var_names; // will be same size as accessInfo; stores the var name that holds the size of a dimension (vector order corresponds to 1st -> Nth dimension)

        // loads the dimensions of the array from memory at runtime; also decodes these values and stores them as variables into the dimSize_var_name vector
        std::string arr_name = ip->Items[1]->labelName; 
        for(int i = 0; i < accessInfo.size(); ++i) {
            int mem_loc = i+2; // +2 because location 0 is the size of the array
            // to hold the location of the dim size
            std::string dimSizeOffset = "%" + newLabel + "_dimSizeOffset_" + std::to_string(varNameCounter);
            varNameCounter++;

            std::string dimSize_var_name = "%" + newLabel + "_dimSize_var_name_" + std::to_string(varNameCounter);
            varNameCounter++;
            dimSizes_var_names.push_back(dimSize_var_name);

            ret_strings.insert(ret_strings.end(), {dimSizeOffset, "<-", std::to_string(mem_loc), "*", "8"});
            ret_vectors.push_back(ret_strings);
            ret_strings.clear();

            // the actual location of the current dimension's size
            ret_strings.insert(ret_strings.end(), {dimSizeOffset, "<-", arr_name, "+", dimSizeOffset});
            ret_vectors.push_back(ret_strings);
            ret_strings.clear();

            ret_strings.insert(ret_strings.end(), {dimSize_var_name, "<-", "load", dimSizeOffset});
            ret_vectors.push_back(ret_strings);
            ret_strings.clear();

            // decode the dimension size
            ret_strings.insert(ret_strings.end(), {dimSize_var_name, "<-", dimSize_var_name,  ">>", "1"});
            ret_vectors.push_back(ret_strings);
            ret_strings.clear();
        }

        // now that all the dimensions have been found, stored, and decoded, begin calculation of the extra offset
        std::string offset = "%" + newLabel + "_offset_" + std::to_string(varNameCounter); 
        ret_strings.insert(ret_strings.end(), {offset, "<-", accessInfo.back()->labelName}); // start counting offset at the last access index
        ret_vectors.push_back(ret_strings);
        ret_strings.clear();
        
        varNameCounter++;
        for(int i = 0; i < accessInfo.size() - 1; ++i) { // skip the last access as we initialize index to this anyways
            std::string size_of_dims = "%" + newLabel + "_size_of_dims_" + std::to_string(varNameCounter); // during runtime, holds the value of M x N, or L x M x N, or even N
            varNameCounter++;
            ret_strings.insert(ret_strings.end(), {size_of_dims, "<-", accessInfo[i]->labelName}); // start the size_of_dims multiplication at the access number (eg. i, j, k, etc.)
            ret_vectors.push_back(ret_strings);
            ret_strings.clear();
            
            for(int j = i + 1; j < dimSizes_var_names.size(); ++j) { // for each dimension's variable name (L, M, N, etc.) 
                ret_strings.insert(ret_strings.end(), {size_of_dims, "<-", size_of_dims, "*", dimSizes_var_names[j]});
                ret_vectors.push_back(ret_strings);
                ret_strings.clear();
            }

            ret_strings.insert(ret_strings.end(), {offset, "<-", offset, "+", size_of_dims});
            ret_vectors.push_back(ret_strings);
            ret_strings.clear();
        }

        // offset multiply by 8 
        ret_strings.insert(ret_strings.end(), {offset, "<-", offset, "*", "8"});
        ret_vectors.push_back(ret_strings);
        ret_strings.clear();

        std::string addr = "%" + newLabel + "_addr_" + std::to_string(varNameCounter);
        varNameCounter++;
        ret_strings.insert(ret_strings.end(), {addr, "<-", offset, "+", base_offset});
        ret_vectors.push_back(ret_strings);
        ret_strings.clear();
        
        if(ip->Type == InstructionType::assign_store_array) {
            ret_strings.insert(ret_strings.end(), {"store", addr, "<-", ip->Items.back()->labelName});
            ret_vectors.push_back(ret_strings);
            ret_strings.clear();
        }
        else{ // load 
            ret_strings.insert(ret_strings.end(), {ip->Items[0]->labelName, "<-", "load", addr});
            ret_vectors.push_back(ret_strings);
            ret_strings.clear();
        }
        
        return ret_vectors;
	}


//    std::vector<std::vector<std::string>> array_store_translation(Instruction* ip, std::string newLabel, int &varNameCounter) {
//        std::vector<std::string> ret_strings;
//        std::vector<std::vector<std::string>> ret_vectors;
//
//
//
//        return ret_vectors;
//	}


	// Initialization of a tuple
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
