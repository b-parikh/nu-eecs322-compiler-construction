#include <iostream> 
#include <register_allocation.h>
#include <unordered_set>
#include <set> //unordered_set isn't used because it can't hold enums without custom hash
#include <map> //unordered_map isn't used because it isn't possible to use an enum as the key or value without custom hash

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
            std::cout << '\n';
        }
        return fp;
    } 

    std::pair<Function*, bool> color_graph(Function *fp) { //, std::pair<std::string, set_of_str> node) {
        //std::vector<Node> stack = fp->IG_nodes; // old IG
        std::vector<Node> new_IG; // empty IG, start rebuilding
        std::vector<Node> stack = init_stack(fp->IG_nodes);
        std::vector<Node> colored_variables; // can't use set because it'd need custom hash
		std::vector<REG> color_no_use;
        bool spilled = false;
        //std::cout << "stack: ";
//        for(auto s : stack)
//            std::cout << s.name << ' ';
//        std::cout << '\n';
        Node node_to_spill;
        while(!stack.empty()) {
            //std::cout << "right before new pop " << stack.size() << '\n';
            Node popped = stack.back();
            
            stack.pop_back();
            
            //std::cout << popped.name << '\n';
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
				color_no_use.clear();
				for(auto n : popped.neighbors) {
					bool is_register = false;
					for(auto regi : GP_REGISTERS) {
						if(n == regi) { // skip if the neighbor is a REG
							is_register = true;
							break;
						}
					}
					if (is_register)
						continue;
//					for(auto r : ALL_REG) {
//						if(popped.neighbors.find(REG_STR_MAP[r]) != popped.neighbors.end()) {
//							continue;
//						}
                    else {
                        //std::cout << "popped var name (reg match found): " << popped.name << '\n';
                        //bool neighbor_has_same_assignment = false;
                        for(auto& cv : colored_variables) {
							//std::cout << cv.name << ": "<< REG_STR_MAP[cv.color] << " ; " << REG_STR_MAP[r] << '\n';
                            if(n == cv.name) { // if the neighbor VAR is in colored_variables
								for(auto r: ALL_REG) {
									if (cv.color == r) {
										color_no_use.push_back(r);
										break;
									}
								}
								break; // no need to check other values in color_variables
                            }
                        }
					}
				}

                bool color_found = false;
				for(auto r : ALL_REG) {
					for(auto no_r : color_no_use) {
						if(r != no_r) {
							color_found = true;
							popped.color = r;
							popped.colored = true;
							//std::cout << "from the map: " << REG_STR_MAP[popped.color] << '\n';
							colored_variables.push_back(popped);
							break; // found a register that doesn't interfere; break
						}
                    }
					if(color_found)
						break;
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
            if(!popped.colored) {// Callee saved register not assigned, so must spill; break because we've found something to spill
                //std::cout << "popped var name (reg match not found): " << popped.name << '\n';
                node_to_spill = popped;
                spilled = true;
                //std::cout << "going to spill so break\n";
                break;
            }

            //std::cout << "right before popped push_back" << '\n';
            new_IG.push_back(popped);
            //std::cout << "pushed node to IG\n";
        } //while(!stack.empty());
        fp->IG_nodes = new_IG;
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
            Function* new_F = new Function();
            new_F->name = fp->name;
            if(spilled) {
                Item spill_var;
                //spill_var.labelName = node.first;
                spill_var.labelName = node_to_spill.name;
                std::cout << "var spilled: " << spill_var.labelName << '\n';
                spill_var.type = Type::var;
                Item spill_str;
                //spill_str.labelName = node.first;
                spill_str.labelName = '%' + node_to_spill.name;
                spill_str.type = Type::var;
                *new_F = spill(*fp, spill_var, spill_str);
            }
            else 
                new_F = fp;

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
            std::cout << "before analyze\n";
            analyze(new_F);
            std::cout << "after analyze\n";
            generate_IG(new_F);
            std::cout << "after IG\n";

            // change IG from str_to_set to vector<Node>
            for(auto &p : new_F->IG) { // for each pair in the IG
                Node n;
                n.name = p.first;
				std::cout << p.first << "\nneighbors: ";
				for (auto pk : p.second) 
					std::cout << pk << ' ';
				std::cout << '\n';
                n.neighbors = p.second;
                new_F->IG_nodes.push_back(n); // IG_nodes is a vector<Node>
            }
            std::cout << "changed IG representation\n";

            std::pair<Function*, bool> spilled_and_colored_IG = color_graph(new_F);
            std::cout << "spilled and colored\n";
            new_F = spilled_and_colored_IG.first;
            spilled = spilled_and_colored_IG.second;
            std::cout << new_F->name << '\n';
        } while(spilled);
        new_F = vars_to_reg(new_F);
//        std::cout << "register allocation\n";
//        std::cout << new_F->name << '\n';
//        for(auto i : new_F->instructions) {
//            for(auto it: i->items)
//                std::cout << it.labelName << ' ';
//            std::cout << '\n';
//        }

        return new_F; // return to code generator
    }

}
