#include <spiller.h>


namespace L2 {

	void spill(Function f, string spill_var, string spill_str) {
		//TODO
	}

    void spill_wrapper(Program p) {
		Function f = *p.functions[0];
		
		std::string spill_str = *p.extra.back();
		*p.extra.pop_back();
		std::string spill_var = *p.extra.back();

		spill(f, spill_var, spill_str);

    }
}
