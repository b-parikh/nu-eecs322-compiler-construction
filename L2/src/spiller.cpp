#include <spiller.h>
#include <iostream>

namespace L2 {

    void addItemToInstruction(Instruction* i, std::string labelName, Type type) {
        Item it;
        it.labelName = labelName;
        it.type = type;
        i->items.push_back(it);
    }

    //done
    int print_function_meta(Function &new_F, bool spill_var_found, Function &f) {
        int numLocals = f.locals + (spill_var_found ? 1 : 0);
        //std::cout << "print_function_meta called: ";
        //std::cout << "f.locals: " << f.locals << " numLocals: " << numLocals << '\n';
        //std::cout << f.arguments << ' ' << numLocals << ' ';
        new_F.locals = numLocals;
        return numLocals;
    }

    // Instead of just copying the instruction into new_F, each item is placed manually.
    // This is to ensure that the gen, kill, in, and out sets aren't also copied over.
    void print_instruction_as_is(Function &new_F, Instruction* instruct) {
        Instruction* i = new Instruction();
        for(auto it : instruct->items) {
            i->items.push_back(it);
        }

        i->identifier = instruct->identifier;
        new_F.instructions.push_back(i);
    }

    //done
    // found in gen_set
    void print_read(Function &new_F, Item spill_str, int spill_var_counter, int num_locals) {
        num_locals = (num_locals - 1) * 8;
        //std::cout << spill_str.labelName << spill_var_counter << " <- mem rsp " << num_locals;
        Instruction* i = new Instruction();
        // add var to instruction
        addItemToInstruction(i, spill_str.labelName + std::to_string(spill_var_counter), Type::var);
        
        // add <-
        addItemToInstruction(i, "<-", Type::assign_oper);

        // add mem
        addItemToInstruction(i, "mem", Type::mem);

        // add rsp
        addItemToInstruction(i, "rsp", Type::reg);
        
        // add num_locals
        addItemToInstruction(i, std::to_string(num_locals), Type::num);

        i->identifier = 0;
        new_F.instructions.push_back(i);
    }

    //done
    void print_write(Function &new_F, Item spill_str, int spill_var_counter, int num_locals) {
        num_locals = (num_locals - 1) * 8;
        Instruction* i = new Instruction();
        addItemToInstruction(i, "mem", Type::mem);
        addItemToInstruction(i, "rsp", Type::reg);
        addItemToInstruction(i, std::to_string(num_locals), Type::num);
        addItemToInstruction(i, "<-", Type::assign_oper);
        addItemToInstruction(i, spill_str.labelName + std::to_string(spill_var_counter), Type::var);

        i->identifier = 0; //assignment
        new_F.instructions.push_back(i);

        //std::cout << "mem rsp " << num_locals << " <- " << spill_str.labelName << spill_var_counter; 
    }

    //spill_var is var to spill; spill_str is name of new vars created by spill
    void print_spilled_instruction(Function &new_F, Instruction instruct, Item spill_var, Item spill_str, int spill_var_counter) {
        //std::string space = instruct.identifier != 3 ? " " : "";
        Instruction* new_I = new Instruction();
        for(int ic = 0; ic < instruct.items.size(); ++ic) { // add every item in instruct to new instruct (new_i)
            auto it = instruct.items[ic];
            if(it.type == Type::var) {
                if(varNameModifier(it) == spill_var.labelName) {
                    //std::cout << "varNameModifier(it): " << varNameModifier(it) << '\n';
                    addItemToInstruction(new_I, spill_str.labelName + std::to_string(spill_var_counter), Type::var);
                } //std::cout << spill_str.labelName << spill_var_counter << space;
                else
                    addItemToInstruction(new_I, it.labelName, Type::var);
                    //std::cout << i.labelName << space;
            }
            else
                addItemToInstruction(new_I, it.labelName, it.type);
                //std::cout << i.labelName << space;
        }
        new_I->identifier = instruct.identifier;
        new_F.instructions.push_back(new_I);

//        auto i = instruct.items[instruct.items.size() - 1]; // last item
//        if(i.type == Type::var) {
//            if(i.labelName == spill_var.labelName)
//                std::cout << spill_str.labelName << spill_var_counter;
//            else
//                std::cout << i.labelName;
//        }
//        else
//            std::cout << i.labelName;
    }

	Function spill(Function f, Item spill_var, Item spill_str) {
        // spill str is target string that each var will be changed to
        Function new_F;
        int spill_init_row = 0; // spill_var not found yet
        bool spill_var_found = false;
        std::string spill_var_labelName = spill_var.labelName; // modify variable name to allow for comparison with gen and kill set contents

        //std::cout << "spill_var_mod: " << spill_var_mod << '\n';
        //std::cout << f.instructions.size() << '\n';
        //std::cout << "length of spill_var_mod: " << spill_var_mod.length() << ' ' << spill_var_mod << '\n';
        for(Instruction* instruct : f.instructions) {
            // all write to spill_var -> store to the new stack loc
            // all read from spill_var -> read from the new stack loc
            //TODO: consider to use gen/kill set here
            //for(auto k : instruct->kill_set)
                //std::cout << k << ' ';
            if(instruct->identifier == 0) { // assignment instruction found
                if(instruct->kill_set.find(spill_var_labelName) != instruct->kill_set.end()) { // var is not in gen_set so not init yet
                    spill_var_found = true;
                    break;
                }
//                else {
//                    //std::cout <<  "spill_var_labelName: " << spill_var_labelName << '\n';
//                    for(auto i : instruct->items){
//                        if(varNameModifier(i) == spill_var_labelName) {
//                            //std::cout << varNameModifier(i) << '\n';
//                            spill_var_found = true;
//                            break;
//                        }
//                        //std::cout << '\n';
//                    }
//                    if(spill_var_found)
//                        break;
//                }
            }
            spill_init_row++;
        }

        // go through rest of instructions
        //write_function_name(f); // function name
        //std::cout << '(' << f.name << '\n'; 
        new_F.name = f.name;
        
        if(spill_var_found) {
            //std::cout << '\t';
            int num_locals = print_function_meta(new_F, spill_var_found, f);  // +1 to # of locals
            //std::cout << "spill_var_found num_locals: " << num_locals << '\n';
            //std::cout << '\n';
            for(int i = 0; i < spill_init_row; ++i) {
                Instruction* instruct = f.instructions[i];
                //std::cout << '\t';
                print_instruction_as_is(new_F, instruct);
                //std::cout << '\n';
            }

            int spill_var_counter = 0;
            for(int i = spill_init_row; i < f.instructions.size(); ++i) {
                Instruction* instruct = f.instructions[i];
                bool in_kill = instruct->kill_set.find(spill_var_labelName) != instruct->kill_set.end();
                bool in_gen = instruct->gen_set.find(spill_var_labelName) != instruct->gen_set.end();
                
//                bool var_assign = false;
//                for(auto it : instruct->items) {
//                    if(varNameModifier(it) == spill_var_labelName)
//                        var_assign = true;
//                }

                if(in_kill || in_gen) {
                    if(in_gen){ // if in gen_set
                        //std::cout << '\t';
                        //std::cout << "print_read\n"; 
                        print_read(new_F, spill_str, spill_var_counter, num_locals);
                        //std::cout << '\n';
    	            }
    
                    //std::cout << '\t';
                    print_spilled_instruction(new_F, *instruct, spill_var, spill_str, spill_var_counter);
                    //std::cout << '\n';
                    
                    if(in_kill) {
                        //std::cout << '\t';
                        //std::cout << "print_write\n";
                        print_write(new_F, spill_str, spill_var_counter, num_locals);
                        //std::cout << '\n';
                    }
                    spill_var_counter++;
                }
//                else if (var_assign) {
//                    Instruction* new_I = new Instruction();
//
//                    for(auto it : instruct->items) {
//                        addItemToInstruction(new_I, it.labelName, it.type);
//                    }
//                    spill_var_counter++;
//                }
                else{
                    //std::cout << '\t';
                    //std::cout << "print_i_as_is\n";
                    print_instruction_as_is(new_F, instruct);
                    //std::cout << '\n';
                }
            }
        }
        else { // spilled var not found; copy everything over
            //std::cout << '\t';
            int num_locals = print_function_meta(new_F, spill_var_found, f); // no increment
            //std::cout << "spill_var not found: " << num_locals << '\n';
            //std::cout << '\n';
            for(Instruction* instruct : f.instructions) {
                //std::cout << '\t';
                print_instruction_as_is(new_F, instruct);
                //std::cout << '\n';
            }
        }
        return new_F;
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
