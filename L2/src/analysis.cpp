#include <analysis.h>

namespace L2 {

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

    void compute_gen_and_kill(Instruction &instruct) {

        if(instruct.identifier == 0) { // assignment
            std::cout << instruct.gen_set.size() << '\n';
            gk_assignment(instruct);
        }
            
        // else if(ip->identifier == 1)
        // write_return(move_stack_by, outputFile);
        // else if(ip->identifier == 2)
        // write_arithmetic(ip, outputFile);
        // else if(ip->identifier == 3)
        // write_inc_dec(ip, outputFile);
        // else if(ip->identifier == 4)
        // write_assign_comparison(ip, outputFile);
        // else if(ip->identifier == 5)
        // write_shift(ip, outputFile);
        // else if(ip->identifier == 6)
        // write_goto_jump(ip, outputFile); // just a label
        // else if(ip->identifier == 7)
        // write_cjump_twoargs(ip, outputFile);
        // else if(ip->identifier == 8)
        // write_lea(ip, outputFile);
        // else if(ip->identifier == 9)
        // write_call(ip, outputFile, fp->arguments);
        // else if(ip->identifier == 10)
        // write_label_instruction(ip,outputFile);
        // else
        // write_cjump_onearg(ip, outputFile);

    }

    // void compute_in_and_out(Function &f) {

    //     int num_instructions = f.instructions.size();

    //     do{
    //         set_of_str in_set;
    //         // set_of_str out_set; // no out_set as there is no successor
    //         in_set = f.instructions[num_instructions - 1].gen_set;
    //         f.in_set = in_set;

    //         for(int i = num_instructions - 2; i >= 0; --i) {
    //             int successor = i + 1;
    //             f.instructions[i].out_set = f.instructions[successor].in_set;
    //             f.instructions[i].in_set = 
    //         }
    //     }

    // }

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
