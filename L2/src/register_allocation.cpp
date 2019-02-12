#include <iostream> 
#include <register_allocation.h>
#include <unordered_set>
#include <set> //unordered_set isn't used because it can't hold enums without custom hash
#include <map> //unordered_map isn't used because it isn't possible to use an enum as the key or value without custom hash

namespace L2 {

//    const set_of_str GP_REGISTERS( {"r10", "r11", "r12", "r13", "r14", "r15", "r8", "r9", "rax", "rbp", "rbx", "rcx", "rdi", "rdx", "rsi"} );

    std::vector<REG> CALLER_SAVE {
            REG::r8,
            REG::r9,
            REG::r10,
            REG::r11,
            REG::rax,
            REG::rcx,
            REG::rdi,
            REG::rdx,
            REG::rsi,
 //   };

   // std::vector<REG> CALLEE_SAVE {
            REG::r12,
            REG::r13,
            REG::r14,
            REG::r15,
            REG::rbp,
            REG::rbx
    };

    std::map<std::string, REG> STR_REG_MAP = {
            {"r10", REG::r10},
            {"r11", REG::r11},
            {"r12", REG::r12},
            {"r13", REG::r13},
            {"r14", REG::r14},
            {"r15", REG::r15},
            {"r8", REG::r8},
            {"r9", REG::r9},
            {"rax", REG::rax},
            {"rbp", REG::rbp},
            {"rbx", REG::rbx},
            {"rdi", REG::rdi},
            {"rcx", REG::rcx},
            {"rdx", REG::rdx},
            {"rsi", REG::rsi}
    };

    std::map<REG, std::string> REG_STR_MAP = {
            {REG::r10, "r10"},
            {REG::r11, "r11"},
            {REG::r12, "r12"},
            {REG::r13, "r13"},
            {REG::r14, "r14"},
            {REG::r15, "r15"},
            {REG::r8, "r8"},
            {REG::r9, "r9"},
            {REG::rax, "rax"},
            {REG::rbp, "rbp"},
            {REG::rbx, "rbx"},
            {REG::rdi, "rdi"},
            {REG::rcx, "rcx"},
            {REG::rdx, "rdx"},
            {REG::rsi, "rsi"}
    };

    void RESET_REG_SETS() {
        CALLER_SAVE =
        {
            REG::r8,
            REG::r9,
            REG::r10,
            REG::r11,
            REG::rax,
            REG::rcx,
            REG::rdi,
            REG::rdx,
            REG::rsi,
     //   };

       // CALLEE_SAVE = 
     //   {
            REG::r12,
            REG::r13,
            REG::r14,
            REG::r15,
            REG::rbp,
            REG::rbx
        };
    }

    std::vector<Node> init_stack(std::vector<Node> IG_nodes) {
        std::vector<Node> stack;
        for(auto &n : IG_nodes) {
            // check if register
            if(STR_REG_MAP.find(n.name) != STR_REG_MAP.end()) {
                 n.color = STR_REG_MAP.find(n.name)->second;
                 n.isReg = true;
            }
            stack.push_back(n);
        }
        return stack;
    }

    std::pair<Function*, bool> color_graph(Function *fp) { //, std::pair<std::string, set_of_str> node) {
        //std::vector<Node> stack = fp->IG_nodes; // old IG
        std::vector<Node> new_IG; // empty IG, start rebuilding
        Function* new_F = fp;
        std::vector<Node> stack = init_stack(fp->IG_nodes);
        bool spilled = false;
		
        do {
            RESET_REG_SETS();
            Node popped = stack.back();
            stack.pop_back();
            
            // assign colors
			
            if(STR_REG_MAP.find(popped.name) != STR_REG_MAP.end()) { // if reg, assign itself
                //std::cout << "popped register name: " << popped.name << '\n';
                popped.color = STR_REG_MAP.find(popped.name)->second;
                popped.colored = true;
            }

            else { // popped is variable; assign a register to it
                //std::cout << popped.name << '\n';
//                for(auto n : popped.neighbors) {
//                    std::cout << n << ' ';
//                }
                //std::cout << '\n';
                for(auto r : CALLER_SAVE) {
                    if(popped.neighbors.find(REG_STR_MAP[r]) != popped.neighbors.end()) {
						
                        continue;
                    }
                    else {
                        //std::cout << "popped var name (reg match found): " << popped.name << '\n';
                        popped.color = r;
                        popped.colored = true;
                    }
                }
            }
//            if(!popped.colored) { // Caller saved register not assigned
//                for(auto r : CALLEE_SAVE) {
//                    if(popped.neighbors.find(REG_STR_MAP[r]) != popped.neighbors.end()) {
//                        continue;
//                    }
//                    else {
//                        popped.color = r;
//                        popped.colored = true;
//                    }
//                }
//            }
            if(!popped.colored) {// Callee saved register not assigned, so must spill
                //std::cout << "popped var name (reg match not found): " << popped.name << '\n';
                popped.color = REG::no_reg;
                spilled = true;
                new_F->nodes_to_spill.push_back(popped);
            }

            new_IG.push_back(popped);

        } while(!stack.empty());

        /*
         * Now that the variables have been assigned registers and spilled variables are identified,
         * begin to spill. This will generate a new function that can be analyzed for liveness. This
         * function will have a new IG created for it in the register_allocation do-while loop.
         */
        
        //for(auto const& node : new_F->nodes_to_spill) {
//            if(GP_REGISTERS.find(node.first) != GP_REGISTERS.end()) {// the node is register, so don't spill
//                //std::cout << "nothing spilled " << node.first << '\n';
//                continue;
//            }
//            else { //spill
                Item spill_var;
                //spill_var.labelName = node.first;
                spill_var.labelName = new_F->nodes_to_spill[0].name;
                //std::cout << "var spilled: " << spill_var.labelName << '\n';
                spill_var.type = Type::var;
                Item spill_str;
                //spill_str.labelName = node.first;
                spill_str.labelName = '%' + new_F->nodes_to_spill[0].name;
                spill_str.type = Type::var;
                *new_F = spill(*new_F, spill_var, spill_str);
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
        return std::pair<Function*, bool> (new_F, spilled);
    }

    Function* register_allocation(Function* fp) {
        Function* new_F = fp;
        bool spilled = false;
        do {
            analyze(new_F);
            generate_IG(new_F);

            // change IG from str_to_set to vector<Node>
            for(auto &p : new_F->IG) { // for each pair in the IG
                Node n;
                n.name = p.first;
                n.neighbors = p.second;
                new_F->IG_nodes.push_back(n); // IG_nodes is a vector<Node>
            }

            std::pair<Function*, bool> spilled_and_colored_IG = color_graph(new_F);
            new_F = spilled_and_colored_IG.first;
            std::cout << new_F->name << '\n';
            for(auto i : new_F->instructions) {
                for(auto it: i->items)
                    std::cout << it.labelName << ' ';
                std::cout << '\n';
            }
            spilled = spilled_and_colored_IG.second;
            //std::cout << new_F->name << '\n';
        } while(spilled);

        // register allocation
        

        return new_F;
    }

}
