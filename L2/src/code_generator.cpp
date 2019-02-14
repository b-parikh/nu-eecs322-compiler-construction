#include <string>
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <code_generator.h>
#include <stdlib.h>

using namespace std;

namespace L2{

  std::string labelModifier(std::string inputLabel) {
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
  void write_return(int move_stack_by, std::ofstream& outputFile) {
        outputFile << "addq $" << move_stack_by << ", %rsp\n";
	outputFile << "retq\n";
  }

  std::string register_map(std::string reg) {
	if(reg == "r10")
		return "r10b";
	else if(reg == "r11")
		return "r11b";
	else if(reg == "r12")
		return "r12b";
	else if(reg == "r13")
		return "r13b";
	else if(reg == "r14")
		return "r14b";
	else if(reg == "r15")
		return "r15b";
	else if(reg == "r8")
		return "r8b";
	else if(reg == "r9")
		return "r9b";
	else if(reg == "rax")
		return "al";
	else if(reg == "rbx")
		return "bl";
	else if(reg == "rcx")
		return "cl";
	else if(reg == "rdx")
		return "dl";
	else if(reg == "rbp")
		return "bpl";
	else if(reg == "rdi")
		return "dil";
	else // rsi
		return "sil";
  }
  std::string comparison_map(std::string comp) {
      if(comp == "<")
	 return "l";
      else if (comp == "<=") 
	 return "le";
      else 
 	 return "e";
  }

  std::string comparison_map_switched(std::string comp) {
	if(comp == "<")
		return "g";
	else if (comp == "<=")
		return "ge";
	else 
		return "e";
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
	if(arg2.labelName[0] == 'r'){
	   outputFile << "cmpq %" << arg2.labelName << ", %" << arg1.labelName << '\n';
	}
	else{
	   outputFile << "cmpq $" << arg2.labelName << ", %" << arg1.labelName << '\n';
	}
	outputFile << "set" << comparison_map(comp.labelName) << " %" << register_map(dest.labelName) << '\n';
	outputFile << "movzbq %" << register_map(dest.labelName) << ", %" << dest.labelName << '\n';
      }
      else if(arg2.labelName[0] == 'r') {
	   	outputFile << "cmpq $" << arg1.labelName << ", %" << arg2.labelName << '\n';
	   	outputFile << "set" << comparison_map_switched(comp.labelName) << " %" << register_map(dest.labelName) << '\n';
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

	void write_shift(Instruction* ip, std::ofstream& outputFile) {
		std::string left = ip->items[0].labelName;
		left = '%' + left;
		std::string shift = ip->items[1].labelName;
		std::string right = ip->items[2].labelName;

     if(shift[0] == '<'){
		if(right[0] == 'r')
			outputFile << "salq " << "%" << register_map(right) << ", " << left << '\n';
		else
			outputFile << "salq " << "$" << right << ", " << left << '\n';
		}

     else {
	if(right[0] == 'r')
	   outputFile << "sarq " << "%" << register_map(right) << ", " << left << '\n';
	else
	   outputFile << "sarq " << "$" << right << ", " << left << '\n';
     }
  }

  void write_goto_jump(Instruction* ip, std::ofstream& outputFile) {
	//std::cout << "jmp " << labelModifier(ip->items[1].labelName) << '\n';
	outputFile << "jmp " << labelModifier(ip->items[1].labelName) << '\n';
  }

  void write_cjump_twoargs(Instruction* ip, std::ofstream& outputFile) {
    std::string arg1 = ip->items[1].labelName;
    std::string arg2 = ip->items[3].labelName;
    std::string comp = ip->items[2].labelName;
    std::string label1 = ip->items[4].labelName;
    std::string label2 = ip->items[5].labelName;

	if(arg1[0] == 'r') {
		if(arg2[0] == 'r')
			outputFile << "cmpq %" << arg2 << ", %" << arg1 << '\n';
		else
			outputFile << "cmpq $" << arg2 << ", %" << arg1 << '\n';
			outputFile << 'j' << comparison_map(comp) << ' ' << labelModifier(label1) << '\n';
		if(ip->items.size() == 6)
			outputFile << "jmp " << labelModifier(label2) << '\n';
	}
      else if(arg2[0] == 'r') {
	   	outputFile << "cmpq $" << arg1 << ", %" << arg2 << '\n';
	   	outputFile << 'j' << comparison_map_switched(comp) << ' ' << labelModifier(label1) << '\n';
	   	 if(ip->items.size() == 6)
	      	outputFile << "jmp " << labelModifier(label2) << '\n';
      } else { // both args are numbers so calculate during compile time
	  bool out;
	  if (comp == "<")
	       out = std::stoi(arg1) < std::stoi(arg2);
	  else if(comp == "<=")
	       out = std::stoi(arg1) <= std::stoi(arg2);
	  else
	       out = std::stoi(arg1) == std::stoi(arg2);

	  if(out)
	      outputFile << "jmp " << labelModifier(label1) << '\n';
	  else // if(ip->items.size() == 6)
	       outputFile << "jmp " << labelModifier(label2) << '\n';
    }
 }

  void write_cjump_onearg(Instruction* ip, std::ofstream& outputFile) {
     std::string arg1 = ip->items[1].labelName;
     std::string arg2 = ip->items[3].labelName;
     std::string comp = ip->items[2].labelName;
     std::string label = ip->items[4].labelName;
     
     if(arg1[0] == 'r') {
	if(arg2[0] == 'r'){
	   outputFile << "cmpq %" << arg2 << ", %" << arg1 << '\n';
	}
	else{
	   outputFile << "cmpq $" << arg2 << ", %" << arg1 << '\n';
	}
	outputFile << 'j' << comparison_map(comp) << ' ' << labelModifier(label) << '\n';
      }
      else if(arg2[0] == 'r') {
	   outputFile << "cmpq $" << arg1 << ", %" << arg2 << '\n';
	   outputFile << 'j' << comparison_map_switched(comp) << ' ' << labelModifier(label) << '\n';
           //std::cerr << "WRITING J INST\n";
      } else { // both args are numbers
	  bool out;
	  if (comp == "<")
	       out = std::stoi(arg1) < std::stoi(arg2);
	  else if(comp == "<=")
	       out = std::stoi(arg1) <= std::stoi(arg2);
	  else
	       out = std::stoi(arg1) == std::stoi(arg2);

	  if(out)
	      outputFile << "jmp " << labelModifier(label) << '\n';
     	  }
    }
     

  void write_lea(Instruction* ip, std::ofstream& outputFile) { // @ is not pushed into parsed_registers
     std::string reg1 = ip->items[0].labelName;
     std::string reg2 = ip->items[2].labelName;
     std::string reg3 = ip->items[3].labelName;
     std::string num = ip->items[4].labelName;

     outputFile << "lea " << "(%" << reg2 << ", %" << reg3 << ", " << num << "), %" << reg1 << '\n';
  }

  void write_call(Instruction* ip, std::ofstream& outputFile, int num_args) {
     std::string label = ip->items[1].labelName;
     if(label == "print" || label == "allocate")
        outputFile << "call " << label << '\n';
     else if (label == "array-error")
        outputFile << "call array_error" << '\n';
     else {
        int move_stack_by = 1;
        if(num_args > 6)
            move_stack_by += num_args - 6;
        move_stack_by *= 8;
        outputFile << "subq $" << move_stack_by << ", %rsp\n";
        if(label[0] == 'r')
           outputFile << "jmp *%" << label << '\n';
        else
     	   outputFile << "jmp " << labelModifier(label) << '\n'; 
    }
}

  void write_label_instruction(Instruction* ip, std::ofstream& outputFile) {
    outputFile << labelModifier(ip->items[0].labelName) << ":\n";
  }

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

