#include <analysis.h>

namespace L2 {

   set_of_str subtract_sets(set_of_str &s1, set_of_str &s2) {
        std::vector<std::string> toDelete;

        for(auto &i : s1) {
            set_of_str::const_iterator item = s2.find(i);
            if(item != s2.end()){
                s2.erase(item);
                toDelete.push_back(i);
            }
    }

    for(auto &i : toDelete){
        set_of_str::const_iterator item = s1.find(i);
        s1.erase(item);
    }

    return s1;

    std::string varNameModifier(Item i) { 
        if(i.type == Type::var)
            (i.labelName).erase(0,1);

        return i.labelName;
        }

    bool regOrVar(Item i) {
        if(i.type == Type::reg || i.type == Type::var)
            return true;
        return false;
    }

    // TODO: assginment - we will probably have a case of 4: w <- stack-arg M
    void gk_assignment(Instruction &i) {

        set_of_str gen_set = i.gen_set;
        set_of_str kill_set = i.kill_set;

        int instruction_length = (i.items).size();

        if (instruction_length == 3) {
            if(regOrVar(i.items[2])) { //RHS == reg, var
                gen_set.emplace(varNameModifier(i.items[2]));
            }
            std::cout << varNameModifier(i.items[0]) << '\n';
            kill_set.emplace(varNameModifier(i.items[0]));
            std::cout << varNameModifier(i.items[0]) << '\n';
        }
        else { // length == 5
            if(i.items[0].type == Type::mem) { // LHS is mem
                gen_set.emplace(varNameModifier(i.items[1])); // reg or var
                if(regOrVar(i.items[4]))
                    gen_set.emplace(varNameModifier(i.items[4]));
            } else {
                gen_set.emplace(varNameModifier(i.items[3]));
                kill_set.emplace(varNameModifier(i.items[0]));
            }
        }

        i.gen_set = gen_set;
        i.kill_set = kill_set;
    }

    void gk_arithmetics(Instruction &i) { // reg1, var1, mem = reg1, var, mem + rev2, var2, mem, num
        set_of_str gen_set = i.gen_set;
        set_of_str kill_set = i.kill_set;

        int instruction_length = (i.items).size();

        if (instruction_length == 3) {
            if(regOrVar(i.items[2])) { //RHS == reg, var
                gen_set.emplace(varNameModifier(i.items[2]));
            }
            // LHS = reg, var
            gen_set.emplace(varNameModifier(i.items[0]));
            kill_set.emplace(varNameModifier(i.items[0]));
        }
        else { // length == 5
            if(i.items[0].type == Type::mem) { // LHS is mem
                gen_set.emplace(varNameModifier(i.items[1])); // reg or var
                if(regOrVar(i.items[4]))
                    gen_set.emplace(varNameModifier(i.items[4]));
            } else {
                gen_set.emplace(varNameModifier(i.items[3]));
                gen_set.emplace(varNameModifier(i.items[0]));
                kill_set.emplace(varNameModifier(i.items[0]));
            }
        }

        i.gen_set = gen_set;
        i.kill_set = kill_set;
    }

    void gk_inc_dec(Instruction &i) { // reg, var = reg, var + 1
        i.gen_set.emplace(varNameModifier(i.items[0]));
        i.kill_set.emplace(varNameModifier(i.items[0]));
    }

    void gk_assign_comparison(Instruction &i) { // reg1, var1 <- reg2, var2, num (compare) reg3, var3, num
        if(regOrVar(i.items[2])) // first t in RHS is not number
            i.gen_set.emplace(varNameModifier(i.items[2]));
        if(regOrVar(i.items[2])) // second t in RHS is not number
            i.gen_set.emplace(varNameModifier(i.items[4]));

        i.kill_set.emplace(varNameModifier(i.items[0]));
    }

    void gk_shift(Instruction &i) { // reg1, var1 <<= reg2, var2
        i.gen_set.emplace(varNameModifier(i.items[1]));

        i.gen_set.emplace(varNameModifier(i.items[0]));
        i.kill_set.emplace(varNameModifier(i.items[0]));
    }

    void gk_cjump(Instruction &i) { //
        if(regOrVar(i.items[1])) // first t in RHS is not number
            i.gen_set.emplace(varNameModifier(i.items[1]));
        if(regOrVar(i.items[3])) // second t in RHS is not number
            i.gen_set.emplace(varNameModifier(i.items[3]));
    }

    void gk_lea(Instruction &i) { // reg1, var1 = reg2, var2 + (reg3, var3 * 4)
        i.gen_set.emplace(varNameModifier(i.items[2])); // items[1] == @
        i.gen_set.emplace(varNameModifier(i.items[3]));

        i.kill_set.emplace(varNameModifier(i.items[0]));
    }

    void gk_call(Instruction &i) { // call (reg, var, label, runtime) num
        if(regOrVar(i.items[1])) // neither label or runtime functions
            i.gen_set.emplace(varNameModifier(i.items[1]));

        int num_args = stoi(i.items[2].labelName);
        int num_args_gen = (num_args > 6) ? 6 : num_args;
        for (int inum = 0; inum < num_args_gen; inum++)
            i.gen_set.emplace(func_args[inum]);

        i.kill_set = caller_save_regs;
    }

    void compute_gen_and_kill(Instruction &instruct) {
        if(instruct.identifier == 0) { // assignment
            std::cout << instruct.gen_set.size() << '\n';
            gk_assignment(instruct);
        }
//         else if(ip->identifier == 1)
//            write_return(move_stack_by, outputFile);
         else if(instruct.identifier == 2)
            gk_arithmetics(instruct);
         else if(instruct.identifier == 3)
            gk_inc_dec(instruct);
         else if(instruct.identifier == 4)
            gk_assign_comparison(instruct);
         else if(instruct.identifier == 5)
            gk_shift(instruct);
//         else if(ip->identifier == 6)
//            write_goto_jump(ip, outputFile); // just a label
         else if(instruct.identifier == 7)
            gk_cjump(instruct);
         else if(instruct.identifier == 8)
            gk_lea(instruct);
         else if(instruct.identifier == 9)
            gk_call(instruct);
//         else if(ip->identifier == 10)
//            write_label_instruction(ip,outputFile);
         else if(instruct.identifier == 11)
            gk_cjump(instruct);
         else
            ;   // Do nothing for RETURN, GOTO, LABEL_inst

    }

    void compute_in_and_out(Function &f) {
        // computing in and out set going from last to first instruction
        int num_instructions = f.instructions.size();
        
        // compute prev_in and prev_out set for all instructions 
        set_of_str in_set;
        // set_of_str out_set; // last instruction doesn't have an out_set
        
        in_set = f.instructions[num_instructions - 1].gen_set;
        f.in_set = in_set; 
        for(int i = num_instructions - 2; i >= -; --i) {
            int successor = i + 1;
            f.instructions[i].out_set = f.instructions[successor].in_set;
            // calculate gen[i] U (out[i] - in[i])

        do{
            // compuing in and out set of last instruction
            in_set = f.instructions[num_instructions - 1].gen_set;
            f.in_set = in_set;

            for(int i = num_instructions - 2; i >= 0; --i) {
                int successor = i + 1;
                f.instructions[i].out_set = f.instructions[successor].in_set;
                f.instructions[i].in_set = 
            }
        }

    }

    void analyze(Program p) {
        Function f = *p.functions[0];
        std::cout << "Function: " << f.name << '\n';
        for(Instruction* instruct : f.instructions) {
            // std::cout << instruct-> identifier << '\n';
            compute_gen_and_kill(*instruct);
            // std::cout << instruct->gen_set.size() << '\n';
            // std::cout << instruct->kill_set.size() << '\n';
            for(auto s : instruct->gen_set) {
                std::cout << "gen_set: " << s << '\t';
            }
            for(auto s : instruct->kill_set)
                std::cout << "kill_set: " << s << '\n';
            
	    }

    }

}
