#include <string>
#include <instruction_selector.h>
#include <L2.h>

namespace L3 {

    Item* createL2Item(std::string labelName, L2::Type aType) {
        L2::Item* item = new L2::Item();
        item->labelName = labelName;
        item->Type = aType;

        return item;
    }

    std::vector<L2::Instruction*> enforce_caller_convention(Instruction_call* ip) {
        std::vector<std::string> ARG_REGISTERS = 
        {"rdi", "rsi", "rdx", "rcx", "r8", "r9"};

        std::vector<L2::Instruction*> instructs_to_ret;
        int stack_loc = -8;
        L2::Instruction* mem_alloc = new L2::Instruction();

        L2::Item* mem = createL2Item("mem", L2::Type::mem);
        mem_alloc->items.push_back(*mem);

        L2::Item* rsp = createL2Item("rsp", L2::Type::reg);
        mem_alloc->items.push_back(*rsp);

        L2::Item* move_rsp_by = createL2Item(std::to_string(stack_loc), L2::Type::num);
        mem_alloc->items.push_back(*move_rsp_by);
        stack_loc -= 8;

        L2::Item* assign_oper = createL2Item("<-", L2::Type::assign_oper);
        mem_alloc->items.push_back(*assign_oper);

        // the last item in the instruction is the function being called
        std::string ret_label = ip->items[ip->items.size() -1].labelName;
        ret_label = ret_label + "_ret";
        L2::Item* ret_label_item = createL2Item(ret_label, L2::Type label);
        mem_alloc->items.push_back(*ret_label_item);

        instructs_to_ret.push_back(mem_alloc);

        // add arguments
        for(int j = 0; j < ip->arguments.size(); ++j) {
            auto currArgument = ip->arguments[j];
            L2::Instruction* into_reg = new L2::Instruction();
            L2::Item* reg = createL2Item(ARG_REGISTERS[j], L2::Type::reg);
            into_reg->items.push_back(*reg);
            
            L2::Item* assign_oper = createL2Item("<-", L2::Type::assign_oper);
            into_reg->items.push_back(*assign_oper);
            
            L2::Item* arg;
            if(currArgument.Type == Atomic_Type::num)
                arg = createL2Item(ip->arguments[j].labelName, L2::Type::num);
            else // arg is var
                arg = createL2Item(ip->arguments[j].labelName, L2::Type::var);

            into_reg->items.push_back(*arg);
            instructs_to_ret.push_back(into_reg);

        }

        // store rest of args on stack
        if(ARG_REGISTERS.size() > ip->arguments.size()) {
            for(int j = ARG_REGISTERS.size(); j < arguments.size(); ++j) {
                    
                // mem rsp stack_loc
                L2::Instruction* mem_alloc = new L2::Instruction();
    
                L2::Item* mem = createL2Item("mem", L2::Type::mem);
                mem_alloc->items.push_back(*mem);
        
                L2::Item* rsp = createL2Item("rsp", L2::Type::reg);
                mem_alloc->items.push_back(*rsp);
        
                L2::Item* move_rsp_by = createL2Item(std::to_string(stack_loc), L2::Type::num);
                mem_alloc->items.push_back(*move_rsp_by);
        
                stack_loc -= 8;

                L2::Item* assign_oper = createL2Item("<-", L2::Type::assign_oper);
                mem_alloc->items.push_back(*assign_oper);
                
                L2::Item* arg;
                if(currArgument.Type == Atomic_Type::num)
                    arg = createL2Item(ip->arguments[j].labelName, L2::Type::num);
                else // arg is var
                    arg = createL2Item(ip->arguments[j].labelName, L2::Type::var);
            
                mem_alloc->items.push_back(*arg);

                instructs_to_ret.push_back(mem_alloc);

            }
        }

        L2::Instruction* call_I = new L2::Instruction();
        L2::Item* call = new L2::Item();
        call->labelName = "call";
        call->Type = L2::Type::num;

        L2::Item* fToCall = new L2::Item();
        fToCall.labelName = ip->Items[1]->labelName;
        fToCall.type = L2::Type::label;

        L2::Item* numArgs = new L2::Item();
        numArgs.labelName = std::to_string(ip->arguments.size());
        numArgs.type = L2::Type::num;

        call_I.push_back(*call);
        call_I.push_back(*fToCall);
        call_I.push_back(*numArgs);

        instructs_to_ret.push_back(call_I);

        L2::Instruction* ret_label_i = new L2::Instruction();
        ret_label_i.push_back(ret_label_item); // from previous use of return label

        return instructs_to_ret;
    }

    
    std::vector<L2::Instruction*> enforce_callee_convention(std::vector<Item*> arguments) {
        std::vector<std::string> ARG_REGISTERS = {"rdi", "rsi", "rdx", "rcx", "r8", "r9"};

        // add arguments
        for(int j = 0; j < arguments.size(); ++j) {
            auto currArgument = arguments[j];
            L2::Instruction* into_reg = new L2::Instruction();

            L2::Item* arg;
            arg = createL2Item(arguments[j].labelName, L2::Type::var);
            into_reg->items.push_back(*arg);

            L2::Item* assign_oper = createL2Item("<-", L2::Type::assign_oper);
            into_reg->items.push_back(*assign_oper);

            L2::Item* reg = createL2Item(ARG_REGISTERS[j], L2::Type::reg);
            into_reg->items.push_back(*reg);
            
            instructs_to_ret.push_back(into_reg);

        }
        int stack_loc = 0;
        // store rest of args on stack
        if(ARG_REGISTERS.size() > arguments.size()) {
            for(int j = ARG_REGISTERS.size(); j < arguments.size(); ++j) {
                L2::Instruction* mem_alloc = new L2::Instruction();
                
                L2::Item* arg;
                if(currArgument.Type == Atomic_Type::num)
                    arg = createL2Item(arguments[j].labelName, L2::Type::num);
                else // arg is var
                    arg = createL2Item(arguments[j].labelName, L2::Type::var);
            
                mem_alloc->items.push_back(*arg);
                
                L2::Item* assign_oper = createL2Item("<-", L2::Type::assign_oper);
                mem_alloc->items.push_back(*assign_oper);
                    
                // mem rsp stack_loc
    
                L2::Item* mem = createL2Item("mem", L2::Type::mem);
                mem_alloc->items.push_back(*mem);
        
                L2::Item* rsp = createL2Item("rsp", L2::Type::reg);
                mem_alloc->items.push_back(*rsp);
        
                L2::Item* move_rsp_by = createL2Item(std::to_string(stack_loc), L2::Type::num);
                mem_alloc->items.push_back(*move_rsp_by);
        
                stack_loc -= 8;
                instructs_to_ret.push_back(mem_alloc);

            }
        }
    }

	Function* instruction_selection(Function* fp) {
		L2::Function* new_F = new L2::Function();
		new_F->name = fp->name;
		new_F->arguments = fp->arguments.size();
		new_F->locals = 0;

        std::vector<L2::Instruction*> callee_convention = enforce_callee_convention(Instruction_call* ip);
        for(auto &cci : callee_convention)
            new_F->instructions.push_back(cci);

		for (auto &ip : fp->instructions) {
			if(ip->Type == InstructionType::assign){
                L2::Instruction* i = new L2::Instruction();
                L2::Item* var = createL2Item(ip->items[0].labelName, L2::Type::var);
				i->items.push_back(*var);

				L2::Item* assign = createL2Item("<-", L2::Type::assign_oper);
				i->items.push_back(*assign);
                L2::Item* source = createL2Item(ip->items[1].labelName, L2::Type::num);
                i->items.push_back(*source);

				new_F->instructions.push_back(i);
			} else if(ip->Type == InstructionType::assign_arithmetic) {
                // TODO: IF VAR1 == VAR2 MAKE A TILE
                L2::Instruction* i = new L2::Instruction();
                L2::Item* var = createL2Item(ip->items[0].labelName, L2::Type::var);
				i->items.push_back(*var);

				L2::Item* assign = createL2Item("<-", L2::Type::assign_oper);
				i->items.push_back(*assign);

                L2::Item* source = createL2Item(ip->items[1].labelName, L2::Type::num);
                i->items.push_back(*source);
                
				new_F->instructions.push_back(i);

                L2::Instruction* i2 = new L2::Instruction();
                L2::Item* var2 = createL2Item(ip->items[0].labelName, L2::Type::var);
				i2->items.push_back(*var2);

                L2::Item* oper = createL2Item(ip->items[2].labelName + '=', L2::Type::var);
				i2->items.push_back(oper);
                
                L2::Item* source = createL2Item(ip->items[3].labelName, L2::Type::var);
				i2->items.push_back(*source);

				new_F->instructions.push_back(i2);
			} else if(ip->Type == InstructionType::assign_compare) { 
                L2::Instruction* i = new L2::Instruction();
                L2::Item* var = createL2Item(ip->items[0].labelName, L2::Type::var);
				i->items.push_back(*var);

				L2::Item* assign = createL2Item("<-", L2::Type::assign_oper);
				i->items.push_back(*assign);

				if(ip->items[2].Oper == Compare_Operator::geq || ip->items[2].Oper == Compare_Operator::gr) {
					ip->items[2].labelName.replace(0, 1, "<");
                    
                    L2::Item* var1 = createL2Item(ip->items[3].labelName, L2::Type::var);
                    L2::Item* oper = createL2Item(ip->items[2].labelName, L2::Type::var);
                    L2::Item* var2 = createL2Item(ip->items[1].labelName, L2::Type::var);
				} else { // as normal L2 comparisons
                    L2::Item* var1 = createL2Item(ip->items[1].labelName, L2::Type::var);
                    L2::Item* oper = createL2Item(ip->items[2].labelName, L2::Type::var);
                    L2::Item* var2 = createL2Item(ip->items[3].labelName, L2::Type::var);
				}

                i->items.push_back(*var1);
                i->items.push_back(*oper);
                i->items.push_back(*var2);

				new_F->instructions.push_back(i);
			} else if(ip->Type == InstructionType::assign_load) {
                L2::Instruction* i = new L2::Instruction();
                L2::Item* var = createL2Item(ip->items[0].labelName, L2::Type::var);
				i->items.push_back(*var);
                
				L2::Item* assign = createL2Item("<-", L2::Type::assign_oper);
				i->items.push_back(*assign);

				L2::Item* mem = createL2Item("mem", L2::Type::mem);
				i->items.push_back(*mem);
                L2::Item* source = createL2Item(ip->items[1].labelName, L2::Type::var);
				i->items.push_back(*source);
				//TODO memory should be calculated as more same var/reg are being loaded in a function
				L2::Item* number = createL2Item("0", L2::Type::number);
				i->items.push_back(*number);
				new_F->instructions.push_back(i);
			} else if(ip->Type == InstructionType::assign_store) {
                L2::Instruction* i = new L2::Instruction();
				L2::Item* mem = createL2Item("mem", L2::Type::compare_oper);
				i->items.push_back(*mem);
                L2::Item* var = createL2Item(ip->items[0].labelName, L2::Type::var);
				i->items.push_back(*var);
				//TODO same as above
				L2::Item* number = createL2Item("0", L2::Type::number);
				i->items.push_back(*number);
				L2::Item* assign = createL2Item("<-", L2::Type::assign_oper);
				i->items.push_back(*assign);
                L2::Item* source = createL2Item(ip->items[1].labelName, L2::Type::var);
				i->items.push_back(*source);
				new_F->instructions.push_back(i);
			} else if(ip->Type == InstructionType::return_empty) { 
                L2::Instruction* i = new L2::Instruction();
				L2::Item* item = new L2::Item();
				item->labelName = "return";
				i->items.push_back(*item);
				new_F->instructions.push_back(i);
			} else if(ip->Type == InstructionType::return_value) { 
                L2::Instruction* i = new L2::Instruction();
				L2::Item* reg = createL2Item("rax", L2::Type::reg);
				i->items.push_back(*reg);
				L2::Item* assign = createL2Item("<-", L2::Type::assign_oper);
				i->items.push_back(*assign);
                L2::Item* var = createL2Item(ip->items[0].labelName, L2::Type::var);
				i->items.push_back(*var);
				new_F->instructions.push_back(i);

                L2::Instruction* i2 = new L2::Instruction();
				L2::Item* item = new L2::Item();
				item->labelName = "return";
				i2->items.push_back(*item);
				new_F->instructions.push_back(i2);
			} else if(ip->Type == InstructionType::label) {
                L2::Instruction* i = new L2::Instruction();
                L2::Item* label_item = createL2Item(ip->items[0].labelName, L2::Type::var);
				i->items.push_back(*label_item);
				new_F->instructions.push_back(i);
			} else if(ip->Type == InstructionType::br_unconditional) {
                L2::Instruction* i = new L2::Instruction();
				L2::Item* item = new L2::Item();
				item->labelName = "goto";
				i->items.push_back(*item);
                L2::Item* label_item = createL2Item(ip->items[0].labelName, L2::Type::var);
				i->items.push_back(*label_item);
				new_F->instructions.push_back(i);
			} else if(ip->Type == InstructionType::br_conditional) {
                L2::Instruction* i = new L2::Instruction(); //TODO for tiling, the predecessor and cjump should beone tile 
				L2::Item* item = new L2::Item();
				item->labelName = "cjump";
				i->items.push_back(*item);
                L2::Item* var = createL2Item(ip->items[0].labelName, L2::Type::var);
				i->items.push_back(*var);
				L2::Item* compare = createL2Item("=", L2::Type::compare_oper);
				i2->items.push_back(*compare);
				L2::Item* item2 = new L2::Item();
				item2->labelName = "true";
				i->items.push_back(*item2);
                L2::Item* label_item = createL2Item(ip->items[1].labelName, L2::Type::var);
				i->items.push_back(*label_item);
				new_F->instructions.push_back(i);
			} else if(ip->Type == InstructionType::call) {
                std::vector<L2::Instruction*> callerConventionInstructions = enforce_caller_convention(Instruction_call* ip);
                for(auto &i : callerConventionInstructions)
                    new_F->instructions.push_back(i);
			} else {
                std::vector<L2::Instruction*> callerConventionInstructions = enforce_caller_convention(Instruction_call* ip);
                for(auto &cci : callerConventionInstructions)
                    new_F->instructions.push_back(cci);

                // rax
				Instruction* i = new L2::Instruction();
                L2::Item* varToPopulate = createL2Item(ip->items[0].labelName, L2::Type::var);
				i->items.push_back(*varToPopulate);

                L2::Item* assign_oper = createL2Item("<-", L2::Type::assign_oper);
                mem_alloc->items.push_back(*assign_oper);

				L2::Item* reg = createL2Item("rax", L2::Type::reg);
				i->items.push_back(*reg);
				new_F->instructions.push_back(i);
			}

		    return new_F;
            }
        }
}
