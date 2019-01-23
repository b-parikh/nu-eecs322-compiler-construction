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
    //std::ofstream outputFile;
    //outputFile.open("prog.S");
    FILE* outputFile;
    outputFile = fopen("prog.S", "w"); 
    /* 
     * Generate target code
     */ 

    //TODO
    fprintf(outputFile,
"       .text\n"
"       \t.globl go\n"
"         go:\n"
"         \tpushq %rbx\n"
"         \tpushq %rbp\n"
"         \tpushq %r12\n"
"         \tpushq %r13\n"
"         \tpushq %r14\n"
"         \tpushq %r15\n"
"         \t call %s\n"
"         \tpopq %r15\n"
"         \tpopq %r14\n"
"         \tpopq %r13\n"
"         \tpopq %r12\n"
"         \tpopq %rbp\n"
"	 \tpopq %rbx\n"
"	 \tretq\n",progNameModifier(p.entryPointLabel).c_str());

    /* 
     * Close the output file.
     */ 
    //outputFile.close();
    fclose(outputFile);
    return ;
  }


  string progNameModifier(string inputLabel) {
    inputLabel.at(0) = '_';
    return inputLabel;
  }

}
