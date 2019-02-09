#include <spiller.h>
#include <iostream>

namespace L2 {

    void addItemToInstruction(Instruction &i, std::string labelName, l2::Type type) {
        Item it;
        it.labelName = labelName;
        it.type = type;
        i.items.push_back(it);
    }

    void print_function_meta(Function &new_F, bool spill_var_found, Function f) {
        int numLocals = f.locals + (spill_var_found ? 1 : 0);
        //std::cout << "f.locals: " << f.locals << " numLocals: " << numLocals << ' ';
        //std::cout << f.arguments << ' ' << numLocals;
        new_F.locals = numLocals;
    }

    void print_instruction_as_is(Function &new_F, Instruction instruct) {
        new_F.items = instruct.items;
        L1_f.Instructions.push_back(L1_i);
    }

    // found in gen_set
    void print_read(Function &new_F, Item spill_str, int spill_var_counter, int num_locals) {
        num_locals = (num_locals - 1) * 8;
        //std::cout << spill_str.labelName << spill_var_counter << " <- mem rsp " << num_locals;
        Instruction i = new Instruction();
        // add var to instruction
        addItemToInstruction(*i, spill_str.labelName + std::stoi(spill_var_counter), Type::var);
        
        // add <-
        addItemToInstruction(*i, "<-", Type::assign_oper);

        // add mem
        addItemToInstruction(*i, "mem", Type::mem);

        // add num_locals
        addItemToInstruction(*i, std::stoi(num_locals), Type::num);

        new_F.instructions.push_back(i);
    }

    void print_write(Function &new_F, Item spill_str, int spill_var_counter, int num_locals) {
        num_locals = (num_locals - 1) * 8;
        Instruction i = new Instruction();
        addItemToInstruction(*i, "mem", Type::mem);
        addItemToInstruction(*i, "rsp", Type::reg);
        addItemToInstruction(*i, std::stoi(num_locals), Type::num);
        addItemToInstruction(*i, "<-", Type::assign_oper);
        addItemToInstruction(*i, spill_str.labelName + std::stoi(spill_var_counter), Type::var);

        new_F.push_back(i);

        //std::cout << "mem rsp " << num_locals << " <- " << spill_str.labelName << spill_var_counter; 
    }

    void print_spilled_instruction(Function &new_F, Instruction instruct, Item spill_var, Item spill_str, int spill_var_counter) {
        //std::string space = instruct.identifier != 3 ? " " : "";
        Instruction i;
        for(int ic = 0; ic < instruct.items.size() - 1; ++ic) {
            auto it = instruct.items[ic];
            if(it.type == Type::var) {
                if(it.labelName == spill_var.labelName)
                    addItemToInstruction(i, spill_str.labelName + spill_var_counter, Type::var);
                    //std::cout << spill_str.labelName << spill_var_counter << space;
                //else
                    //std::cout << i.labelName << space;
            }
            else
                std::cout << i.labelName << space;
        }

        auto i = instruct.items[instruct.items.size() - 1]; // last item
        if(i.type == Type::var) {
            if(i.labelName == spill_var.labelName)
                std::cout << spill_str.labelName << spill_var_counter;
            else
                std::cout << i.labelName;
        }
        else
            std::cout << i.labelName;
    }

	void spill(Function f, Item spill_var, Item spill_str) {
        // spill str is target string that each var will be changed to
        Function new_F;
        int spill_init_row = 0; // spill_var not found yet
        bool spill_var_found = false;
        std::string spill_var_mod = varNameModifier(spill_var); // modify variable name to allow for comparison with gen and kill set contents
        //std::cout << f.instructions.size() << '\n';
        //std::cout << "length of spill_var_mod: " << spill_var_mod.length() << ' ' << spill_var_mod << '\n';
        for(Instruction* instruct : f.instructions) {
            // all write to spill_var -> store to the new stack loc
            // all read from spill_var -> read from the new stack loc
            //TODO: consider to use gen/kill set here
            //for(auto k : instruct->kill_set)
                //std::cout << k << ' ';
            if(instruct->identifier == 0) { // assignment instruction found
                if(instruct->kill_set.find(spill_var_mod) != instruct->kill_set.end()) { // var is not in gen_set so not init yet
                    spill_var_found = true;
                    break;
                }
            }
            spill_init_row++;
        }
        // go through rest of instructions
        //write_function_name(f); // function name
        //std::cout << '(' << f.name << '\n'; 
        new_F.name = f.name;
        if(spill_var_found) {
            //std::cout << '\t';
            print_function_meta(new_F, spill_var_found, f);  // +1 to # of locals
            //std::cout << '\n';
            for(int i = 0; i < spill_init_row; ++i) {
                Instruction* instruct = f.instructions[i];
                //std::cout << '\t';
                print_instruction_as_is(*instruct);
                //std::cout << '\n';
            }

            int spill_var_counter = 0;
            for(int i = spill_init_row; i < f.instructions.size(); ++i) {
                Instruction* instruct = f.instructions[i];
                bool in_kill = instruct->kill_set.find(spill_var_mod) != instruct->kill_set.end();
                bool in_gen = instruct->gen_set.find(spill_var_mod) != instruct->gen_set.end();
                
                if(in_kill || in_gen) {
                    if(in_gen){ // if in gen_set
                        //std::cout << '\t';
                        print_read(new_F, spill_str, spill_var_counter, num_locals);
                        //std::cout << '\n';
    	            }
    
                    //std::cout << '\t';
                    print_spilled_instruction(new_F, *instruct, spill_var, spill_str, spill_var_counter);
                    //std::cout << '\n';
                    
                    if(in_kill) {
                        //std::cout << '\t';
                        print_write(new_F, spill_str, spill_var_counter, num_locals);
                        //std::cout << '\n';
                    }
                    spill_var_counter++;
                }
                else{
                    //std::cout << '\t';
                    print_instruction_as_is(new_F, *instruct);
                    //std::cout << '\n';
                }
            }
        }
        else {
            //std::cout << '\t';
            print_function_meta(new_F, spill_var_found, f); // no increment
            //std::cout << '\n';
            for(Instruction* instruct : f.instructions) {
                //std::cout << '\t';
                print_instruction_as_is(new_F, *instruct);
                //std::cout << '\n';
            }
        }

        //std::cout << ")\n";
    }

    //required for spill_only option
    void spill_wrapper(Program p) {
		Function f = *p.functions[0];
	    //std::cout << f.instructions.size() << '\n';	
	    Item spill_str = p.spill_extras.back();
		p.spill_extras.pop_back();
		Item spill_var = p.spill_extras.back();

		spill(f, spill_var, spill_str);
    }
}
