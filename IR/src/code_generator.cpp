#include <string>
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <code_generator.h>
#include <stdlib.h>
#include <algorithm>

using namespace std;

namespace IR{
	std::string get_longest_varname(Function* fp) {
	  std::string longest_var;
	  int longest_var_len = 0;
	  for(auto &ip : fp->instructions) {
		for(auto&item : ip->Items) {
		  if(item->itemType == Atomic_Type::var) {
			if(item->labelName.length() > longest_var_len) {
			  longest_var = item->labelName;
			  longest_var_len = item->labelName.length();
			}
		  }
		}
	  }
	
	  return longest_var;
	}

    std::vector<std::vector<string>> convert_instruction(Instruction* ip, string long_var) {
        std::vector<std::vector<string>> ret_vectors;
		std::vector<string> ret_strings;

        if(ip->Type == InstructionType::assign) { //as it is
			ret_strings.push_back(ip->Items[0]->labelName);
			ret_strings.push_back("<-");
			ret_strings.push_back(ip->Items[1]->labelName);

            ret_vectors.push_back(ret_strings);
        } else if(ip->Type == InstructionType::assign_arithmetic) { //as it is
			ret_strings.push_back(ip->Items[0]->labelName);
			ret_strings.push_back("<-");

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

			ret_strings.push_back(ip->Items[2]->labelName);
	
            ret_vectors.push_back(ret_strings);
		} else if(ip->Type == InstructionType::assign_compare) { //as it is
			ret_strings.push_back(ip->Items[0]->labelName);
			ret_strings.push_back("<-");

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

			ret_strings.push_back(ip->Items[2]->labelName);

            ret_vectors.push_back(ret_strings);
		} else if(ip->Type == InstructionType::assign_load_array) {

		} else if(ip->Type == InstructionType::assign_store_array) {

		} else if(ip->Type == InstructionType::assign_new_array) {

		} else if(ip->Type == InstructionType::assign_new_tuple) {

		} else if(ip->Type == InstructionType::assign_length) {

		} else if(ip->Type == InstructionType::call) { // as it is
			ret_strings.push_back("call");

			if(ip->calleeType == CalleeType::print)
			  ret_strings.push_back("print");
			else if(ip->calleeType == CalleeType::array_error)
			  ret_strings.push_back("array-error");
			else // var, label
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
			ret_strings.push_back("call");

			if(ip->calleeType == CalleeType::print)
			  ret_strings.push_back("print");
			else if(ip->calleeType == CalleeType::array_error)
			  ret_strings.push_back("array-error");
			else // var, label
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
		} else if(ip->Type == InstructionType::br_conditional) { // explicit jump
			ret_strings.push_back("br");
			ret_strings.push_back(ip->Items[0]->labelName);
			ret_strings.push_back(ip->Items[1]->labelName);

            ret_vectors.push_back(ret_strings);
			ret_strings.clear();

			ret_strings.push_back("br");
			ret_strings.push_back(ip->Items[2]->labelName);

            ret_vectors.push_back(ret_strings);
        } else { //init_var
			// std::cerr << "init_var"; // init_var does not print anything for L3
        }

		return ret_vectors;
    } 

    void generate_code(Program p){
      
      /* 
       * Open the output file.
      */
      std::ofstream outputFile;
      outputFile.open("prog.L3");
  
      /* 
       * Generate L3 code
       */ 
  
      for(auto& fp: p.functions) {
          
		  // PRINT initial line of the function
          outputFile << "define " << fp->name << " (";
		  int arg_size = fp->arguments.size();
		  if (arg_size > 0) {
		    for(int i = 0; i < arg_size - 1; i++)
			  outputFile << fp->arguments[i]->labelName << ", ";
		    outputFile << fp->arguments[arg_size - 1];
		  }
		  outputFile << ") {";

		  // Get the longest var that will be used in the array-related functions
		  std::string longest_var = get_longest_varname(fp);

          for(auto &ip : fp->instructions) {
			  std::vector<std::vector<string>> to_print = convert_instruction(ip, longest_var);
			  if (!to_print.empty()) { //init_var does not print anything for L3
			    for(auto vec_str : to_print) {
                  outputFile << "\t";
                  for(auto str : vec_str)
                    outputFile << str << ' ';
                }
                outputFile << '\n';
			  }
          }
          outputFile << "}\n";
      }
      outputFile << '\n';

      /* 
       * Close the output file.
       */ 
      outputFile.close();
      return ;
    }
}
