#include <string>
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <code_generator.h>
#include <stdlib.h>
#include <L2.h>
#include <instruction_selector.h>
#include <algorithm>

using namespace std;

namespace L3{
   
	// replace all labels except function names 
    void replace_labels(Program &p) {
        for(auto& fp : p.functions) {
            std::string functionName = fp->name;
            functionName.erase(0,1);

			// Find all label in the function
	        std::vector<std::string> allLabelNames;
            for(auto &ip : fp->instructions) {
                if(ip->Type == InstructionType::label) {
                    //std::cout << ip->Items[0] ->labelName << '\n';
                    allLabelNames.push_back(ip->Items[0]->labelName);
                }
            }

            for(auto &ip : fp->instructions) {
                if(ip->Type != InstructionType::call && ip->Type != InstructionType::call_assign) {
                    for(auto &itp : ip->Items) {
                        if(itp->Type == Atomic_Type::label) {
                            if(std::find(allLabelNames.begin(), allLabelNames.end(), itp->labelName) != allLabelNames.end())
								itp->labelName = itp->labelName + '_' + functionName;
                        }
                    }
                }
            }
			allLabelNames.clear();
        }
    }

    void generate_code(Program p){
      replace_labels(p);
      // put main at start of function list
      if(p.functions.size() > 1) {
        for(int i = 0; i < p.functions.size(); ++i){
			//std::cerr<< p.functions[i]->name << ':' << p.functions[i]->isMain << '\n';
            if(p.functions[i]->isMain){
                Function* temp = p.functions[i];
                p.functions[i] = p.functions[0];
                p.functions[0] = temp;
                 break;
            }
        }
      }
      
      /* 
       * Open the output file.
      */
      std::ofstream outputFile;
      outputFile.open("prog.L2");
  
      /* 
       * Generate L2 code
       */ 
  
      outputFile << '(' << p.functions[0]->name << '\n'; // :main
      for(auto& fp: p.functions) {
          L2::Function* new_L2_F = instruction_selection(fp);
          
          outputFile << "\t(" << new_L2_F->name << '\n';
          outputFile << '\t' << new_L2_F->arguments << ' ' << new_L2_F->locals << '\n'; // TODO; insert locals number
  
          for(auto &instructp : new_L2_F->instructions) {
              outputFile << "\t\t";
              for(auto &item : instructp->items) {
                  outputFile << item.labelName << ' ';
              }
              outputFile << '\n';
          }
          outputFile << ")\n\n";
      }
      outputFile << ")\n";
      /* 
       * Close the output file.
       */ 
      outputFile.close();
      return ;
    }
}
