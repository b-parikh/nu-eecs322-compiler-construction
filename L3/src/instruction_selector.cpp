#include <string>

namespace L3 {

	Function* instruction_selection(Funtion* fp) {
		Function* new_F;
		new_F->name = fp->name;
		new_F->arguments = fp->arguments.size();
		new_F->locals = 0;
		for (auto &ip : fp->instructions) {
			if {// Instruction_assign
				Instruction* i = new Instruction();
				i->items.push_back(ip->items[0]);
				i->items.push_back("<-");
				i->items.push_back(ip->items[1]);
				new_F->instructions.push_back(i);
			} else if { // Instruction_assign_arithmetic
				Instruction* i = new Instruction();
				i->items.push_back(ip->items[0]);
				i->items.push_back("<-");
				i->items.push_back(ip->items[1]);
				new_F->instructions.push_back(i);

				Instruction* i2 = new Instruction();
				i->items.push_back(ip->items[0]);
				i->items.push_back(ip->items[2] + "=");
				i->items.push_back(ip->items[4]);
				new_F->instructions.push_back(i2);
			} else if { // Instruction_assign_compare
				Instruction* i = new Instruction();
				i->items.push_back(ip->items[0]);
				i->items.push_back("<-");
				i->items.push_back(ip->items[1]);
				i->items.push_back(ip->items[2]);
				i->items.push_back(ip->items[3]);
				new_F->instructions.push_back(i);
			} else if { // Instruction_assign_load
				Instruction* i = new Instruction();
				i->items.push_back(ip->items[0]);
				i->items.push_back("<-");
				i->items.push_back("mem");
				i->items.push_back(ip->items[1]);
				i->items.push_back("0"); //TODO memory should be calculated as more same var/reg are being loaded in a function
				new_F->instructions.push_back(i);
			} else if { // Instruction_assign_store
				Instruction* i = new Instruction();
				i->items.push_back("mem");
				i->items.push_back(ip->items[0]);
				i->items.push_back("0"); //TODO same as above
				i->items.push_back("<-"); 
				i->items.push_back(ip->items[1]);
				new_F->instructions.push_back(i);
			} else if { // Instruction_return_empty
				Instruction* i = new Instruction();
				i->items.push_back("return");
				new_F->instructions.push_back(i);
			} else if { // Instruction_return_value
				Instruction* i = new Instruction();
				i->items.push_back("rax");
				i->items.push_back("<-");
				i->items.push_back(ip->items[0]);
				new_F->instructions.push_back(i);

				Instruction* i2 = new Instruction();
				i->items.push_back("return");
				new_F->instructions.push_back(i2);
			} else if { // Instruction_label
				Instruction* i = new Instruction();
				i->items.push_back(ip->items[0]);
				new_F->instructions.push_back(i);
			} else if { // Instruction_br_unconditional
				Instruction* i = new Instruction();
				i->items.push_back("goto");
				i->items.push_back(ip->items[0]);
				new_F->instructions.push_back(i);
			} else if { // Instruction_br_conditional
				Instruction* i = new Instruction(); //TODO for tiling, the predecessor and cjump should beone tile 
				i->items.push_back("cjump");
				i->items.push_back(ip->items[0]);
				i->items.push_back("=");
				i->items.push_back("true");
				i->items.push_back(ip->items[1]);
				new_F->instructions.push_back(i);
			} else if { // Instruction_call
				Instruction* i = new Instruction(); //TODO call and call_assign should be done with calling convention?
				new_F->instructions.push_back(i);
			} else { // Instruction_call_assign
				Instruction* i = new Instruction();
				new_F->instructions.push_back(i);
			}
		}
		return new_F;
	}
}
