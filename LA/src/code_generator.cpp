#include <string>
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <code_generator.h>
#include <stdlib.h>
#include <algorithm>

using namespace std;

namespace LA{

    // Find the longest label that will be used to create a label for a basic block
	std::string get_longest_label(Function* fp) {
	    std::string longest_label;
	    int longest_label_len = 0;
	    for(auto &instruct : fp->instructions) {
               for(auto &item : instruct->Items) {
   		        if(item->itemType == Atomic_Type::label) {
   		  	        if(item->labelName.length() > longest_label_len) {
   			            longest_label = item->labelName;
   			            longest_label_len = item->labelName.length();
   		            }
   		        }
	        }
        }

		// no label found
		if(longest_label_len == 0)
			return ":no_label_available_" + fp->name;
		else {
			// remove : sign
			if(longest_label.at(0) == ':')
				longest_label.erase(0,1);

			return ":" + longest_label + '_' + fp->name;
		}
    }

	std::string get_longest_variable(Function* fp) {
		std::vector<Item*> declared;
		declared.insert(declared.end(), fp->arguments.begin(), fp->arguments.end());

	    std::string longest_variable;
	    int longest_variable_len = 0;
	    for(auto &instruct : fp->instructions) {
               for(auto &item : instruct->Items) {
   		        if(item->itemType == Atomic_Type::var) {
					if(instruct->Type == InstructionType::init_var)
						declared.push_back(item);
					else {
						for(auto &declared_item : declared) {
							if(item->labelName == declared_item->labelName)
								item->varType = declared_item->varType;
						}
					}

   		  	        if(item->labelName.length() > longest_variable_len) {
   			            longest_variable = item->labelName;
   			            longest_variable_len = item->labelName.length();
   		            }
   		        }
	        }
        }
        if(longest_variable_len == 0)
            return "new_var";
        else
            return longest_variable;
    }

	// return ir label if it is a label, otherwise return ir var
	std::string to_ir_item_label(Item* laItem, Instruction* ip=nullptr) {
		if(laItem->itemType == Atomic_Type::label) {
			if(ip ==nullptr || ip->calleeType == CalleeType::label)
				return ':' + laItem->labelName;
			else
				return laItem->labelName;
		} else if(laItem->itemType == Atomic_Type::var)
			return '%' + laItem->labelName;
		else
			return laItem->labelName;
	}

	std::string get_arg_type(Item* item) {
		VarType argType = item->varType;
		if(argType == VarType::int64_type)
			return "int64";
		else if(argType == VarType::arr_type) {
			std::string brackets;
			for(int i=0; i<item->numDimensions; i++)
				brackets += "[]";
			//brackets = "int64" + brackets;
			return "int64" + brackets;
		} else if(argType == VarType::tuple_type)
			return "tuple";
		else // code_type
			return "code";
	}

	std::string get_return_type(Function* fp) {
		VarType returnType = fp->returnType;
		if(returnType == VarType::int64_type)
			return "int64";
		else if(returnType == VarType::arr_type) {
			std::string brackets;
			for(int i=0; i<fp->numDimensions; i++)
				brackets += "[]";
			brackets = "int64" + brackets;
			return brackets;
		} else if(returnType == VarType::tuple_type)
			return "tuple";
		else if(returnType == VarType::code_type)
			return "code";
		else // void_type
			return "void";
	}

	// Generate blocks for IR
	Function* generate_blocks(Function* fp, std::string longestLabel) {
		Function* newF = new Function();
		bool firstInst = true;
		int labelCounter = 0;
		for(auto &ip : fp->instructions) {
			if(firstInst) {
				// Insert the initial label for a block
				if(ip->Type != InstructionType::label) {
					Instruction* newI = new Instruction();
					newI->Type = InstructionType::label;

					Item* newItem = new Item();
					newItem->labelName = longestLabel + std::to_string(labelCounter);
					labelCounter++;
					newItem->itemType = Atomic_Type::label;

					newI->Items.push_back(newItem);

					newF->instructions.push_back(newI);
				}

				firstInst = false;
			} else if(ip->Type == InstructionType::label) {
				Instruction* newI = new Instruction();
				newI->Type = InstructionType::br_unconditional;

				Item* newItem = new Item();
				newItem->labelName = ip->Items.back()->labelName;
				newItem->itemType = Atomic_Type::label;

				newI->Items.push_back(newItem);

				newF->instructions.push_back(newI);
			}

			// Insert all original instructions
			newF->instructions.push_back(ip);
				
			if (ip->Type == InstructionType::br_unconditional || ip->Type == InstructionType::br_conditional ||
				ip->Type == InstructionType::return_empty || ip->Type == InstructionType::return_value) {
				firstInst = true;
			}
		}

		return newF;
	}

    std::vector<std::vector<string>> convert_instruction(Instruction* ip, std::string newVarLabel, int& varNameCounter, std::string newLabel, int& labelNameCounter) {
        std::vector<std::vector<string>> ret_vectors;
		std::vector<string> ret_strings;
        std::vector<Item*> to_encode;
        std::vector<Item*> to_decode;

        newLabel.erase(0,1); // to remove the ":" at the start

        if(ip->Type == InstructionType::assign) { 
            // encode RHS before putting into a variable (if RHS is a number)
            to_encode = assign_encode(ip);
            ret_vectors = encode_all_items(ip, to_encode, newVarLabel, varNameCounter);
            
			ret_strings.push_back(to_ir_item_label(ip->Items[0], ip));
			ret_strings.push_back("<-");
			ret_strings.push_back(to_ir_item_label(ip->Items[1], ip));

            ret_vectors.push_back(ret_strings);
        } else if(ip->Type == InstructionType::assign_arithmetic) { //as it is
            // decode t in t op t if t is a variable
            to_decode = assign_to_decode(ip);
            ret_vectors = decode_all_items(ip, to_decode, newVarLabel, varNameCounter);

			ret_strings.push_back(to_ir_item_label(ip->Items[0], ip));
			ret_strings.push_back("<-");

			//ret_strings.push_back(to_ir_item_label(ip->Items[1], ip));
			ret_strings.push_back(ip->Items[1]->labelName);

			if(ip->Arith_Oper == Arith_Operator::shift_left) {
			  ret_strings.push_back("<<");
			} else if(ip->Arith_Oper == Arith_Operator::shift_right) {
			  ret_strings.push_back(">>");
			} else if(ip->Arith_Oper == Arith_Operator::plus) {
			  ret_strings.push_back("+");
			} else if(ip->Arith_Oper == Arith_Operator::minus) {
			  ret_strings.push_back("-");
			} else if(ip->Arith_Oper == Arith_Operator::multiply) {
			  ret_strings.push_back("*");
			} else if(ip->Arith_Oper == Arith_Operator::bw_and) {
			  ret_strings.push_back("&");
			} else {
			  std::cerr << "Incorrect Arith Operator\n";
			}

			//ret_strings.push_back(to_ir_item_label(ip->Items[2], ip));
			ret_strings.push_back(ip->Items[2]->labelName);
            ret_vectors.push_back(ret_strings);
            ret_strings.clear();

            to_encode = assign_to_encode(ip);
            std::vector<std::vector<std::string>> encoded_instructions = encode_all_items(ip, to_encode, newVarLabel, varNameCounter);
	        ret_vectors.insert(ret_vectors.end(), encoded_instructions.begin(), encoded_instructions.end());

		} else if(ip->Type == InstructionType::assign_compare) { //as it is
            // decode t in t op t if t is a variable
            to_decode = assign_to_decode(ip);
            ret_vectors = decode_all_items(ip, to_decode, newVarLabel, varNameCounter);

			ret_strings.push_back(to_ir_item_label(ip->Items[0], ip));
			ret_strings.push_back("<-");

			//ret_strings.push_back(to_ir_item_label(ip->Items[1], ip));
			ret_strings.push_back(ip->Items[1]->labelName);

			if(ip->Comp_Oper == Compare_Operator::gr) {
			  ret_strings.push_back(">");
			} else if(ip->Comp_Oper == Compare_Operator::geq) {
			  ret_strings.push_back(">=");
			} else if(ip->Comp_Oper == Compare_Operator::le) {
			  ret_strings.push_back("<");
			} else if(ip->Comp_Oper == Compare_Operator::leq) {
			  ret_strings.push_back("<=");
			} else if(ip->Comp_Oper == Compare_Operator::eq) {
			  ret_strings.push_back("=");
			} else {
			  std::cerr << "Incorrect Compare Operator\n";
			}

			//ret_strings.push_back(to_ir_item_label(ip->Items[2], ip));
			ret_strings.push_back(ip->Items[2]->labelName);
            ret_vectors.push_back(ret_strings);
            
            to_encode = assign_to_encode(ip);
            std::vector<std::vector<std::string>> encoded_instructions = encode_all_items(ip, to_encode, newVarLabel, varNameCounter);
	        ret_vectors.insert(ret_vectors.end(), encoded_instructions.begin(), encoded_instructions.end());

		} else if(ip->Type == InstructionType::assign_load_array) {
            // check if allocated previously
            ret_vectors = check_array_allocation(ip, newVarLabel, varNameCounter, newLabel, labelNameCounter);

			if(ip->Items[0]->varType == VarType::arr_type) {
                std::vector<std::vector<std::string>> arr_access_temp = check_array_access(ip, newVarLabel, varNameCounter, newLabel, labelNameCounter);
                ret_vectors.insert(ret_vectors.end(), arr_access_temp.begin(), arr_access_temp.end());
            }

//			if(ip->Items[1]->varType == VarType::arr_type)
//				ret_vectors = check_array_access(ip, newVarLabel, varNameCounter, newLabel, labelNameCounter);

            // if load operation has variables that are arguments, decode them
            to_decode = assign_load_array_decode(ip);
            std::vector<std::vector<std::string>> decoded_ret_vectors = decode_all_items(ip, to_decode, newVarLabel, varNameCounter);
            ret_vectors.insert(ret_vectors.end(), decoded_ret_vectors.begin(), decoded_ret_vectors.end());


			ret_strings.push_back(to_ir_item_label(ip->Items[0], ip));
    		ret_strings.push_back("<-");
			ret_strings.push_back(to_ir_item_label(ip->Items[1], ip));			
			int numDimensions = ip->array_access_location.size();
			for(int i=0; i<numDimensions; i++) {
	    		ret_strings.push_back("[");
				ret_strings.push_back(to_ir_item_label(ip->array_access_location[i], ip));
				ret_strings.push_back("]");
			}

            ret_vectors.push_back(ret_strings);
		} else if(ip->Type == InstructionType::assign_store_array) {
			//std::cerr << int(ip->Items[0]->varType) << "\n";
            // check if allocated previously
            ret_vectors = check_array_allocation(ip, newVarLabel, varNameCounter, newLabel, labelNameCounter);

            // don't check if tuple
			if(ip->Items[0]->varType == VarType::arr_type) {
                std::vector<std::vector<std::string>> arr_access_temp = check_array_access(ip, newVarLabel, varNameCounter, newLabel, labelNameCounter);
                ret_vectors.insert(ret_vectors.end(), arr_access_temp.begin(), arr_access_temp.end());
            }

            to_encode = assign_store_array_encode(ip);
            std::vector<std::vector<std::string>> encoded_ret_vectors = encode_all_items(ip, to_encode, newVarLabel, varNameCounter);
            ret_vectors.insert(ret_vectors.end(), encoded_ret_vectors.begin(), encoded_ret_vectors.end());

            to_decode = assign_store_array_decode(ip);
            std::vector<std::vector<std::string>> decoded_ret_vectors = decode_all_items(ip, to_decode, newVarLabel, varNameCounter);
            ret_vectors.insert(ret_vectors.end(), decoded_ret_vectors.begin(), decoded_ret_vectors.end());

    		ret_strings.push_back(to_ir_item_label(ip->Items[0], ip));
			int numDimensions = ip->array_access_location.size();
			for(int i=0; i<numDimensions; i++) {
	    		ret_strings.push_back("[");
				ret_strings.push_back(to_ir_item_label(ip->array_access_location[i], ip));
				ret_strings.push_back("]");
			}
    		ret_strings.push_back("<-");
			ret_strings.push_back(to_ir_item_label(ip->Items.back(), ip));			

            ret_vectors.push_back(ret_strings);
		} else if(ip->Type == InstructionType::assign_new_array) {
            // encode arguments to new Array that may be constants
            to_encode = assign_new_array_encode(ip);
            ret_vectors = encode_all_items(ip, to_encode, newVarLabel, varNameCounter);

			ret_strings.push_back(to_ir_item_label(ip->Items[0], ip));
    		ret_strings.push_back("<-");
    		ret_strings.push_back("new");
    		ret_strings.push_back("Array");
    		ret_strings.push_back("(");
			for(int i=0; i<ip->arguments.size()-1; i++) {
				ret_strings.push_back(to_ir_item_label(ip->arguments[i], ip));
				ret_strings.push_back(",");
			}
			ret_strings.push_back(to_ir_item_label(ip->arguments.back(), ip));

    		ret_strings.push_back(")");

            ret_vectors.push_back(ret_strings);
		} else if(ip->Type == InstructionType::assign_new_tuple) {
            // encode arguments to new Tuple that may be constants
            to_encode = assign_new_array_encode(ip);
            ret_vectors = encode_all_items(ip, to_encode, newVarLabel, varNameCounter);

			ret_strings.push_back(to_ir_item_label(ip->Items[0], ip));
    		ret_strings.push_back("<-");
    		ret_strings.push_back("new");
    		ret_strings.push_back("Tuple");
    		ret_strings.push_back("(");
			ret_strings.push_back(to_ir_item_label(ip->arguments.back(), ip));
    		ret_strings.push_back(")");

            ret_vectors.push_back(ret_strings);
		} else if(ip->Type == InstructionType::assign_length) {
            // decode 3rd argument if it's a variable
            to_decode = array_length_decode(ip);
            ret_vectors = decode_all_items(ip, to_decode, newVarLabel, varNameCounter);

			ret_strings.push_back(to_ir_item_label(ip->Items[0], ip));
    		ret_strings.push_back("<-");
    		ret_strings.push_back("length");
			ret_strings.push_back(to_ir_item_label(ip->Items[1], ip));
			ret_strings.push_back(to_ir_item_label(ip->Items[2], ip));

            ret_vectors.push_back(ret_strings);
		} else if(ip->Type == InstructionType::call) { 
            to_encode = call_encode(ip);
            ret_vectors = encode_all_items(ip, to_encode, newVarLabel, varNameCounter);

			ret_strings.push_back("call");

			ret_strings.push_back(to_ir_item_label(ip->Items[0], ip));

			ret_strings.push_back("(");

    		int arg_size = ip->arguments.size();
    		if (arg_size > 0) {
    		    for(int i = 0; i < arg_size - 1; i++)
    		        ret_strings.push_back(to_ir_item_label(ip->arguments[i], ip) + ",");
    		  ret_strings.push_back(to_ir_item_label(ip->arguments[arg_size - 1], ip));
    		}
  
			ret_strings.push_back(")");

            ret_vectors.push_back(ret_strings);
		} else if(ip->Type == InstructionType::call_assign) { // as it is 
            to_encode = call_encode(ip);
            ret_vectors = encode_all_items(ip, to_encode, newVarLabel, varNameCounter);

			ret_strings.push_back(to_ir_item_label(ip->Items[0], ip));
			ret_strings.push_back("<-");
			ret_strings.push_back("call");

			ret_strings.push_back(to_ir_item_label(ip->Items[1], ip));

			ret_strings.push_back("(");

    		int arg_size = ip->arguments.size();
    		if (arg_size > 0) {
    		  for(int i = 0; i < arg_size - 1; i++)
    			ret_strings.push_back(to_ir_item_label(ip->arguments[i], ip) + ",");
    		  ret_strings.push_back(to_ir_item_label(ip->arguments[arg_size - 1], ip));
    		}
  
			ret_strings.push_back(")");

            ret_vectors.push_back(ret_strings);

		} else if(ip->Type == InstructionType::label) { // as it is
			ret_strings.push_back(ip->Items[0]->labelName);

            ret_vectors.push_back(ret_strings);
		} else if(ip->Type == InstructionType::return_empty) { // as it is
			ret_strings.push_back("return");

            ret_vectors.push_back(ret_strings);
		} else if(ip->Type == InstructionType::return_value) { // as it is
            to_encode = return_value_encode(ip);
            // ret_vectors will contain an instruction ONLY if the return value is a number; otherwise the return value will be encoded in previous assign instruction
            ret_vectors = encode_items(ip, to_encode, newVarLabel, varNameCounter);
			ret_strings.push_back("return");
			ret_strings.push_back(to_ir_item_label(ip->Items[0], ip));

            ret_vectors.push_back(ret_strings);
		} else if(ip->Type == InstructionType::br_unconditional) { // as it is
			ret_strings.push_back("br");
			ret_strings.push_back(ip->Items[0]->labelName);

            ret_vectors.push_back(ret_strings);
		} else if(ip->Type == InstructionType::br_conditional) { // explicit jump
			ret_strings.push_back("br");
            std::vector<Item*> items_to_decode = br_conditional_decode(ip);
            // decode the argument Item 
            ret_vectors = decode_all_items(ip, items_to_decode, newVarLabel, varNameCounter);

			ret_strings.push_back(ip->Items[0]->labelName);
			//ret_strings.push_back(to_ir_item_label(ip->Items[0], ip));
			ret_strings.push_back(ip->Items[1]->labelName);
			ret_strings.push_back(ip->Items[2]->labelName);

            ret_vectors.push_back(ret_strings);
            ret_strings.clear();
 		} else if(ip->Type == InstructionType::print) {
            to_encode = print_encode(ip);
            ret_vectors = encode_all_items(ip, to_encode, newVarLabel, varNameCounter);

			ret_strings.push_back("call");
			ret_strings.push_back("print");
			ret_strings.push_back("(");
            //ret_strings.push_back(ip->Items[0]->labelName);
			ret_strings.push_back(to_ir_item_label(ip->Items[0], ip));
			ret_strings.push_back(")");

            ret_vectors.push_back(ret_strings);
       } else { //init_var
			ret_strings.push_back(get_arg_type(ip->Items[0]));
			ret_strings.push_back(to_ir_item_label(ip->Items[0], ip));

			ret_vectors.push_back(ret_strings);
            // set all arrays/tuples to 0
            std::vector<std::vector<std::string>> init_instructions = initialize_arrays_and_tuples(ip, newVarLabel, varNameCounter, newLabel, labelNameCounter);
            ret_vectors.insert(ret_vectors.end(), init_instructions.begin(), init_instructions.end());
        }

		return ret_vectors;
    } 

    void generate_code(Program p){
      /* 
       * Open the output file.
      */
      std::ofstream outputFile;
      outputFile.open("prog.IR");

      /* 
       * Generate IR code
       */ 
      for(auto& fp: p.functions) {
		  // create an item for the function name for the conversion
		  Item* func_name = new Item();
		  func_name->labelName = fp->name;
		  func_name->itemType = Atomic_Type::label;

		  // PRINT initial line of the function
          outputFile << "define " << get_return_type(fp) << " " << to_ir_item_label(func_name) << " (";
		  int arg_size = fp->arguments.size();
		  if (arg_size > 0) {
		    for(int i = 0; i < arg_size - 1; i++)
			  outputFile << get_arg_type(fp->arguments[i]) << ' ' << to_ir_item_label(fp->arguments[i]) << ", ";
		    outputFile << get_arg_type(fp->arguments[arg_size - 1]) << ' ' << to_ir_item_label(fp->arguments[arg_size - 1]);
		  }
		  outputFile << ") {\n";

          // to generate new labels and variables
		  std::string longest_label = get_longest_label(fp);
		  std::string longest_variable = get_longest_variable(fp);
          int varNameCounter = 0;
          int labelNameCounter = 0;
          
		  Function* blockedF = generate_blocks(fp, longest_label);

          for(auto &ip : blockedF->instructions) {
			//std::cerr<< int(ip->Type) <<'\n';
			std::vector<std::vector<string>> to_print = convert_instruction(ip, longest_variable, varNameCounter, longest_label, labelNameCounter);
			if (!to_print.empty()) { //init_var does not print anything for L3
			  for(auto vec_str : to_print) {
                outputFile << "\t";
                for(auto str : vec_str)
                  outputFile << str << ' ';
                outputFile << '\n';
              }
		    }
		  }
          outputFile << "}\n\n";
      }

      /* 
       * Close the output file.
       */ 
      outputFile.close();
      return;
    }
}
