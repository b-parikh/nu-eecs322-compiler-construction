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
    
    void replace_labels(Program &p) {
        std::vector<std::string> allFunctionNames;
        for(auto& fp : p.functions)
            allFunctionNames.push_back(fp->name);
        
        for(auto& fp : p.functions) {
            std::string functionName = fp->name;
            functionName.erase(0,1);
            for(auto &ip : fp->instructions) {
                if(ip->Type != InstructionType::call && ip->Type != InstructionType::call_assign) {
                    for(auto &itp : ip->Items) {
                        if(itp->Type == Atomic_Type::label) {
                            if(std::find(allFunctionNames.begin(), allFunctionNames.end(), itp->labelName) != allFunctionNames.end())
                                continue;

                            itp->labelName = itp->labelName + '_' + functionName;
                        }
                    }
                }
            }
        }
    }

    void generate_code(Program p){
      replace_labels(p);
      // put main at start of function list
      if(p.functions.size() > 1) {
        for(int i = 0; i < p.functions.size(); ++i){
            if(p.functions[i]->isMain){
                Function* temp = p.functions[i];
                p.functions[i] = p.functions[0];
                p.functions[0] = temp;
                 //std::swap(p.functions[i],p.functions[0]);
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
