//    std::vector<Context*> identify_blocks(Function* fp) {
//		std::vector<Context*> all_contexts;
//        Context* c = new Context();
//		for (auto &ip : fp->instructions) {
//			if(ip->Type == InstructionType::label && !(c->Instructions.empty())) {
//                all_contexts.push_back(c);
//                c = new Context();
//			}
//			c->Instructions.push_back(ip);
//			if(ip->Type == InstructionType::br_unconditional || ip->Type == InstructionType::br_conditional) {
//				all_contexts.push_back(c);
//                c = new Context();
//			}
//		}
//        return all_contexts;
//	}
#include <IR.h>
#include <map>
#include <string>

namespace IR {
    
    void build_label_block_map(Program &p) {
        // return block creation
        Basic_block* final_block = new Basic_block();
        final_block->isEmpty= true;
        p.empty_block = final_block;

        for(auto& fp : p.functions) {
            for(auto& bp : fp->blocks) {
                std::string first_label = bp->instructions[0]->Items.back()->labelName; // first instruction is label
                fp->label_to_block[first_label] = bp;
            }
            fp->label_to_block["return"] = p.empty_block;
        }
    }

    void connect_blocks(Function* fp) {
        std::map<std::string, Basic_block*> label_to_block = fp->label_to_block;
        for(auto& bp : fp->blocks) {
            // for every block, push back to its next_block based on its last
            // instruction's labels and the function's label_to_block map

            // guaranteed to be br or return  
            Instruction* final_i = bp->instructions.back();
            if(final_i->Type == InstructionType::br_conditional) { // last two Items are labels
                Item* label1 = final_i->Items[final_i->Items.size() - 2];
                Item* label2 = final_i->Items[final_i->Items.size() - 1];

                bp->next_block.push_back(label_to_block[label1->labelName]);
                bp->next_block.push_back(label_to_block[label2->labelName]);
            }
            else if(final_i->Type == InstructionType::br_unconditional) {
                Item* label = final_i->Items.back(); // last item is label

                bp->next_block.push_back(label_to_block[label->labelName]);
            }
            else { // return instruction
                bp->next_block.push_back(label_to_block["return"]);
            }
        }
    }
}
