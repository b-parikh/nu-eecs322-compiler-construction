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

	std::string get_oper_str(Oper oper_enum) {
		std::string oper_str;

		if(oper_enum == Oper::shift_left) {
		  oper_str = "<<";
		} else if(oper_enum == Oper::shift_right) {
		  oper_str = ">>";
		} else if(oper_enum == Oper::plus) {
		  oper_str = "+";
		} else if(oper_enum == Oper::minus) {
		  oper_str = "-";
		} else if(oper_enum == Oper::multiply) {
		  oper_str = "*";
		} else if(oper_enum == Oper::bw_and) {
		  oper_str = "&";
		} else if(oper_enum == Oper::gr) {
		  oper_str = ">";
		} else if(oper_enum == Oper::geq) {
		  oper_str = ">=";
		} else if(oper_enum == Oper::le) {
		  oper_str = "<";
		} else if(oper_enum == Oper::leq) {
		  oper_str = "<=";
		} else if(oper_enum == Oper::eq) {
		  oper_str = "=";
		} else {
		  oper_str = "";
		  std::cerr << "Incorrect Arith Operator\n";
		}

		return oper_str;
	}

    std::vector<std::vector<string>> convert_instruction(Scope* currS, Instruction* ip, std::string newVarLabel, int& varNameCounter, std::string newLabel, int& labelNameCounter, std::ofstream& outputFile) {
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

			ret_strings.push_back(get_oper_str(ip->Operator));

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
            DEBUG_LOG("return empty called from code generator");
            //std::cerr << "return empty called from code generator\n";
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
			ret_strings.push_back("int64");
			std::string cond = newVarLabel + std::to_string(varNameCounter);
			varNameCounter++;
			ret_strings.push_back(cond);
			ret_vectors.push_back(ret_strings);
			ret_strings.clear();
			
			ret_strings.push_back(cond);
			ret_strings.push_back("<-");
			ret_strings.push_back(ip->Items[0]->labelName);
			ret_strings.push_back(get_oper_str(ip->Operator));
			ret_strings.push_back(ip->Items[1]->labelName);
            ret_vectors.push_back(ret_strings);
			ret_strings.clear();

			ret_strings.push_back("br");
            ret_strings.push_back(cond);
            ret_strings.push_back(ip->Items[2]->labelName);
            ret_strings.push_back(ip->Items[3]->labelName);
            ret_vectors.push_back(ret_strings);

		} else if(ip->Type == InstructionType::init_var) {
			std::string var_type = get_arg_type(ip->Items[0]);

			for(auto &var : ip->Items) {
				ret_strings.push_back(var_type);
				ret_strings.push_back(var->labelName);
				ret_vectors.push_back(ret_strings);
				ret_strings.clear();
			}

		} else if(ip->Type == InstructionType::while_instruction) {
			//TODO

		} else if(ip->Type == InstructionType::continue_instruction) {
			//TODO

		} else if(ip->Type == InstructionType::break_instruction) {
			//TODO

 		} else if(ip->Type == InstructionType::print) {
		    ret_strings.push_back("print");
			ret_strings.push_back("(");
			ret_strings.push_back(ip->Items[0]->labelName);
			ret_strings.push_back(")");

            ret_vectors.push_back(ret_strings);

	   } else if(ip->Type == InstructionType::scope_end) {
           DEBUG_LOG("scope_end in code_generator");
			//TODO

       } else { // scope_begin
           DEBUG_LOG("scope_begin in code_generator");
           // go to the child scope designated by child_scope_generated
           currS = currS->children_scopes[currS->child_scopes_generated];

           for(auto& ip : currS->Instructions) {
               std::vector<std::vector<std::string>> to_print = 
                   convert_instruction(currS, ip, newVarLabel, varNameCounter, newLabel, labelNameCounter, outputFile);
               
   			   if (!to_print.empty()) { //init_var does not print anything for L3
   			       for(auto vec_str : to_print) {
                       outputFile << "\t";
                       for(auto str : vec_str)
                           outputFile << str << ' ';
                       outputFile << '\n';
                   }
   		       }
           }
           
           currS = currS->parent_scope; // go back to parent scope 
           currS->child_scopes_generated++;

           ret_vectors.clear();
       }

	   return ret_vectors;
    } 

    void generate_code(Program p){
      /* 
       * Open the output file.
      */

      DEBUG_LOG("Generating code");
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

          // to generate new labels and variables
		  std::string longest_label = get_longest_label(fp);
		  std::string longest_variable = get_longest_variable(fp);
          int varNameCounter = 0;
          int labelNameCounter = 0;

          // to flatten scopes 
          
		  // PRINT initial line of the function
          outputFile << get_return_type(fp) << " " << func_name->labelName << " (";
		  int arg_size = fp->arguments.size();
		  if (arg_size > 0) {
		    for(int i = 0; i < arg_size - 1; i++)
			  outputFile << get_arg_type(fp->arguments[i]) << ' ' << fp->arguments[i]->labelName << ", ";
		    outputFile << get_arg_type(fp->arguments[arg_size - 1]) << ' ' << fp->arguments[arg_size - 1]->labelName;
		  }
		  outputFile << ") {\n";

          // current scope is used when scope_begin is encountered in convert_instruction
          auto currS = fp->func_scope;
          for(auto &ip : currS->Instructions) {
			//std::cerr<< int(ip->Type) <<'\n';
			std::vector<std::vector<string>> to_print = convert_instruction(currS, ip, longest_variable, varNameCounter, longest_label, labelNameCounter, outputFile);
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
