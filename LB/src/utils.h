#pragma once

//#include <LB.h>
//#include <string>

#define DEBUG_OPT

#ifdef DEBUG_OPT
#    define DEBUG_LOG(x) std::cerr << x << '\n';
#else
#    define DEBUG_LOG(x) do {} while(0);
#endif

namespace LB {
   // std::string varNameModifier(Item* itp);
}
