#include <iostream>
#include <string>
#include <analysis.h>

namespace L3 {

   void print_set(set_of_str &s) {
       for(std::string const i : s)
           std::cout << i  << ' ';
       std::cout << '\n';
   }

   void print_instruction(Instruction &instruction) {
        for(Item* i : instruction.Items) {
            std::cout << i->labelName << ' ';
        }
        std::cout << '\n';
   }

   set_of_str subtract_sets(set_of_str s1, set_of_str s2) { // out - kill
        std::vector<std::string> toDelete;

        for(auto &i : s1) {
            set_of_str::const_iterator item = s2.find(i);
            if(item != s2.end()) { // if item is in s2
                toDelete.push_back(i);
            }
        }

        for(auto &i : toDelete){
            set_of_str::const_iterator item = s1.find(i);
            s1.erase(item);
        }

        return s1;
   }

    bool isVar(Item* i) { return i->Type == Atomic_Type::var; }

    void init_all_sets(Function* fp) {
        for(auto i : fp->instructions) {
            i->gen_set.clear();
            i->kill_set.clear();
            i->in_set.clear();
            i->out_set.clear();
        }
    }
    /*
     * Gets the function and the index of the instruction of which the successor
     * needs to be found.
     */
//    void goto_successor_push_back(Function &f, int currInstructionIndex) {
//        Instruction* currI = f.instructions[currInstructionIndex];
//        std::string labelToMatch = currI->Items[1].labelName;
//        for(int n = 0; n < f.instructions.size(); ++n) {
//            Instruction* i = f.instructions[n];
//            if(i->identifier != 10) // if not label_instruction
//                continue;
//            else {
//                if(i->items[0].labelName == labelToMatch)
//                    currI->successors.push_back(f.instructions[n+1]);
//                else
//                    continue;
//            }
//        }
//    }

    /*
     * Gets the function pointer and the index of the instruction of which the successor
     * needs to be found.
     */
    void find_br_unconditional_successor(Function* f, int currInstructionIndex) {
        Instruction* prevI = f->instructions[currInstructionIndex];
        std::string labelToMatch = prevI->Items[1]->labelName;
        for(int n = 0; n < f->instructions.size(); ++n) {
            Instruction* i = f->instructions[n];
            if(i->Type != InstructionType::label) // if not label_instruction
                continue;
            else {
                if(i->Items[0]->labelName == labelToMatch) {
                    prevI->successors.push_back(f->instructions[n+1]); // push back the instruction after the label
                    //std::cout << n << " successor found\t";
                    //print_instruction(*f.instructions[n+1]);
                }
                else
                    continue;
            }
        }
    }
        
    void find_br_conditional_successor(Function* f, int currInstructionIndex) {
        Instruction* prevI = f->instructions[currInstructionIndex];
        std::string labelToMatch = prevI->Items[1]->labelName;
        for(int n = 0; n < f->instructions.size(); ++n) {
            Instruction* i = f->instructions[n];
            if(i->Type != InstructionType::label)
                continue;
            else {
                if(i->Items[0]->labelName == labelToMatch) {
                    prevI->successors.push_back(f->instructions[n+1]); // push back the instruction after the label
                    //std::cout << n << " successor found\t";
                    //print_instruction(*f.instructions[n+1]);
                }
                else
                    continue;
            }
        }

        
    }
//    void cjump_twoargs_successor_push_back(Function &f, int currInstructionIndex) {
//        Instruction* prevI = f.instructions[currInstructionIndex];
//        std::string label1 = prevI->items[4].labelName;
//        std::string label2 = prevI->items[5].labelName;
//        for(int n = 0; n < f.instructions.size(); ++n) {
//            Instruction* i = f.instructions[n];
//            if(i->identifier != 10) // if not label_instruction
//                continue;
//            else {
//                if(i->items[0].labelName == label1 || i->items[0].labelName == label2)
//                    prevI->successors.push_back(f.instructions[n+1]);
//                else
//                    continue;
//            }
//        }
//    }

    void gk_assign(Instruction* i) {
        set_of_str gen_set = i->gen_set;
        set_of_str kill_set = i->kill_set;


        if(isVar(i->Items[1]))
            gen_set.emplace(i->Items[1]->labelName);

        kill_set.emplace(i->Items[0]->labelName);

        i->gen_set = gen_set;
        i->kill_set = kill_set;
    }

    void gk_assign_comparison_and_assign_arithmetic(Instruction* i) {
        if(isVar(i->Items[1]))
            i->gen_set.emplace(i->Items[1]->labelName); //arg1
        if(isVar(i->Items[2]))
            i->gen_set.emplace(i->Items[2]->labelName); //arg2

        i->kill_set.emplace(i->Items[0]->labelName); //location
    }

    void gk_call_assign(Instruction* i) {
        // callee
        if(isVar(i->Items[1])) {
            i->gen_set.emplace(i->Items[1]->labelName);
        }

        // destination
        if(isVar(i->Items[0])) {
            i->gen_set.emplace(i->Items[0]->labelName);
        }

        for (int inum = 0; inum < i->arguments.size(); inum++) {
            if(isVar(i->arguments[inum]))
                i->gen_set.emplace(i->arguments[inum]->labelName);
        }

    }

    void gk_assign_load(Instruction* i) {
        i->gen_set.emplace(i->Items[0]->labelName);
        i->kill_set.emplace(i->Items[1]->labelName);
    }

    void gk_assign_store(Instruction* i) {
        i->kill_set.emplace(i->Items[0]->labelName);
        if(isVar(i->Items[1]))
            i->gen_set.emplace(i->Items[1]->labelName);
    }

    void gk_return_empty(Instruction* i) {
        // return has no gen or kill set
    }

    void gk_return_value(Instruction* i) {
        set_of_str gen_set = i->gen_set;
        if(isVar(i->Items[0]))
            gen_set.emplace(i->Items[0]->labelName);
    }


    void gk_br_conditional(Instruction* i) {
        i->gen_set.emplace(i->Items[0]->labelName); // var
    }

    void gk_call(Instruction* i) {
        // callee
        if(isVar(i->Items[1])) {
            i->gen_set.emplace(i->Items[1]->labelName);
        }

        for (int inum = 0; inum < i->arguments.size(); inum++) {
            if(isVar(i->arguments[inum]))
                i->gen_set.emplace(i->arguments[inum]->labelName);
        }
    }

//    void gk_br_unconditional(Instruction* i) {
//    }


//    void gk_stack_arg(Instruction& i) {
//        i.kill_set.emplace(varNameModifier(i.items[0]));
//    }

    void compute_gen_and_kill(Instruction* instruct) {
        if(instruct->Type == InstructionType::assign) 
            gk_assign(instruct);
         else if(instruct->Type == InstructionType::assign_compare)
            gk_assign_comparison_and_assign_arithmetic(instruct);
         else if(instruct->Type == InstructionType::assign_arithmetic)
            gk_assign_comparison_and_assign_arithmetic(instruct);
         else if(instruct->Type == InstructionType::assign_load)
            gk_assign_load(instruct);
         else if(instruct->Type == InstructionType::assign_store)
            gk_assign_store(instruct);
         else if(instruct->Type == InstructionType::return_value)
            gk_return_value(instruct);
         else if(instruct->Type == InstructionType::return_empty)
            gk_return_empty(instruct);
//         else if(iinstruct->Type == InstructionType::label)
//            gk_label(instruct);
//         else if(iinstruct->Type == InstructionType::br_unconditional)
//            gk_br_unconditional(instruct);
         else if(instruct->Type == InstructionType::br_conditional)
            gk_br_conditional(instruct);
         else if(instruct->Type == InstructionType::call)
            gk_call(instruct);
         else
            gk_call_assign(instruct);

    }

    void compute_in_and_out(Function* f) {
        // computing in and out set going from last to first instruction
        int num_instructions = f->instructions.size();
        bool not_converged;
        set_of_str prev_out_set;
        set_of_str prev_in_set;

        do{
			not_converged = false;
            // compute IN set of last instruction; has no successor or OUT set
            Instruction* ip = f->instructions[num_instructions - 1];
            ip->in_set = ip->gen_set;

//            std::cout << "\nIN and OUT sets for instruction\n";
//            std::cout << "IN set: ";
//            print_set(ip->in_set);
//            std::cout << "OUT set: ";
//            print_set(ip->out_set);

            // starting from second to last instruction to first instruction, assign successor(s)
            for(int n = num_instructions - 2; n >= 0; --n) {
                ip = f->instructions[n];
                if(ip->Type == InstructionType::br_unconditional) { // push_back successor if goto 
                   find_br_unconditional_successor(f, n); 
                }
                else if(ip->Type == InstructionType::br_conditional) { // push_back successor if cjump_onearg
                        find_br_conditional_successor(f, n);
                } 
                else
                    ip->successors.push_back(f->instructions[n+1]);

                prev_out_set = ip->out_set;
                prev_in_set = ip->in_set;
                
                //std::cout << "curr out_set: ";
                //print_set(ip->out_set);
                // foreach successor, insert successor's IN set into currI's OUT set (populate out set)
                //ip->out_set.clear();
                if(ip->Type != InstructionType::return_empty || ip->Type != InstructionType::return_value) {
                    for(auto sp : ip->successors) { // return should have no out set
                        (ip->out_set).insert(sp->in_set.begin(), sp->in_set.end());
                    //std::cout << "an in_set placed into curr out_set: ";
                    //print_set(sp->in_set);
                    }
                }
                //std::cout << "out_set after insertions: ";
                //print_set(ip->out_set);
                // populate in set
//                std::cout << "OUT and KILL sets\n";
//                std::cout << "OUT set: ";
//                print_set(ip->out_set);
//                std::cout << "KILL set: ";
//                print_set(ip->kill_set);
                set_of_str out_sub_kill = subtract_sets(ip->out_set, ip->kill_set); // OUT[i] - KILL[i]
                ip->in_set.clear();
                ip->in_set.insert(out_sub_kill.begin(), out_sub_kill.end());
                ip->in_set.insert(ip->gen_set.begin(), ip->gen_set.end());

                if(prev_out_set != ip->out_set || prev_in_set != ip->in_set) {
                    not_converged = true;
                }
//            std::cout << "\nIN and OUT sets for instruction\n";
//            std::cout << "IN set: ";
//            print_set(ip->in_set);
//            std::cout << "OUT set: ";
//            print_set(ip->out_set);
//            std::cout << '\n';
            }
        } while(not_converged);

    }

    void analyze(Function* fp) {

        init_all_sets(fp);
        //std::cout << "Function: " << f.name << '\n';
        for(Instruction* instruct : fp->instructions) {
            compute_gen_and_kill(instruct);
            // print gen and kill for each instruction
//            std::cout << "instruction: \ngen_set: ";
//            print_set(instruct->gen_set);
//            std::cout << "kill_set: ";
//            print_set(instruct->kill_set);         
//            std::cout << '\n';
	    }
        
        compute_in_and_out(fp);

//        if(liveness_only) {
//            std::cout << "(\n(in\n";
//            for(auto i : f.instructions) {
//                std::cout << " (";
//                for(auto s : i->in_set) {
//                    std::cout << s << ' ';
//                }
//                std::cout << ")\n";
//            }
//            std::cout << ")\n\n(out\n";
//            for(auto i : f.instructions) {
//                std::cout << " (";
//                for(auto s : i->out_set)
//                    std::cout << s << ' ';
//                std::cout << ")\n";
//            }
//    		std::cout << ")\n\n)";
//        }

    }


}
