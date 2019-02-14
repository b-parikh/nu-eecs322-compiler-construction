#include <string>
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <code_generator.h>
#include <stdlib.h>

using namespace std;

namespace L2{
  void generate_code(Program p){
    /* 
     * Open the output file.
    */
    std::ofstream outputFile;
    outputFile.open("prog.L1");

    /* 
     * Generate L1 code
     */ 
 
    outputFile << '(' << p.entryPointLabel << '\n';

    for (auto& fp : p.functions) {
        Function* new_F = register_allocation(fp);
        //std::cout << '(' << new_F->name << '\n';
        outputFile << "(" <<  new_F->name << '\n';
        //std::cout << new_F->arguments << ' ' << new_F->locals << '\n';
        outputFile << "\t" << new_F->arguments << ' ' << new_F->locals << '\n';        
        for(auto instruct_p : new_F->instructions) {
			outputFile << "\t";
            for(auto item : instruct_p->items) {
                //std::cout << item.labelName << ' ';
                outputFile << item.labelName << ' ';
            }
            //std::cout << '\n';
            outputFile << '\n';
        }
        //std::cout << '\n';
        outputFile << ")\n";
    }
    //std::cout << '\n';
    outputFile << ")\n";
    outputFile << '\n';

    /* 
     * Close the output file.
     */ 
    outputFile.close();
    return ;
  }
}

