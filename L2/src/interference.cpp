#include <interference.h>
#include <iostream>

namespace L2 {

    set_of_str GP_REGISTERS( {"r10", "r11", "r12", "r13", "r14", "r15", "r8", "r9", "rax", "rbp", "rbx", "rcx", "rdi", "rcx", "rdi", "rdx", "rsi"} );

    bool find_in_vector(std::string toFind, vector_of_str &v) {
        for(auto s : v) {
            if (toFind == s)
                return true;
        }
        
        return false;
    }

    void init_IG(Function &f) {
        str_to_set IG = f.IG;
        for(auto r : GP_REGISTERS) {
            set_of_str gp_reg_temp = GP_REGISTERS;
            gp_reg_temp.erase(r);

            std::pair<std::string, set_of_str> reg_to_add (r, gp_reg_temp);
            IG.emplace(reg_to_add);
        }
        f.IG = IG;
    }

    vector_of_str get_vars(set_of_str aSet) {
        vector_of_str toReturn;
        for(auto s : aSet) {
            if(GP_REGISTERS.find(s) == GP_REGISTERS.end()) {
                toReturn.push_back(s);
            }
        }

        return toReturn;
    }

    void add_in_and_out_sets(Function &f) {
        str_to_set IG = f.IG;
        for(auto i : f.instructions) {
            std::vector<set_of_str> in_and_out;
            in_and_out.push_back(i->in_set);
            in_and_out.push_back(i->out_set);
            if(i->shifting_var_or_reg != "") { // special case of shifting
                    // we know that the gen_set contains only the variable/register
                    set_of_str gp_reg_temp = GP_REGISTERS;
                    gp_reg_temp.erase("rcx");

                    set_of_str edgesOfVar = gp_reg_temp;
                    IG[i->shifting_var_or_reg] = edgesOfVar;

                    for(auto s : gp_reg_temp) 
                        IG.find(s)->second.insert(i->shifting_var_or_reg);
            }
                    
            for(auto set : in_and_out) {
                vector_of_str vars_in_set = get_vars(set);
                if(!vars_in_set.empty()) {
                    for(auto new_var : vars_in_set) {
                        set_of_str new_var_edges;
                        for(auto var: set) {
                            if(new_var != var) {
                                new_var_edges.emplace(var);
                                IG[var].insert(new_var);
                            }

                        } //insert currently either replaces or does nothing; needs to only add to the set
                        auto isInIG = IG.find(new_var);
                        if(isInIG == IG.end()) { // if new_var is not in the graph
                            IG.insert( { {new_var, new_var_edges} } );
                        }
                        else {
                            for(auto edge : new_var_edges){ // if new_var in graph, then append all of its edges to the current node in IG of new_var
                                isInIG->second.insert(edge); // registers already added, so this usually deals with variables
                            }
                        }
                    }
                }
            }
        }
        f.IG = IG;
     }  

    void add_kill_out_set(Function &f) {
        str_to_set IG = f.IG;
        for(auto i : f.instructions) {
            if(i->reg_var_assignment == false) {
                for(auto k : i->kill_set) { // no need, kill set will have a max of 1 item
                    auto kp = IG.find(k);
                    if(kp == IG.end()) { // k doesn't have a node in IG
                        set_of_str k_edges;
                        for(auto o : i->out_set) {
                            k_edges.emplace(o);
                            IG.find(o)->second.emplace(k);
                        }
                        IG.insert( { {k, k_edges} } );
                    }
                    else {
                        for(auto o : i->out_set) {
                            if(o == "rax")
                                continue;
                            if(o == k)
                                continue;
                            kp->second.insert(o); // draw edge from everything in out set to k in kill set
                            IG.find(o)->second.emplace(k);
                        }
                    }
                }
            }
            else {
                for(auto k : i->kill_set) {
                    auto kp = IG.find(k);
                    if(kp == IG.end()) {
                        set_of_str k_edges;
                        IG.insert( { {k, k_edges} } );
                    }
                }
            }
        }
        f.IG = IG;
    }

    void generate_IG(Program p) {
        Function f = *p.functions[0];
        init_IG(f);
        add_in_and_out_sets(f);
        add_kill_out_set(f);

        for(auto key_val_pair : f.IG) {
            std::cout << key_val_pair.first << ' ';
            for(auto s : key_val_pair.second)
               std::cout << s << ' '; 
        std::cout << '\n';
        }
    }
}
