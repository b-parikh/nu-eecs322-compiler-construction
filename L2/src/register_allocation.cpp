#include <iostream> 
#include <register_allocation.h>
#include <unordered_set>
#include <set> //unordered_set isn't used because it can't hold enums without custom hash
#include <map> //unordered_map isn't used because it isn't possible to use an enum as the key or value without custom hash
#include <algorithm>

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

    void reset_Function(Function* fp) {
        fp->name = "";
        fp->arguments = 0;
        fp->locals = 0;
        fp->instructions.clear();
        fp->IG.clear();
        fp->IG_nodes.clear();
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
//					for(auto regi : GP_REGISTERS) {
//						if(n == regi) { // skip if the neighbor is a REG
//							is_register = true;
//							break;
//						}
//					}
//					if (is_register)
//						continue;

                    // if the current neighbor is a variable
                    else { 

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
										color_no_use.push_back(r); // TODO: If in color_no_use, just remove r from ALL_REG
										break;
									}
								}
                                // TODO Should condition of inner if hold in order to break?
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
            // TODO: color no use can be non empty and a register can still be assigned

            /* Register not assigned to popped, so must spill.
             * Break out of while loop because we have found something to spill.
             * (DOES THIS MEAN THAT THE NON-EMPTY STACK IS ABANDONED?)
             */ 
            //if(!popped.colored) {
                //nodes_to_spill.push_back(popped);
                //spilled = true;
                //break;
            //}
            // reconstruct the IG
            //new_IG.push_back(popped);
         // while stack isn't empty
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
//            Function* new_F = new Function();
//            new_F->name = fp->name;
//            if(spilled) {
//                Item spill_var;
//                spill_var.labelName = node_to_spill.name;
//                spill_var.type = Type::var;
//                Item spill_str;
//                spill_str.labelName = '%' + node_to_spill.name;
//                spill_str.type = Type::var;
//                *new_F = spill(*fp, spill_var, spill_str);
//            }

            /* If nothing is spilled, then just return the input function which had the variables swapped
             * for registers in the color member.
             * NOTE: THE INSTRUCTIONS DON'T HAVE VARIABLES CHANGED TO REGISTERS. This is done in vars_to_reg.
             */ 
//            else 
//                new_F = fp;
        
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
            
//            if (spilled == true){
//              std::cerr << "spilled = true\n";
//            }
//            else {
//              std::cerr << "spilled = false\n";
//            }

            if(spilled) {
                // spill all variables deemed to be spilled 
                for(auto &node_to_spill : vars_to_spill) {
                    curr_F = fp; // spilled function becomes current function
                    reset_Function(fp); // make way for new spilled function
                    
                    Item spill_var;
                    spill_var.labelName = node_to_spill.name;
                    spill_var.type = Type::var;
                    Item spill_str;
                    spill_str.labelName = '%' + node_to_spill.name;
                    spill_str.type = Type::var;
                    *fp = spill(*curr_F, spill_var, spill_str);

                    /* Create gen/kill, in/out, and IG for newly spilled function.
                     * Feed this function into spill in the next iter of for loop.
                     */
                    //analyze(fp);
                    //generate_IG(fp);
                }
            }
        }
        vars_to_reg(fp);
        return fp;
    }
//    Function* register_allocation(Function* fp) {
//        Function* new_F = fp;
//        bool spilled = false;
//        do {
//            analyze(new_F); // generate gen, kill, in, and out sets for each instruction
//            generate_IG(new_F); // create an IG for the function
//
//            new_F->IG_nodes.clear(); 
//            // change IG from str_to_set to vector<Node> for better representation
//            for(auto &p : new_F->IG) { // for each pair (node : neighbors) in the IG
//                Node n;
//                n.name = p.first;
//                n.neighbors = p.second;
//                new_F->IG_nodes.push_back(n); // IG_nodes is a vector<Node>
//            }
//
//            /* new_F now has both the old and new IG representations
//             * We will not use the old representation in coloring because
//             * it sucks.
//             */
//
//            std::pair<Function*, bool> spilled_and_colored_IG = color_graph(new_F);
//            new_F = spilled_and_colored_IG.first;
//            spilled = spilled_and_colored_IG.second; // if color_graph spilled, is true; else false
//        } while(spilled);
//        new_F = vars_to_reg(new_F);
//
//        return new_F; // return to code generator
//    }

}
