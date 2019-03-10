#include <string>
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <code_generator.h>
#include <stdlib.h>
#include <algorithm>

using namespace std;

namespace LB{

    // Find the longest label that will be used to create a label for a basic block
	std::string get_longest_label(Function* fp) {
	    std::string longest_label;
	    int longest_label_len = 0;
	    for(auto &instruct : fp->func_scope->Instructions) {
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
	    for(auto &instruct : fp->func_scope->Instructions) {
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

    std::vector<std::vector<string>> convert_instruction(Instruction* ip, std::string newVarLabel, int& varNameCounter, std::string newLabel, int& labelNameCounter) {
        std::vector<std::vector<string>> ret_vectors;
		std::vector<string> ret_strings;

        if(ip->Type == InstructionType::assign) { 
			ret_strings.push_back(ip->Items[0]->labelName);
			ret_strings.push_back("<-");
			ret_strings.push_back(ip->Items[1]->labelName);

            ret_vectors.push_back(ret_strings);
        } else if(ip->Type == InstructionType::assign_operation) { //as it is
			ret_strings.push_back(ip->Items[0]->labelName);
			ret_strings.push_back("<-");

			ret_strings.push_back(ip->Items[1]->labelName);

			if(ip->Operator == Oper::shift_left) {
			  ret_strings.push_back("<<");
			} else if(ip->Operator == Oper::shift_right) {
			  ret_strings.push_back(">>");
			} else if(ip->Operator == Oper::plus) {
			  ret_strings.push_back("+");
			} else if(ip->Operator == Oper::minus) {
			  ret_strings.push_back("-");
			} else if(ip->Operator == Oper::multiply) {
			  ret_strings.push_back("*");
			} else if(ip->Operator == Oper::bw_and) {
			  ret_strings.push_back("&");
			} else if(ip->Operator == Oper::gr) {
			  ret_strings.push_back(">");
			} else if(ip->Operator == Oper::geq) {
			  ret_strings.push_back(">=");
			} else if(ip->Operator == Oper::le) {
			  ret_strings.push_back("<");
			} else if(ip->Operator == Oper::leq) {
			  ret_strings.push_back("<=");
			} else if(ip->Operator == Oper::eq) {
			  ret_strings.push_back("=");
			} else {
			  std::cerr << "Incorrect Arith Operator\n";
			}

			ret_strings.push_back(ip->Items[2]->labelName);
            ret_vectors.push_back(ret_strings);
		} else if(ip->Type == InstructionType::assign_load_array) {
			ret_strings.push_back(ip->Items[0]->labelName);
    		ret_strings.push_back("<-");
			ret_strings.push_back(ip->Items[1]->labelName);			
			int numDimensions = ip->array_access_location.size();
			for(int i = 0; i < numDimensions; ++i) {
	    		ret_strings.push_back("[");
                ret_strings.push_back(ip->array_access_location[i]->labelName);
				ret_strings.push_back("]");
			}

            ret_vectors.push_back(ret_strings);
		} else if(ip->Type == InstructionType::assign_store_array) {
    		ret_strings.push_back(ip->Items[0]->labelName);
			int numDimensions = ip->array_access_location.size();
			for(int i=0; i<numDimensions; i++) {
	    		ret_strings.push_back("[");
                ret_strings.push_back(ip->array_access_location[i]->labelName);
				ret_strings.push_back("]");
			}
    		ret_strings.push_back("<-");
			ret_strings.push_back(ip->Items.back()->labelName);

            ret_vectors.push_back(ret_strings);
		} else if(ip->Type == InstructionType::assign_new_array) {
			ret_strings.push_back(ip->Items[0]->labelName);
    		ret_strings.push_back("<-");
    		ret_strings.push_back("new");
    		ret_strings.push_back("Array");
    		ret_strings.push_back("(");
			for(int i=0; i<ip->arguments.size()-1; i++) {
				ret_strings.push_back(ip->arguments[i]->labelName);
				ret_strings.push_back(",");
			}
			ret_strings.push_back(ip->arguments.back()->labelName);

    		ret_strings.push_back(")");

            ret_vectors.push_back(ret_strings);
		} else if(ip->Type == InstructionType::assign_new_tuple) {
			ret_strings.push_back(ip->Items[0]->labelName);
    		ret_strings.push_back("<-");
    		ret_strings.push_back("new");
    		ret_strings.push_back("Tuple");
    		ret_strings.push_back("(");
			ret_strings.push_back(ip->arguments.back()->labelName);
    		ret_strings.push_back(")");

            ret_vectors.push_back(ret_strings);
		} else if(ip->Type == InstructionType::assign_length) {
			ret_strings.push_back(ip->Items[0]->labelName);
    		ret_strings.push_back("<-");
    		ret_strings.push_back("length");
			ret_strings.push_back(ip->Items[1]->labelName);
			ret_strings.push_back(ip->Items[2]->labelName);

            ret_vectors.push_back(ret_strings);
		} else if(ip->Type == InstructionType::call) { 
			ret_strings.push_back(ip->Items[0]->labelName);

			ret_strings.push_back("(");

    		int arg_size = ip->arguments.size();
    		if (arg_size > 0) {
    		    for(int i = 0; i < arg_size - 1; i++)
    		        ret_strings.push_back(ip->arguments[i]->labelName + ",");
    		  ret_strings.push_back(ip->arguments[arg_size - 1]->labelName);
    		}
  
			ret_strings.push_back(")");

            ret_vectors.push_back(ret_strings);
		} else if(ip->Type == InstructionType::call_assign) { // as it is 
			ret_strings.push_back(ip->Items[0]->labelName);
			ret_strings.push_back("<-");
			ret_strings.push_back(ip->Items[1]->labelName);

			ret_strings.push_back("(");

    		int arg_size = ip->arguments.size();
    		if (arg_size > 0) {
    		  for(int i = 0; i < arg_size - 1; i++)
    			ret_strings.push_back(ip->arguments[i]->labelName + ",");
    		  ret_strings.push_back(ip->arguments[arg_size - 1]->labelName);
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
			ret_strings.push_back("return");
			ret_strings.push_back(ip->Items[0]->labelName);

            ret_vectors.push_back(ret_strings);
		} else if(ip->Type == InstructionType::br_unconditional) { // as it is
			ret_strings.push_back("br");
			ret_strings.push_back(ip->Items[0]->labelName);

            ret_vectors.push_back(ret_strings);
		} else if(ip->Type == InstructionType::if_instruction) {
			//TODO

		} else if(ip->Type == InstructionType::while_instruction) {
			//TODO

		} else if(ip->Type == InstructionType::continue_instruction) {
			//TODO

		} else if(ip->Type == InstructionType::break_instruction) {
			//TODO

		} else if(ip->Type == InstructionType::scope_begin) {
			//TODO

//		} else if(ip->Type == InstructionType::scope_end) {
			//TODO

 		} else if(ip->Type == InstructionType::print) {
			ret_strings.push_back("print");
			ret_strings.push_back("(");
			ret_strings.push_back(ip->Items[0]->labelName);
			ret_strings.push_back(")");

            ret_vectors.push_back(ret_strings);
       } else { //init_var
			//TODO
			ret_strings.push_back(get_arg_type(ip->Items[0]));
			ret_strings.push_back(ip->Items[0]->labelName);

			ret_vectors.push_back(ret_strings);
            // set all arrays/tuples to 0
            //std::vector<std::vector<std::string>> init_instructions = initialize_arrays_and_tuples(ip, newVarLabel, varNameCounter, newLabel, labelNameCounter);
            //ret_vectors.insert(ret_vectors.end(), init_instructions.begin(), init_instructions.end());
        }

		return ret_vectors;
    } 

    void generate_code(Program p){
      /* 
       * Open the output file.
      */
      std::ofstream outputFile;
      outputFile.open("prog.a");

      /* 
       * Generate IR code
       */ 
      for(auto& fp: p.functions) {
		  // create an item for the function name for the conversion
		  //std::cerr<<fp->name << '\n';
		  Item* func_name = new Item();
		  func_name->labelName = fp->name;
		  func_name->itemType = Atomic_Type::label;

		  // PRINT initial line of the function
          outputFile << get_return_type(fp) << " " << func_name->labelName << " (";
		  int arg_size = fp->arguments.size();
		  if (arg_size > 0) {
		    for(int i = 0; i < arg_size - 1; i++)
			  outputFile << get_arg_type(fp->arguments[i]) << ' ' << fp->arguments[i]->labelName << ", ";
		    outputFile << get_arg_type(fp->arguments[arg_size - 1]) << ' ' << fp->arguments[arg_size - 1]->labelName;
		  }
		  outputFile << ") {\n";

          // to generate new labels and variables
		  std::string longest_label = get_longest_label(fp);
		  std::string longest_variable = get_longest_variable(fp);
          int varNameCounter = 0;
          int labelNameCounter = 0;
          
          for(auto &ip : fp->func_scope->Instructions) {
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
          outputFile << "}\n";
      }

      /* 
       * Close the output file.
       */ 
      outputFile.close();
      return;
    }
}
