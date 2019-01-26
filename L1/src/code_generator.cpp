//#include <string>
#include <iostream>
#include <fstream>
#include <stdio.h>

#include <code_generator.h>

using namespace std;

namespace L1{
  void generate_code(Program p){

    /* 
     * Open the output file.
     */ 
    std::ofstream outputFile;
    outputFile.open("prog.S");
    /* 
     * Generate target code
     */ 
 
    //TODO
     outputFile <<  
       ".text\n"
       "\t.globl go\n"
        "go:\n"
         "\tpushq %rbx\n"
         "\tpushq %rbp\n"
         "\tpushq %r12\n"
         "\tpushq %r13\n"
         "\tpushq %r14\n"
         "\tpushq %r15\n"
         "\tcall " <<  progNameModifier(p.entryPointLabel) << "\n"
         "\tpopq %r15\n"
         "\tpopq %r14\n"
         "\tpopq %r13\n"
         "\tpopq %r12\n"
         "\tpopq %rbp\n"
	 "\tpopq %rbx\n"
	 "\tretq\n\n";


    int vector_size = p.functions.size();
    for(int i = 0; i < vector_size; ++i) {
	   outputFile << progNameModifier(p.functions[i]->name) << ":\n";
    } 
    /* 
     * Close the output file.
     */ 
    outputFile.close();
    return ;
  }


  string progNameModifier(string inputLabel) {
    inputLabel.at(0) = '_';
    return inputLabel;
    }
  }

