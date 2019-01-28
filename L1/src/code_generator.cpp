#include <string>
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <code_generator.h>
#include <stdlib.h>

using namespace std;

namespace L1{

  string labelModifier(string inputLabel) {
    inputLabel.at(0) = '_';
    return inputLabel;
  }

  void write_assignment(Instruction* ip, std::ofstream& outputFile) {
    int instruction_length = (ip->items).size();
	outputFile << "movq ";
	if (instruction_length == 3) {
	   Item dest = ip->items[0]; // append '%' here instead of in L29
	   Item src = ip->items[2]; 

	   if (src.labelName[0] == 'r') // reg <- reg
	      src.labelName = '%' + src.labelName;
	   else if (src.labelName[0] == ':') // reg <- label
	      src.labelName = '$' + labelModifier(src.labelName);
	   else // reg <- const
	      src.labelName = '$' + src.labelName;
       outputFile << src.labelName << ", %" << dest.labelName << '\n';
	} else { 
		if (ip->items[0].labelName == "mem") { // store into memory
		   Item dest = ip->items[1];
		   Item offset = ip->items[2];
		   Item src = ip->items[4]; 
		   if (src.labelName[0] == 'r') { // mem <- reg
		     // std::cout << src.labelName << '\n';
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

 	       outputFile << offset.labelName << "(%" << src.labelName << "), %" << dest.labelName << '\n'; 
		}
	}
  }
  void write_return(Instruction* ip, std::ofstream& outputFile) {
	outputFile << "retq\n";
  }

  std::string register_map(std::string reg) {
     if(reg[1] == '1' || reg.length() == 2)
        return reg + "b";
     else if(reg[2] == 'x')
        return reg[1] + "l";
     else
        return reg[1] + reg[2] + "l";
  }
  
  std::string comparison_map(std::string comp) {
      if(comp == "<")
	 return "setl";
      else if (comp == "<=") 
	 return "setle";
      else 
 	 return "sete";
  }

  std::string comparison_map_switched(std::string comp) {
	if(comp == "<")
		return "setg";
	else if (comp == "<=")
		return "setge";
	else 
		return "sete";
 }

  std::string find_arithmetic_op(Item op) {
	char oper = op.labelName[0];
	if(oper == '+')
	   return "addq";
	else if(oper == '-')
	   return "subq";
	else if(oper == '*')
	   return "imulq";
	else
	   return "andq";
  }

  void write_arithmetic(Instruction* ip, std::ofstream& outputFile) {
	//mem += reg, const
	//reg += reg, const, mem
	int instruction_length = (ip->items).size();
	 
	if (instruction_length == 3) {
	   Item dest = ip->items[0];
	   dest.labelName = '%' + dest.labelName;
	   
	   Item src = ip->items[2]; 
	   Item op = ip->items[1];

	   if (src.labelName[0] == 'r') // reg <- reg
	      src.labelName = '%' + src.labelName;
	   else // reg <- const
	      src.labelName = '$' + src.labelName;
       outputFile << find_arithmetic_op(op) << ' ' << src.labelName << ", " << dest.labelName << '\n';
	} else { 
		if (ip->items[0].labelName == "mem") { // mem on left side
		   Item dest = ip->items[1];
		   dest.labelName = '%' + dest.labelName;

		   Item offset = ip->items[2];
		   Item op = ip-> items[3];
		   Item src = ip->items[4]; 

		   if (src.labelName[0] == 'r') { // mem <- reg
		      src.labelName = '%' + src.labelName;
		   } else { // mem <- const
		      src.labelName = '$' + src.labelName;
		   }
	       outputFile << find_arithmetic_op(op) << ' ' <<  src.labelName << ", " << offset.labelName<< '(' << dest.labelName << ")\n";
		} else { // reg <- mem
		   Item src = ip->items[3];
		   src.labelName = '%' + src.labelName;
	           Item dest = ip->items[0];
		   dest.labelName = '%' + dest.labelName;
	       	   Item offset = ip->items[4];
		   Item op = ip->items[1];
 	           outputFile << find_arithmetic_op(op) << ' ' << offset.labelName << '(' << src.labelName << "), " << dest.labelName << '\n'; 
		}
   }
 }

  void write_inc_dec(Instruction* ip, std::ofstream& outputFile) {
     Item reg = ip->items[0];
     reg.labelName = '%' + reg.labelName;
     Item op = ip->items[1];

     if(op.labelName[0] == '-')
	outputFile << "dec " << reg.labelName << '\n';
     else
	outputFile << "inc " << reg.labelName << '\n';
  }

  void write_assign_comparison(Instruction* ip, std::ofstream& outputFile) {
     Item dest = ip->items[0];
     Item arg1 = ip->items[2];
     Item arg2 = ip->items[4];
     Item comp = ip->items[3];

     if(arg1.labelName[0] == 'r') {
	if(arg2.labelName[0] == 'r')
	   outputFile << "cmpq %" << arg2.labelName << ", %" << arg1.labelName << '\n';
	else
	   outputFile << "cmpq $" << arg2.labelName << ", %" << arg1.labelName << '\n';
	
	outputFile << comparison_map(comp.labelName) << " %" << register_map(dest.labelName) << '\n';
	outputFile << "movzbq %" << register_map(dest.labelName) << ", %" << dest.labelName << '\n';
      }
      else if(arg2.labelName[0] == 'r') {
	   outputFile << "cmpq %" << arg1.labelName << ", $" << arg2.labelName << '\n';
	   outputFile << comparison_map_switched(comp.labelName) << " %" << register_map(dest.labelName) << '\n';
	   outputFile << "movzbq %" << register_map(dest.labelName) << ", %" << dest.labelName << '\n';
      } else { // both args are numbers
	  bool out;
	  if (comp.labelName == "<")
	       out = std::stoi(arg1.labelName) < std::stoi(arg2.labelName);
	  else if(comp.labelName == "<=")
	       out = std::stoi(arg1.labelName) <= std::stoi(arg2.labelName);
	  else
	       out = std::stoi(arg1.labelName) == std::stoi(arg2.labelName);

	  outputFile << "movq $" << (int)out << ", %" << dest.labelName << '\n';
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
       //std::cout << "p.functions.size() = " <<  vector_size << '\n'; // 2
       auto fp = p.functions[i];
       //std::cout << "fp->instructions.size() = " << fp->instructions.size() << '\n';
       outputFile << labelModifier(fp->name) << ":\n";
	   // add func arg # and local #
	   // function to iterate through instructions vector
	   for (Instruction* ip : fp->instructions) {
	        //std::cout << "2nd for loop" << '\n';
		//std::cout << ip->items[0].labelName << '\n';	
	        if(ip->identifier == 0)
	 	    write_assignment(ip, outputFile);
		else if(ip->identifier == 1)
		   write_return(ip, outputFile);
		else if(ip->identifier == 2)
		   write_arithmetic(ip, outputFile);
		else if(ip->identifier == 3)
		   write_inc_dec(ip, outputFile);
		else if(ip->identifier == 4)
		   write_assign_comparison(ip, outputFile);
	   }
	//std::cout << vector_size << '\n'; // doesn't print
    }

    /* 
     * Close the output file.
     */ 
    outputFile.close();
    return ;
  }
}

