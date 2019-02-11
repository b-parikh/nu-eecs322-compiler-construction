#include <iostream> 
#include <register_allocation.h>
#include <unordered_set>

namespace L2 {

    /*
     * Receives a function and returns a struct 
     * with vector of instructions
     */
    const set_of_str GP_REGISTERS( {"r10", "r11", "r12", "r13", "r14", "r15", "r8", "r9", "rax", "rbp", "rbx", "rcx", "rdi", "rcx", "rdi", "rdx", "rsi"} );
    enum class reg{r12, r13, r14, r15, rbp, rbx,  r8, r9, r10, r11, rax, rcx, rdi, rdx, rsi};

    std::unordered_set<reg> CALLER_SAVE;
    std::unordered_set<reg> CALLEE_SAVE;
    for(int i = r8; i != rsi; ++i)
        CALLER_SAVE.insert(i);
    for(int i = r12; i != r8; ++i)
        CALLEE_SAVE.insert(i);

    std::vector<Node> init_stack(str_to_set IG) {
        std::vector<Node> stack;
        for(auto &IG_node : IG) {
            int colorNum = 0;
            Node n;
            n.name = IG_node.first;
            n.neighbors = IG_node.second;

            // assign color if register is found
            if(GP_REGISTERS.find(n.name) != GP_REGISTERS.end()) {
                 n.color = colorNum;
                 ++colorNum;
            }
            stack.push_back(n);
        }
        return stack;
    }

    Function color_graph(Function *fp, std::pair<std::string, set_of_str> node) { // IG is required as well, and fp includes IG
        str_to_set IG = fp->IG;
        std::vector stack = init_stack(IG);
        do {
            Node popped = stack.back();
            stack.pop_back();



            
        } while(!stack.empty());
        Function new_F = *fp;
        for(auto const& node : IG) {
            if(GP_REGISTERS.find(node.first) != GP_REGISTERS.end()) {// the node is register
                //std::cout << "nothing spilled " << node.first << '\n';
                continue;
            }
            else {
                //spill
                Item spill_var;
                spill_var.labelName = node.first;
                //std::cout << "var spilled: " << spill_var.labelName << '\n';
                spill_var.type = Type::var;
                Item spill_str;
                spill_str.labelName = node.first;
                spill_str.type = Type::var;
                new_F = spill(new_F, spill_var, spill_str);
                //std::cout << '(' << new_F.name << '\n';
//                for(auto i : new_F.instructions) {
//                    for(auto it : i->items) {
//                        std::cout << it.labelName << ' ';
//                    }
//                    std::cout << '\n';
//                }
//                std::cout << ")\n";
            //}
        //}
        return std::pair<Function, bool> (new_F, spilled);
    }

    Function register_allocation(Function* fp) {
        Function new_F = *fp;
        bool spilled = false;
        do {
            analyze(new_F);
            generate_IG(new_F);
            std::pair<Function, bool> spilled_and_colored_IG = color_graph(new_F);
            new_F = spilled_and_colored_IG.first;
            spilled = spilled_and_colored_IG.second;
        } while(spilled);

        return new_F;
    }

}
