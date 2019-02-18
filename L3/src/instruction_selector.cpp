#include <string>

namespace L3 {

	Function* instruction_selection(Funtion* fp) {
		Function* new_F = new L2::Function();
		new_F->name = fp->name;
		new_F->arguments = fp->arguments.size();
		new_F->locals = 0;
		for (auto &ip : fp->instructions) {
			if {// Instruction_assign
				Instruction* i = new L2::Instruction();
				i->items.push_back(ip->items[0]);
				L2::Item* assign = createL2Item("<-", L2::Type::assign_oper);
				i->items.push_back(assign);
				i->items.push_back(ip->items[1]);
				new_F->instructions.push_back(i);
			} else if { // Instruction_assign_arithmetic
				Instruction* i = new L2::Instruction();
				i->items.push_back(ip->items[0]);
				L2::Item* assign = createL2Item("<-", L2::Type::assign_oper);
				i->items.push_back(assign);
				i->items.push_back(ip->items[1]);
				new_F->instructions.push_back(i);

				Instruction* i2 = new L2::Instruction();
				i2->items.push_back(ip->items[0]);
				i2->items.push_back(ip->items[2]);
				L2::Item* compare = createL2Item("=", L2::Type::compare_oper);
				i2->items.push_back(compare);
				i2->items.push_back(ip->items[4]);
				new_F->instructions.push_back(i2);
			} else if { // Instruction_assign_compare
				Instruction* i = new L2::Instruction();
				i->items.push_back(ip->items[0]);
				L2::Item* assign = createL2Item("<-", L2::Type::assign_oper);
				i->items.push_back(assign);

				if(ip->items[2].labelName == ">=" || ip->items[2].labelName == ">") {
					i->items.push_back(ip->items[3]);
					ip->items[2].labelName.replace(0, 1, "<");
					i->items.push_back(ip->items[2]);
					i->items.push_back(ip->items[1]);
				} else { // as normal L2 comparisons
					i->items.push_back(ip->items[1]);
					i->items.push_back(ip->items[2]);
					i->items.push_back(ip->items[3]);
				}

				new_F->instructions.push_back(i);
			} else if { // Instruction_assign_load
				Instruction* i = new L2::Instruction();
				i->items.push_back(ip->items[0]);
				L2::Item* assign = createL2Item("<-", L2::Type::assign_oper);
				i->items.push_back(assign);
				L2::Item* mem = createL2Item("mem", L2::Type::mem);
				i->items.push_back(mem);
				i->items.push_back(ip->items[1]);
				//TODO memory should be calculated as more same var/reg are being loaded in a function
				L2::Item* number = createL2Item("0", L2::Type::number);
				i->items.push_back(number);
				new_F->instructions.push_back(i);
			} else if { // Instruction_assign_store
				Instruction* i = new L2::Instruction();
				L2::Item* mem = createL2Item("mem", L2::Type::compare_oper);
				i->items.push_back(mem);
				i->items.push_back(ip->items[0]);
				//TODO same as above
				L2::Item* number = createL2Item("0", L2::Type::number);
				i->items.push_back(number);
				L2::Item* assign = createL2Item("<-", L2::Type::assign_oper);
				i->items.push_back(assign);
				i->items.push_back(ip->items[1]);
				new_F->instructions.push_back(i);
			} else if { // Instruction_return_empty
				Instruction* i = new L2::Instruction();
				L2::Item* item = new L2::Item();
				item->labelName = "return";
				i->items.push_back(item);
				new_F->instructions.push_back(i);
			} else if { // Instruction_return_value
				Instruction* i = new L2::Instruction();
				L2::Item* reg = createL2Item("rax", L2::Type::reg);
				i->items.push_back(reg);
				L2::Item* assign = createL2Item("<-", L2::Type::assign_oper);
				i->items.push_back(assign);
				i->items.push_back(ip->items[0]);
				new_F->instructions.push_back(i);

				Instruction* i2 = new L2::Instruction();
				L2::Item* item = new L2::Item();
				item->labelName = "return";
				i2->items.push_back(item);
				new_F->instructions.push_back(i2);
			} else if { // Instruction_label
				Instruction* i2 = new L2::Instruction();
				i->items.push_back(ip->items[0]);
				new_F->instructions.push_back(i);
			} else if { // Instruction_br_unconditional
				Instruction* i2 = new L2::Instruction();
				L2::Item* item = new L2::Item();
				item->labelName = "goto";
				i->items.push_back(item);
				i->items.push_back(ip->items[0]);
				new_F->instructions.push_back(i);
			} else if { // Instruction_br_conditional
				Instruction* i2 = new L2::Instruction(); //TODO for tiling, the predecessor and cjump should beone tile 
				L2::Item* item = new L2::Item();
				item->labelName = "cjump";
				i->items.push_back(item);
				i->items.push_back(ip->items[0]);
				L2::Item* compare = createL2Item("=", L2::Type::compare_oper);
				i2->items.push_back(compare);
				L2::Item* item2 = new L2::Item();
				item2->labelName = "true";
				i->items.push_back(item2);
				i->items.push_back(ip->items[1]);
				new_F->instructions.push_back(i);
			} else if { // Instruction_call
				Instruction* i2 = new L2::Instruction();
				new_F->instructions.push_back(i);
			} else { // Instruction_call_assign
				Instruction* i2 = new L2::Instruction();
				new_F->instructions.push_back(i);
			}
		}
		return new_F;
	}
}
