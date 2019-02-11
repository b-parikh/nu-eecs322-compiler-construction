#include <string>
#include <vector>
#include <utility>
#include <algorithm>
#include <set>
#include <iterator>
#include <iostream>
#include <cstring>
#include <cctype>
#include <cstdlib>
#include <stdint.h>
#include <unistd.h>

#include <parser.h>
#include <analysis.h>
#include <interference.h>
// #include <transformer.h>
#include <code_generator.h>
#include <spiller.h>
#include <register_allocation.h>
#include <utils.h>

using namespace std;

void print_help (char *progName){
  std::cerr << "Usage: " << progName << " [-v] [-g 0|1] [-O 0|1|2] [-s] [-l 1|2] [-i] SOURCE" << std::endl;
  return ;
}

int main(
  int argc, 
  char **argv
  ){
  auto enable_code_generator = true;
  auto spill_only = false;
  auto interference_only = false;
  int32_t liveness_only = 0;
  int32_t optLevel = 0;

  /* 
   * Check the compiler arguments.
   */
  // Utils::verbose = false;
  if( argc < 2 ) {
    print_help(argv[0]);
    return 1;
  }
  int32_t opt;
  while ((opt = getopt(argc, argv, "vg:O:sl:i")) != -1) {
    switch (opt){

      case 'l':
        liveness_only = strtoul(optarg, NULL, 0);
        break ;

      case 'i':
        interference_only = true;
        break ;

      case 's':
        spill_only = true;
        break ;

      case 'O':
        optLevel = strtoul(optarg, NULL, 0);
        break ;

      case 'g':
        enable_code_generator = (strtoul(optarg, NULL, 0) == 0) ? false : true ;
        break ;

      case 'v':
        // Utils::verbose = true;
        break ;

      default:
        print_help(argv[0]);
        return 1;
    }
  }


  L2::Program p;
  /*
   * Parse the input file.
   */
  if (spill_only){

    /* 
     * Parse an L2 function and the spill arguments.
     */
     //TODO
    p = L2::spill_function(argv[optind]);
    
  } else if (liveness_only){

    /*
     * Parse an L2 function.
     */
     //TODO
    p = L2::parse_function(argv[optind]);
  } else if (interference_only){
    p = L2::parse_function(argv[optind]);

    /*
     * Parse an L2 function.
     */
     //TODO

  } else {

    /* 
     * Parse the L2 program.
     */
    p = L2::parse_file(argv[optind]);
  }

  /*
   * Special cases.
   */
  if (spill_only){

    /*
     * Spill.
     */
     //TODO
    //L2::analyze(p, liveness_only);
    L2::spill_wrapper(p);
    return 0;
  }

  /*
   * Liveness test.
   */
  if (liveness_only){
    //L2::analyze(p, liveness_only);
    return 0;
  }

  /*
   * Interference graph test.
   */
  if (interference_only){
    //TODO
    //L2::analyze(p, liveness_only);
    //L2::generate_IG(p);
    return 0;
  }

  /*
   * Generate the target code.
   */
  if (enable_code_generator){
    L2::generate_code(p); 
  }

  return 0;
}
