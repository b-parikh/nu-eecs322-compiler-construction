#include <spiller.h>
#include <iostream>
#include <utils.h>

namespace L2 {

    int print_function_meta(bool spill_var_found, Function f) {
        int numLocals = f.locals + (spill_var_found ? 1 : 0);
        //std::cout << "f.locals: " << f.locals << " numLocals: " << numLocals << ' ';
        std::cout << f.arguments << ' ' << numLocals;
        return numLocals;
    }

    void print_instruction_as_is(Instruction instruct) {
        if(instruct.identifier == 3) {
            for(int i = 0; i < instruct.items.size(); ++i) {
                std::cout << instruct.items[i].labelName; 
            }
        }
        else {
            for(int i = 0; i < instruct.items.size() - 1; ++i)
                std::cout << instruct.items[i].labelName << ' ';
        std::cout << instruct.items[instruct.items.size() - 1].labelName;
        }
    }

    // found in gen_set
    void print_read(Item spill_str, int spill_var_counter, int num_locals) {
        num_locals = (num_locals - 1) * 8;
        std::cout << spill_str.labelName << spill_var_counter << " <- mem rsp " << num_locals;
    }

    void print_write(Item spill_str, int spill_var_counter, int num_locals) {
        num_locals = (num_locals - 1) * 8;
        std::cout << "mem rsp " << num_locals << " <- " << spill_str.labelName << spill_var_counter; 

    }

    void print_spilled_instruction(Instruction instruct, Item spill_var, Item spill_str, int spill_var_counter) {
        std::string space = instruct.identifier != 3 ? " " : "";
        for(int ic = 0; ic < instruct.items.size() - 1; ++ic) {
            auto i = instruct.items[ic];
            if(i.type == Type::var) {
                if(i.labelName == spill_var.labelName)
                    std::cout << spill_str.labelName << spill_var_counter << space;
                else
                    std::cout << i.labelName << space;
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
        std::cout << '(' << f.name << '\n'; 
        if(spill_var_found) {
            std::cout << '\t';
            int num_locals = print_function_meta(spill_var_found, f);  // +1 to # of locals
            std::cout << '\n';
            for(int i = 0; i < spill_init_row; ++i) {
                Instruction* instruct = f.instructions[i];
                std::cout << '\t';
                print_instruction_as_is(*instruct);
                std::cout << '\n';
            }

            int spill_var_counter = 0;
            for(int i = spill_init_row; i < f.instructions.size(); ++i) {
                Instruction* instruct = f.instructions[i];
                bool in_kill = instruct->kill_set.find(spill_var_mod) != instruct->kill_set.end();
                bool in_gen = instruct->gen_set.find(spill_var_mod) != instruct->gen_set.end();
                
                if(in_kill || in_gen) {
                    if(in_gen){ // if in gen_set
                        std::cout << '\t';
                        print_read(spill_str, spill_var_counter, num_locals);
                        std::cout << '\n';
    	            }
    
                    std::cout << '\t';
                    print_spilled_instruction(*instruct, spill_var, spill_str, spill_var_counter);
                    std::cout << '\n';
                    
                    if(in_kill) {
                        std::cout << '\t';
                        print_write(spill_str, spill_var_counter, num_locals);
                        std::cout << '\n';
                    }
                    spill_var_counter++;
                }
                else{
                    std::cout << '\t';
                    print_instruction_as_is(*instruct);
                    std::cout << '\n';
                }
            }
        }
        else {
            std::cout << '\t';
            print_function_meta(spill_var_found, f); // no increment
            std::cout << '\n';
            for(Instruction* instruct : f.instructions) {
                std::cout << '\t';
                print_instruction_as_is(*instruct);
                std::cout << '\n';
            }
        }

        std::cout << ")\n";
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
