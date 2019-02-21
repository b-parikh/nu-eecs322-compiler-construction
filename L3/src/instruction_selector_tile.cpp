#include <string>
#include <iostream>
#include <instruction_selector.h>
#include <tree.h>
#include <context.h>

namespace L3 {

//    L2::Item* createL2Item(std::string labelName, L2::Type aType) {
//        L2::Item* item = new L2::Item();
//        item->labelName = labelName;
//        item->type = aType;
//
//        return item;
//    }

//    L2::Item* choose_Arith_Oper(Instruction* ip){
//        L2::Item* oper;
//        if(ip->Arith_Oper == Arith_Operator::shift_left)
//            oper = createL2Item("<<=", L2::Type::var);
//        else if(ip->Arith_Oper == Arith_Operator::shift_right)
//            oper = createL2Item(">>=", L2::Type::var);
//        else if(ip->Arith_Oper== Arith_Operator::plus)
//            oper = createL2Item("+=", L2::Type::var);
//        else if(ip->Arith_Oper== Arith_Operator::minus)
//            oper = createL2Item("-=", L2::Type::var);
//        else if(ip->Arith_Oper == Arith_Operator::multiply)
//            oper = createL2Item("*=", L2::Type::var);
//        else if(ip->Arith_Oper == Arith_Operator::bw_and)
//            oper = createL2Item("&=", L2::Type::var);
//
//        return oper;
//    }

//    void add_arguments(Instruction* ip, std::vector<L2::Instruction*>& instructs_to_ret, int& stack_loc) {
//        std::vector<std::string> ARG_REGISTERS = {"rdi", "rsi", "rdx", "rcx", "r8", "r9"};
//
//        int num_args_in_reg = ip->arguments.size() < 6 ? ip->arguments.size() : 6;
//        for(int j = 0; j < num_args_in_reg; ++j) {
//            auto currArgument = ip->arguments[j];
//            L2::Instruction* into_reg = new L2::Instruction();
//            L2::Item* reg = createL2Item(ARG_REGISTERS[j], L2::Type::reg);
//            into_reg->items.push_back(*reg);
//            
//            L2::Item* assign_oper = createL2Item("<-", L2::Type::assign_oper);
//            into_reg->items.push_back(*assign_oper);
//            
//            L2::Item* arg;
//            if(currArgument->Type == Atomic_Type::num)
//                arg = createL2Item(ip->arguments[j]->labelName, L2::Type::num);
//            else // arg is var
//                arg = createL2Item(ip->arguments[j]->labelName, L2::Type::var);
//
//            into_reg->items.push_back(*arg);
//            instructs_to_ret.push_back(into_reg);
//        }
//
//        // store rest of args on stack
//        if(ARG_REGISTERS.size() < ip->arguments.size()) {
//            for(int j = ARG_REGISTERS.size(); j < ip->arguments.size(); ++j) {
//                // mem rsp stack_loc
//                L2::Instruction* mem_alloc = new L2::Instruction();
//    
//                L2::Item* mem = createL2Item("mem", L2::Type::mem);
//                mem_alloc->items.push_back(*mem);
//        
//                L2::Item* rsp = createL2Item("rsp", L2::Type::reg);
//                mem_alloc->items.push_back(*rsp);
//
//                L2::Item* move_rsp_by = createL2Item(std::to_string(stack_loc), L2::Type::num);
//                mem_alloc->items.push_back(*move_rsp_by);
//        
//                stack_loc -= 8;
//
//                L2::Item* assign_oper = createL2Item("<-", L2::Type::assign_oper);
//                mem_alloc->items.push_back(*assign_oper);
//                
//                L2::Item* arg;
//                if(ip->arguments[j]->Type == Atomic_Type::num)
//                    arg = createL2Item(ip->arguments[j]->labelName, L2::Type::num);
//                else // arg is var
//                    arg = createL2Item(ip->arguments[j]->labelName, L2::Type::var);
//            
//                mem_alloc->items.push_back(*arg);
//
//                instructs_to_ret.push_back(mem_alloc);
//            }
//        }
//    }
//
//    std::vector<L2::Instruction*> enforce_runtime_caller_conv(Instruction* ip, std::string runtimeFuncName) {
//        std::vector<L2::Instruction*> instructs_to_ret;
//        int stack_loc = 0; // no stack_loc reqd but doing this bc pass by reference
//        
//        add_arguments(ip, instructs_to_ret, stack_loc);
//
//        L2::Instruction* call_instruct = new L2::Instruction();
//        L2::Item* call = createL2Item("call", L2::Type::runtime);
//        L2::Item* func_name = createL2Item(runtimeFuncName, L2::Type::runtime);
//        L2::Item* numargs = createL2Item(std::to_string(ip->arguments.size()), L2::Type::num);
//        call_instruct->items.push_back(*call);
//        call_instruct->items.push_back(*func_name);
//        call_instruct->items.push_back(*numargs);
//
//        instructs_to_ret.push_back(call_instruct);
//        return instructs_to_ret;
//    }
//
//    std::vector<L2::Instruction*> enforce_caller_convention(Instruction* ip, std::string callerName, int& return_label_count) {
//        std::vector<L2::Instruction*> instructs_to_ret;
//
//        int stack_loc = -8;
//        L2::Instruction* mem_alloc = new L2::Instruction();
//
//        L2::Item* mem = createL2Item("mem", L2::Type::mem);
//        mem_alloc->items.push_back(*mem);
//
//        L2::Item* rsp = createL2Item("rsp", L2::Type::reg);
//        mem_alloc->items.push_back(*rsp);
//
//        L2::Item* move_rsp_by = createL2Item(std::to_string(stack_loc), L2::Type::num);
//        mem_alloc->items.push_back(*move_rsp_by);
//        stack_loc -= 8;
//
//        L2::Item* assign_oper = createL2Item("<-", L2::Type::assign_oper);
//        mem_alloc->items.push_back(*assign_oper);
//
//        // the last item in the instruction is the function being called
//        std::string ret_label = ip->Items[ip->Items.size() -1]->labelName;
//        ret_label.erase(0,1);
//        ret_label = ':' + ret_label;
//        ret_label = ret_label + '_' + callerName + '_' + std::to_string(return_label_count);
//        return_label_count++;
//        L2::Item* ret_label_item = createL2Item(ret_label, L2::Type::label);
//        mem_alloc->items.push_back(*ret_label_item);
//
//        instructs_to_ret.push_back(mem_alloc);
//
//        // store arguments in registers and maybe to the stack
//        add_arguments(ip, instructs_to_ret, stack_loc);
//
//
//        L2::Instruction* call_I = new L2::Instruction();
//        L2::Item* call = new L2::Item();
//        call->labelName = "call";
//        call->type = L2::Type::num;
//
//        L2::Item* fToCall = new L2::Item();
//        fToCall->labelName = ip->Items[ip->Items.size() - 1]->labelName;
//        fToCall->type = L2::Type::label;
//
//        L2::Item* numArgs = new L2::Item();
//        numArgs->labelName = std::to_string(ip->arguments.size());
//        numArgs->type = L2::Type::num;
//
//        call_I->items.push_back(*call);
//        call_I->items.push_back(*fToCall);
//        call_I->items.push_back(*numArgs);
//
//        instructs_to_ret.push_back(call_I);
//
//        L2::Instruction* ret_label_i = new L2::Instruction();
//        ret_label_i->items.push_back(*ret_label_item); // from previous use of return label
//
//        instructs_to_ret.push_back(ret_label_i);
//
//        return instructs_to_ret;
//    }
//
//
//    std::vector<L2::Instruction*> enforce_callee_convention(std::vector<Item*> arguments) {
//        std::vector<std::string> ARG_REGISTERS = {"rdi", "rsi", "rdx", "rcx", "r8", "r9"};
//
//        std::vector<L2::Instruction*> instructs_to_ret;
//        // add arguments
//
//        int num_args_in_reg = arguments.size() < 6 ? arguments.size() : 6;
//        for(int j = 0; j < num_args_in_reg; ++j) {
//            auto currArgument = arguments[j];
//            L2::Instruction* into_reg = new L2::Instruction();
//
//            L2::Item* arg;
//            arg = createL2Item(arguments[j]->labelName, L2::Type::var);
//            into_reg->items.push_back(*arg);
//
//            L2::Item* assign_oper = createL2Item("<-", L2::Type::assign_oper);
//            into_reg->items.push_back(*assign_oper);
//
//            L2::Item* reg = createL2Item(ARG_REGISTERS[j], L2::Type::reg);
//            into_reg->items.push_back(*reg);
//            
//            instructs_to_ret.push_back(into_reg);
//
//        }
//        int stack_loc = 8*(arguments.size() - ARG_REGISTERS.size() - 1);
//        if(ARG_REGISTERS.size() < arguments.size()) {
//            for(int j = ARG_REGISTERS.size(); j < arguments.size(); ++j) {
//                L2::Instruction* mem_alloc = new L2::Instruction();
//                
//                L2::Item* arg;
//                if(arguments[j]->Type == Atomic_Type::num)
//                    arg = createL2Item(arguments[j]->labelName, L2::Type::num);
//                else // arg is var
//                    arg = createL2Item(arguments[j]->labelName, L2::Type::var);
//            
//                mem_alloc->items.push_back(*arg);
//                
//                L2::Item* assign_oper = createL2Item("<-", L2::Type::assign_oper);
//                mem_alloc->items.push_back(*assign_oper);
//                    
//                // mem rsp stack_loc
//    
//                L2::Item* stack_arg = createL2Item("stack-arg", L2::Type::mem);
//                mem_alloc->items.push_back(*stack_arg);
//        
//                L2::Item* move_rsp_by = createL2Item(std::to_string(stack_loc), L2::Type::num);
//                mem_alloc->items.push_back(*move_rsp_by);
//        
//                stack_loc -= 8;
//                instructs_to_ret.push_back(mem_alloc);
//
//            }
//        }
//        return instructs_to_ret;
//    }
//
//    std::vector<L2::Item*> compare_operator_to_L2(Instruction* ip) {
//        L2::Item* var1;
//        L2::Item* oper;
//        L2::Item* var2;
//
//        std::vector<L2::Item*> to_return;
//		if(ip->Comp_Oper == Compare_Operator::geq) { 
//            //std::cerr << "first";
//            var1 = createL2Item(ip->Items[2]->labelName, L2::Type::var);
//            oper = createL2Item("<=", L2::Type::var);
//            var2 = createL2Item(ip->Items[1]->labelName, L2::Type::var);
//        } else if(ip->Comp_Oper == Compare_Operator::gr) {
//            //std::cerr << "second";
//            var1 = createL2Item(ip->Items[2]->labelName, L2::Type::var);
//            oper = createL2Item("<", L2::Type::var);
//            var2 = createL2Item(ip->Items[1]->labelName, L2::Type::var);
//		} else if(ip->Comp_Oper == Compare_Operator::le) { // as normal L2 comparisons
//            //std::cerr << "third";
//            var1 = createL2Item(ip->Items[1]->labelName, L2::Type::var);
//            oper = createL2Item("<", L2::Type::var);
//            var2 = createL2Item(ip->Items[2]->labelName, L2::Type::var);
//		} else if(ip->Comp_Oper == Compare_Operator::leq) { // as normal L2 comparisons
//            //std::cerr << "fourth";
//            var1 = createL2Item(ip->Items[1]->labelName, L2::Type::var);
//            oper = createL2Item("<=", L2::Type::var);
//            var2 = createL2Item(ip->Items[2]->labelName, L2::Type::var);
//		} else if(ip->Comp_Oper == Compare_Operator::eq) { // as normal L2 comparisons
//            //std::cerr << "fifth";
//            var1 = createL2Item(ip->Items[1]->labelName, L2::Type::var);
//            oper = createL2Item("=", L2::Type::var);
//            var2 = createL2Item(ip->Items[2]->labelName, L2::Type::var);
//		} else if(ip->Arith_Oper == Arith_Operator::bw_and) { // as normal L2 comparisons
//            //std::cerr << "fifth";
//            var1 = createL2Item(ip->Items[1]->labelName, L2::Type::var);
//            oper = createL2Item("=", L2::Type::var);
//            var2 = createL2Item(ip->Items[2]->labelName, L2::Type::var);
//		} else // This is an error
//			std::cerr << "Incorrect comparison operator\n";
//
//        to_return.push_back(var1);
//        to_return.push_back(oper);
//        to_return.push_back(var2);
//
//        return to_return;
//    }
//
//    std::vector<Context*> identify_contexts (Function* fp) {
//		std::vector<Context*> all_contexts;
//        Context* c = new Context();
//		for (auto &ip : fp->instructions) {
//			if(ip->Type == InstructionType::label && !(c->Instructions.empty())) {
//                all_contexts.push_back(c);
//                c = new Context();
//			}
//			c->Instructions.push_back(ip);
//			if(ip->Type == InstructionType::br_unconditional || ip->Type == InstructionType::br_conditional) {
//				all_contexts.push_back(c);
//                c = new Context();
//			}
//		}
//        return all_contexts;
//	}

//	// TODO: complete this func
//	std::vector<Tree*> generate_trees(std::vector<std::vector<Instruction*>> context) {
//		std::vector<Tree*> trees;
//		for(auto &i : context) {
//			// TODO initalize the tree
//			for(auto &item : i->Items) {
//				// TODO: adds nodes
//			}
//		}
//
//		return trees;
//	}
//
//	// TODO: complete this func
//	std::vector<Tree*> merge_trees(std::vector<Tree*> forest) {
//		std::vector<Tree*> trees_ret;
//		for(auto &tree : forest) {
//			// TODO merge trees with rules
//			}
//		}
//
//		return trees_ret;
//	}
//
//	// TODO: complete this func
//	std::vector<L2::Instruction*> tile_instruction(std::vector<Tree*> trees) {
//		std::vector<L2::Instruction*> l2_instructions;
//		for(auto &tree : trees) {
//			// TODO generate L2 instructions with pre-defined tiles
//            L2::Instruction* i = new L2::Instruction();
//
//			}
//		}
//
//		return l2_instructions;
//	}

//	L2::Function* instruction_selection(Function* fp) {
//		L2::Function* new_F = new L2::Function();
//		new_F->name = fp->name;
//		new_F->arguments = fp->arguments.size();
//		new_F->locals = 0;
//
//        std::string callerName = new_F->name;
//        callerName.erase(0,1);
//        int return_label_count = 0;
//
//		// Callee convention
//        std::vector<L2::Instruction*> callee_convention = enforce_callee_convention(fp->arguments);
//        for(auto &cci : callee_convention){
//            new_F->instructions.push_back(cci);
//        }

		// 1. Go thru all instruction to define Contexts
//		std::vector<Context*> all_contexts = identify_contexts(fp);
//
//		// 2. gerenate trees, merge those, and tile them to create L2 instructions
//		for (auto &context : contexts) {
//			std::vector<Tree*> forest = generate_trees(context);
//
//			std::vector<Tree*> merged_trees = merge_trees(forest);
//
//			for (auto &tree : merged_trees) {
//				std::vector<L2::Instrunction*> l2_instructions = tile_instruction(tree);
//
//				for(auto &l2 : l2_instructions)
//					new_F->instructions.push_back(l2);
//			}
//		}
//
//		return new_F


		// TODO: Following can be used in tiling. Remove this after tiliing
//		for (auto &ip : fp->instructions) {
//			if(ip->Type == InstructionType::assign){
//                L2::Instruction* i = new L2::Instruction();
//                //std::cerr << "assign\n";
//                L2::Item* var = createL2Item(ip->Items[0]->labelName, L2::Type::var);
//				i->items.push_back(*var);
//
//				L2::Item* assign = createL2Item("<-", L2::Type::assign_oper);
//				i->items.push_back(*assign);
//                L2::Item* source = createL2Item(ip->Items[1]->labelName, L2::Type::num);
//                i->items.push_back(*source);
//
//				new_F->instructions.push_back(i);
//			} else if(ip->Type == InstructionType::assign_arithmetic) {
//                //std::cerr << "assign_arith\n";
//                // TODO: IF VAR1 == VAR2 MAKE A TILE
//                L2::Instruction* i = new L2::Instruction();
//                if(ip->Items[0]->labelName == ip->Items[1]->labelName) {
//                    L2::Item* oper = choose_Arith_Oper(ip);
//                    L2::Item* source = createL2Item(ip->Items[0]->labelName, L2::Type::var);
//                    L2::Item* operand = createL2Item(ip->Items[2]->labelName, L2::Type::var);
//                    i->items.push_back(*source);
//                    i->items.push_back(*oper);
//                    i->items.push_back(*operand);
//
//                    new_F->instructions.push_back(i);
//                }
//
//                else if (ip->Items[0]->labelName == ip->Items[2]->labelName) {
//                    L2::Item* oper = choose_Arith_Oper(ip);
//                    L2::Item* source = createL2Item(ip->Items[0]->labelName, L2::Type::var);
//                    L2::Item* operand = createL2Item(ip->Items[1]->labelName, L2::Type::var);
//                    i->items.push_back(*source);
//                    i->items.push_back(*oper);
//                    i->items.push_back(*operand);
//
//                    new_F->instructions.push_back(i);
//                }
//                else {
//                    L2::Item* var = createL2Item(ip->Items[0]->labelName, L2::Type::var);
//				    i->items.push_back(*var);
//
//				    L2::Item* assign = createL2Item("<-", L2::Type::assign_oper);
//				    i->items.push_back(*assign);
//
//                    L2::Item* source = createL2Item(ip->Items[1]->labelName, L2::Type::num);
//                    i->items.push_back(*source);
//                
//				    new_F->instructions.push_back(i);
//
//                    L2::Instruction* i2 = new L2::Instruction();
//                    L2::Item* var2 = createL2Item(ip->Items[0]->labelName, L2::Type::var);
//    				i2->items.push_back(*var2);
//    
//                    L2::Item* oper = choose_Arith_Oper(ip);
//    
//    				i2->items.push_back(*oper);
//                    
//                    L2::Item* source2 = createL2Item(ip->Items[2]->labelName, L2::Type::var);
//    				i2->items.push_back(*source2);
//    
//    				new_F->instructions.push_back(i2);
//                }
//			} else if(ip->Type == InstructionType::assign_compare) { 
//                //std::cerr << "assign_comp\n";
//                L2::Instruction* i = new L2::Instruction();
//                L2::Item* var = createL2Item(ip->Items[0]->labelName, L2::Type::var);
//				i->items.push_back(*var);
//
//				L2::Item* assign = createL2Item("<-", L2::Type::assign_oper);
//				i->items.push_back(*assign);
//
//                std::vector<L2::Item*> var_op_var = compare_operator_to_L2(ip);
//
//                i->items.push_back(*(var_op_var[0]));
//                i->items.push_back(*(var_op_var[1]));
//                i->items.push_back(*(var_op_var[2]));
//
//				new_F->instructions.push_back(i);
//
//			} else if(ip->Type == InstructionType::assign_load) {
//                L2::Instruction* i = new L2::Instruction();
//                //std::cerr << "assign_load\n";
//                L2::Item* var = createL2Item(ip->Items[0]->labelName, L2::Type::var);
//				i->items.push_back(*var);
//                
//				L2::Item* assign = createL2Item("<-", L2::Type::assign_oper);
//				i->items.push_back(*assign);
//
//				L2::Item* mem = createL2Item("mem", L2::Type::mem);
//				i->items.push_back(*mem);
//                L2::Item* source = createL2Item(ip->Items[1]->labelName, L2::Type::var);
//				i->items.push_back(*source);
//				//TODO memory should be calculated as more same var/reg are being loaded in a function
//				L2::Item* number = createL2Item("0", L2::Type::num);
//				i->items.push_back(*number);
//				new_F->instructions.push_back(i);
//			} else if(ip->Type == InstructionType::assign_store) {
//                L2::Instruction* i = new L2::Instruction();
//                //std::cerr << "assign_store\n";
//				L2::Item* mem = createL2Item("mem", L2::Type::compare_oper);
//				i->items.push_back(*mem);
//                L2::Item* var = createL2Item(ip->Items[0]->labelName, L2::Type::var);
//				i->items.push_back(*var);
//				//TODO same as above
//				L2::Item* number = createL2Item("0", L2::Type::num);
//				i->items.push_back(*number);
//				L2::Item* assign = createL2Item("<-", L2::Type::assign_oper);
//				i->items.push_back(*assign);
//                L2::Item* source = createL2Item(ip->Items[1]->labelName, L2::Type::var);
//				i->items.push_back(*source);
//				new_F->instructions.push_back(i);
//			} else if(ip->Type == InstructionType::return_empty) { 
//                L2::Instruction* i = new L2::Instruction();
//                //std::cerr << "return empty\n";
//				L2::Item* item = new L2::Item();
//				item->labelName = "return";
//				i->items.push_back(*item);
//				new_F->instructions.push_back(i);
//			} else if(ip->Type == InstructionType::return_value) { 
//                L2::Instruction* i = new L2::Instruction();
//                //std::cerr << "return value\n";
//				L2::Item* reg = createL2Item("rax", L2::Type::reg);
//				i->items.push_back(*reg);
//				L2::Item* assign = createL2Item("<-", L2::Type::assign_oper);
//				i->items.push_back(*assign);
//                L2::Item* var = createL2Item(ip->Items[0]->labelName, L2::Type::var);
//				i->items.push_back(*var);
//				new_F->instructions.push_back(i);
//
//                L2::Instruction* i2 = new L2::Instruction();
//				L2::Item* item = new L2::Item();
//				item->labelName = "return";
//				i2->items.push_back(*item);
//				new_F->instructions.push_back(i2);
//			} else if(ip->Type == InstructionType::label) {
//                L2::Instruction* i = new L2::Instruction();
//                //std::cerr << "label\n";
//                L2::Item* label_item = createL2Item(ip->Items[0]->labelName, L2::Type::var);
//				i->items.push_back(*label_item);
//				new_F->instructions.push_back(i);
//			} else if(ip->Type == InstructionType::br_unconditional) {
//                //std::cerr << "br_uncond\n";
//                L2::Instruction* i = new L2::Instruction();
//				L2::Item* item = new L2::Item();
//				item->labelName = "goto";
//				i->items.push_back(*item);
//                L2::Item* label_item = createL2Item(ip->Items[0]->labelName, L2::Type::var);
//				i->items.push_back(*label_item);
//				new_F->instructions.push_back(i);
//			} else if(ip->Type == InstructionType::br_conditional) {
//                //std::cerr << "br_cond\n";
//                L2::Instruction* i = new L2::Instruction(); //TODO for tiling, the predecessor and cjump should beone tile 
//				L2::Item* item = new L2::Item();
//				item->labelName = "cjump";
//				i->items.push_back(*item);
//                
//                Instruction* ip_pred = ip->predecessor;
//                std::vector<L2::Item*> var_op_var = compare_operator_to_L2(ip_pred);
//                i->items.push_back(*(var_op_var[0]));
//                i->items.push_back(*(var_op_var[1]));
//                i->items.push_back(*(var_op_var[2]));
//
//                L2::Item* label_item = createL2Item(ip->Items[1]->labelName, L2::Type::var);
//				i->items.push_back(*label_item);
//				new_F->instructions.push_back(i);
//			} else if(ip->Type == InstructionType::call) {
//                std::vector<L2::Instruction*> callerConventionInstructions;
//                if(ip->calleeType == CalleeType::print)
//                     callerConventionInstructions = enforce_runtime_caller_conv(ip, "print");
//                else if(ip->calleeType == CalleeType::allocate)
//                   callerConventionInstructions = enforce_runtime_caller_conv(ip, "allocate");
//                else if(ip->calleeType == CalleeType::array_error)
//                    callerConventionInstructions = enforce_runtime_caller_conv(ip, "array-error");
//                else
//                    callerConventionInstructions = enforce_caller_convention(ip, callerName, return_label_count);
//                //std::cerr << "caller convention call\n";
//                for(auto &i : callerConventionInstructions)
//                    new_F->instructions.push_back(i);
//			} else { // call assign
//                std::vector<L2::Instruction*> callerConventionInstructions;
//                if(ip->calleeType == CalleeType::print)
//                   callerConventionInstructions = enforce_runtime_caller_conv(ip, "print");
//                else if(ip->calleeType == CalleeType::allocate)
//                    callerConventionInstructions = enforce_runtime_caller_conv(ip, "allocate");
//                else if(ip->calleeType == CalleeType::array_error)
//                    callerConventionInstructions = enforce_runtime_caller_conv(ip, "array-error");
//                else 
//                    callerConventionInstructions = enforce_caller_convention(ip, callerName, return_label_count);
//                for(auto &cci : callerConventionInstructions)
//                    new_F->instructions.push_back(cci);
//
//                //std::cerr << "caller convention call assign\n";
//                // rax
//				L2::Instruction* i = new L2::Instruction();
//                L2::Item* varToPopulate = createL2Item(ip->Items[0]->labelName, L2::Type::var);
//				i->items.push_back(*varToPopulate);
//
//                L2::Item* assign_oper = createL2Item("<-", L2::Type::assign_oper);
//                i->items.push_back(*assign_oper);
//
//				L2::Item* reg = createL2Item("rax", L2::Type::reg);
//				i->items.push_back(*reg);
//				new_F->instructions.push_back(i);
//			}
//
//        }
//		return new_F;
//	}
}
