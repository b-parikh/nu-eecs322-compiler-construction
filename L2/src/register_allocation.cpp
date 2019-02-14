#include <iostream> 
#include <register_allocation.h>
#include <unordered_set>
#include <set> //unordered_set isn't used because it can't hold enums without custom hash
#include <map> //unordered_map isn't used because it isn't possible to use an enum as the key or value without custom hash
#include <algorithm>
#include <string>

namespace L2 {

    const set_of_str GP_REGISTERS( {"r10", "r11", "r12", "r13", "r14", "r15", "r8", "r9", "rax", "rbp", "rbx", "rcx", "rdi", "rdx", "rsi"} );

    std::vector<REG> ALL_REG {
            REG::r8,
            REG::r9,
            REG::r10,
            REG::r11,
            REG::rax,
            REG::rcx,
            REG::rdi,
            REG::rdx,
            REG::rsi,
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
        ALL_REG =
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
            REG::r12,
            REG::r13,
            REG::r14,
            REG::r15,
            REG::rbp,
            REG::rbx
        };
    }

    std::vector<Node> init_stack(std::vector<Node>& IG_nodes) {
        std::vector<Node> stack;
        for(auto& n : IG_nodes) {
            // if register, assign it a register (itself) right now
            if(STR_REG_MAP.find(n.name) != STR_REG_MAP.end()) {
                 n.color = STR_REG_MAP.find(n.name)->second;
                 n.isReg = true;
                 n.colored = true;
            }
            stack.push_back(n);
        }
        return stack;
    }

    Function* vars_to_reg(Function* fp) {
        for(auto &instruct : fp->instructions) {
            if(instruct->identifier == 12) { // stack-arg found
                Item destination = instruct->items[0]; 
                Item assign_oper = instruct->items[1];

                Item mem_addr = instruct->items[3];
                int stack_arg_M = stoi(mem_addr.labelName) + fp->locals * 8;
                mem_addr.labelName = std::to_string(stack_arg_M);

                Item mem_keyword;
                mem_keyword.labelName = "mem";
                mem_keyword.type = Type::mem;
                
                Item rsp_reg;
                rsp_reg.labelName = "rsp";
                rsp_reg.type = Type::reg;

                instruct->items.clear();
                instruct->items.push_back(destination);
                instruct->items.push_back(assign_oper);
                instruct->items.push_back(mem_keyword);
                instruct->items.push_back(rsp_reg);
                instruct->items.push_back(mem_addr);
               
                //(fp->locals)++; 
            }

            for(auto &it : instruct->items) {
                if(it.type != Type::var) {
                    continue;
                }
                else {
                    for(auto &n : fp->IG_nodes) {
                        if(n.name == varNameModifier(it)) {
                            it.labelName = REG_STR_MAP[n.color];
                            break;
                        }
                    }
                }
            } 
        }
        return fp;
    } 
        
    std::pair<std::vector<Node>, Function*> color_graph(Function *fp) { 
        std::vector<Node> new_IG; // empty IG, start rebuilding
        std::vector<Node> stack = init_stack(fp->IG_nodes);
        std::vector<Node> colored_variables; // use vector; can't use set because it'd need custom hash
        std::vector<Node> nodes_to_spill;
        bool spilled = false;
        // begin graph rebuild
        while(!stack.empty()) {
		    std::vector<REG> color_no_use; // colors that can't be used for the current popped node
			RESET_REG_SETS();
            Node popped = stack.back();
            stack.pop_back();
 
            /* Assign colors (aka registers) to the popped node.
             * If the node is a register, then we assign it to itself.
             * Otherwise, the node is a variable. If the variable can be
             * assigned a register, then do so. Else, spill the variable.
             */
            if(popped.colored) { // if the init_stack function has already assigned a reg to popped, then don't do anything
                new_IG.push_back(popped); // add registers to new_IG
                continue;
                //popped.color = STR_REG_MAP.find(popped.name)->second;
            }
            else { // popped is variable; try to assign a register to it
				color_no_use.clear();
                // Go through popped's neighbors
				for(auto n : popped.neighbors) { // n is a string :(
					//bool is_register = false;
                    if(STR_REG_MAP.find(n) != STR_REG_MAP.end()){ // skip if the neighbor is a REG
                        color_no_use.push_back(STR_REG_MAP[n]);
                        continue; // in this case, register can still interfere with variable
                    }
                    else {  // if the current neighbor is a variable
                        /* Find neighbor in colored_variables.
                         * Then, find the color of that colored_variable.
                         * Push this color into color_no_use.
                         * NOTE: CHECK THIS PART.
                         */
                        for(auto& cv : colored_variables) {
                            if(n == cv.name) { 
                                // if neighbor is colored, don't use that color for popped
								for(auto r: ALL_REG) {
									if (cv.color == r) {
										color_no_use.push_back(r); 
										break;
									}
								}
								break; // no need to check other values in colored_variables
                            }
                        }
					}
                }
    
                if(color_no_use.empty()) {
                    popped.color = ALL_REG[0];
                    popped.colored = true;
                    new_IG.push_back(popped);
                    //std::cerr << "assigned register (1): " << REG_STR_MAP[ALL_REG[0]] << '\n';
                    colored_variables.push_back(popped);
                }
                else {
                    for(auto no_r : color_no_use) // remove all of color_no_use from ALL_REG
                        ALL_REG.erase(std::remove(ALL_REG.begin(), ALL_REG.end(), no_r), ALL_REG.end());
                    if(!ALL_REG.empty()) {
                        popped.color = ALL_REG[0];
                        popped.colored = true;
                        //std::cerr << "assigned register: " << REG_STR_MAP[ALL_REG[0]] << '\n';
                        new_IG.push_back(popped);
                        colored_variables.push_back(popped);
                    }
                    else
                        nodes_to_spill.push_back(popped);
                }
            }
        }

        if(!nodes_to_spill.empty()) {
            spilled = true;
        }

        fp->IG_nodes = new_IG;
            
            /*
             * Now that the variables have been assigned registers and spilled variables are identified,
             * begin to spill. This will generate a new function that can be analyzed for liveness. This
             * function will have a new IG created for it in the register_allocation do-while loop.
             */
        
        //std::cout << (spilled ? "true\n" : "false\n");
        return std::pair<std::vector<Node>, Function*> (nodes_to_spill, fp);
    }

    /* color_graph tells us which nodes need to be spilled.
     * Then, register_allocation actually calls spill to spill
     * these nodes.
     */
    Function* register_allocation(Function* fp) {
        bool spilled = true;
        Function* curr_F;
        while(spilled) {
            // check if variables need to be spilled
            analyze(fp);
            generate_IG(fp);

            for(auto &p : fp->IG) { // for each pair (node : neighbors) in the IG
                Node n;
                n.name = p.first;
                n.neighbors = p.second;
                fp->IG_nodes.push_back(n); // IG_nodes is a vector<Node>
            }

            /* new_F now has both the old and new IG representations
             * We will not use the old representation in coloring because
             * it sucks.
             */

            std::pair<std::vector<Node>, Function*> color_graph_results = color_graph(fp);
            std::vector<Node> vars_to_spill = color_graph_results.first;
            fp = color_graph_results.second;
            spilled = (vars_to_spill.empty() ? false : true);
            
            if(spilled) {
                // spill all variables deemed to be spilled 
                for(auto& node_to_spill : vars_to_spill) {
                    curr_F = fp; // spilled function becomes current function
                    //std::cerr << fp->name << ' '  << node_to_spill.name << '\n';
                    Item spill_var;
                    spill_var.labelName = node_to_spill.name;
                    spill_var.type = Type::var;
                    Item spill_str;
                    spill_str.labelName = '%' + node_to_spill.name + "_spilled_";
                    spill_str.type = Type::var;
                    
                    fp = spill(curr_F, spill_var, spill_str);

                    /* Create gen/kill, in/out, and IG for newly spilled function.
                     * Feed this function into spill in the next iter of for loop.
                     */
                }
            }
//            std::cout << "NODE TO SPILL: " << vars_to_spill.size() << '\n';
//            if(vars_to_spill.size() != 0) 
//                std::cerr << "THAT IS... " << vars_to_spill[0].name << '\n';
//            std::cerr << fp->name << '\n';
//            std::cerr << fp->arguments << ' ' << fp->locals << '\n';
//            for(auto& instruct : fp->instructions) {
//                for(auto& i : instruct->items) {
//                    std::cerr << i.labelName << ' ';
//                }
//                std::cerr << '\n';
//            }
//            std::cerr << '\n';
        }
        vars_to_reg(fp);
        return fp;
    }
}
