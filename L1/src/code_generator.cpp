#include <string>
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <code_generator.h>

using namespace std;

namespace L1{

  string labelModifier(string inputLabel) {
    inputLabel.at(0) = '_';
    return inputLabel;
  }

  void write_assignment(Instruction* ip, std::ofstream &outputFile) {
        int instruction_length = (ip->items).size();
	outputFile << "moveq ";
	if (instruction_length == 3) {
	   Item dest = ip->items[0];
	   Item src = ip->items[2]; 

	   if (src.labelName[0] == 'r') // reg <- reg
	      src.labelName = '%' + src.labelName;
	   else if (src.labelName[0] == ':') // reg <- label
	      src.labelName = '$' + labelModifier(src.labelName);
	   else // reg <- const
	      src.labelName = '$' + src.labelName;
           outputFile << src.labelName << ", " << dest.labelName << '\n';

	} else { 
		if (ip->items[0].labelName == "mem") { // store into memory
		   Item dest = ip->items[1];
		   Item offset = ip->items[2];
		   Item src = ip->items[4]; 
		   if (src.labelName[0] == 'r') { // mem <- reg
		      src.labelName = '%' + src.labelName;
		   }
		   else if (src.labelName[0] == ':') { // mem <- label
		      src.labelName = '$' + labelModifier(src.labelName);
		   } else { // mem <- const
		      src.labelName = '$' + src.labelName;
		   }
	       outputFile << src.labelName << ", " << offset.labelName<< '(' << '%' << dest.labelName << ")\n";
		} else { // reg <- mem
		   Item src = ip->items[3];
	       Item dest = ip->items[0];
	       Item offset = ip->items[4];

 	       outputFile << offset.labelName << "(%" << src.labelName << "), " << dest.labelName << '\n'; 
		}
	}
  }


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
         "\tcall " <<  labelModifier(p.entryPointLabel) << "\n"
         "\tpopq %r15\n"
         "\tpopq %r14\n"
         "\tpopq %r13\n"
         "\tpopq %r12\n"
         "\tpopq %rbp\n"
	 "\tpopq %rbx\n"
	 "\tretq\n\n";


    int vector_size = p.functions.size();
    for(int i = 0; i < vector_size; ++i) {
           auto fp = p.functions[i];
	   outputFile << labelModifier(fp->name) << ":\n";
	   // add func arg # and local #
	   // function to iterate through instructions vector
	   for (Instruction* ip : fp->instructions) {
	       if(ip->identifier == 0)
		  write_assignment(ip, outputFile);
	   }
    }

    /* 
     * Close the output file.
     */ 
    outputFile.close();
    return ;
  }
}

