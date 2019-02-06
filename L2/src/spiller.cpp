#include <spiller.h>


namespace L2 {

	void spill(Function f, string spill_var, string spill_str) {
        for(Instruction* instruct : f.instructions) {
            // all write to spill_var -> store to the new stack loc
            // all read from spill_var -> read from the new stack loc
            //TODO: consider to use gen/kill set here
            if(spill_var) {
            }
        }
	}

    void spill_wrapper(Program p) {
		Function f = *p.functions[0];
		
		std::string spill_str = *p.extra.back();
		*p.extra.pop_back();
		std::string spill_var = *p.extra.back();

		spill(f, spill_var, spill_str);

    }
}
