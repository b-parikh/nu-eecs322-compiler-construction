#include <string>
#include <vector>
#include <utility>
#include <algorithm>
#include <set>
#include <iterator>
#include <cstring>
#include <cctype>
#include <cstdlib>
#include <stdint.h>
#include <assert.h>

#include <tao/pegtl.hpp>
#include <tao/pegtl/analyze.hpp>
#include <tao/pegtl/contrib/raw_string.hpp>

#include <L1.h>
#include <parser.h>

namespace pegtl = tao::TAO_PEGTL_NAMESPACE;

using namespace pegtl;
using namespace std;

namespace L1 {

  /* 
   * Data required to parse
   */ 
  std::vector<Item> parsed_registers;

  /* 
   * Grammar rules from now on.
   */
  struct name:
    pegtl::seq<
      pegtl::plus< 
        pegtl::sor<
          pegtl::alpha,
          pegtl::one< '_' >
        >
      >,
      pegtl::star<
        pegtl::sor<
          pegtl::alpha,
          pegtl::one< '_' >,
          pegtl::digit
        >
      >
    > {};

  struct label:
    pegtl::seq<
      pegtl::one<':'>,
      name
    > {};

  struct number:
    pegtl::seq<
      pegtl::opt<
        pegtl::sor<
          pegtl::one< '-' >,
          pegtl::one< '+' >
        >
      >,
      pegtl::plus< 
        pegtl::digit
      >
    >{};

  struct function_name:
    label {};

  struct argument_number:
    number {};

  struct local_number:
    number {} ;

  struct comment: 
    pegtl::disable< 
      TAOCPP_PEGTL_STRING( "//" ), 
      pegtl::until< pegtl::eolf > 
    > {};

  /* 
   * Keywords.
   */
  struct str_return : TAOCPP_PEGTL_STRING( "return" ) {};

  struct seps: 
    pegtl::star< 
      pegtl::sor< 
        pegtl::ascii::space, 
        comment 
      > 
    > {};

  struct Label_rule:
    label {};

  struct Instruction_return_rule:
    pegtl::seq<
      str_return
    > { };

  struct Instruction_rule:
    pegtl::sor<
      pegtl::seq< pegtl::at<Instruction_return_rule>            , Instruction_return_rule             >
    > { };

  struct Instructions_rule:
    pegtl::plus<
      pegtl::seq<
        seps,
        Instruction_rule,
        seps
      >
    > { };

  struct Function_rule:
    pegtl::seq<
      pegtl::one< '(' >,
      seps,
      function_name,
      seps,
      argument_number,
      seps,
      local_number,
      seps,
      Instructions_rule,
      seps,
      pegtl::one< ')' >
    > {};

  struct Functions_rule:
    pegtl::plus<
      seps,
      Function_rule,
      seps
    > {};

  struct entry_point_rule:
    pegtl::seq<
      seps,
      pegtl::one< '(' >,
      seps,
      label,
      seps,
      Functions_rule,
      seps,
      pegtl::one< ')' >,
      seps
    > {};

  struct grammar : 
    pegtl::must< 
      entry_point_rule
    > {};

  /*
   *
   * Our wprks
   *
   */
  struct reg:
    pegtl::sor<
      pegtl::string<'r','a','x'>,
      pegtl::string<'r','b','x'>,
      pegtl::string<'r','d','x'>,
      pegtl::string<'r','1','0'>, 
      pegtl::string<'r','1','1'>, 
      pegtl::string<'r','1','2'>,
      pegtl::string<'r','1','3'>,
      pegtl::string<'r','1','4'>,
      pegtl::string<'r','1','5'>,
      pegtl::string<'r','d','i'>, 
      pegtl::string<'r','s','i'>,
      pegtl::string<'r','d','x'>,
      pegtl::string<'r','c','x'>,
      pegtl::string<'r','8'>,
      pegtl::string<'r','9'>,
      pegtl::string<'r','s','p'>
   > {};

  struct mem:
    pegtl::seq<
     seps,
     pegtl::string<'m','e','m'>,
     seps,
     reg,
     seps,
     number
    > {};

  /* 
   * instructions
   */
  struct assign_operator:
    pegtl::seq<
      seps,
      pegtl::string<'<','-'>,
      seps
    > {};

  struct assignment:
    pegtl::seq<
	  // left side
      seps,
      pegtl::sor<reg, mem>,
      seps,

      assign_operator,

      // right side
      seps,
      pegtl::sor<
        reg,
        mem,
        number,
        label>,
      seps
    > {};
	// increment / decrement?

  struct arithmetics:
	pegtl::seq<
	// +=, -=, *=, &=
	> {};

  struct comparison_operator:
    pegtl::seq<
	  pegtl::sor<
		pegtl::one<'<'>,
		pegtl::one<'='>,
		pegtl::string<'<', '='>
	  >
    > {};

  struct assign_comparison:
	pegtl::seq<

	> {};

  struct shiftings:
    pegtl::seq<
	// <<=, >>=
    > {};

  struct jumps:
    pegtl::seq<
	  seps,
	  pegtl::opt<
		pegtl::string<'g', 'o', 't', 'o'>,
		seps,
	  >
      label
    > {};

  struct cjump:
    pegtl::seq<
	// cjump t cmp t label label
	// cjum t cmp t label
    > {};

  struct call_func:
    pegtl::seq<
	// call func num
    > {};

  //not 100% sure we need this
  struct call_runtime:
    pegtl::seq<
    // print, allocate, array-error
    > {};

  struct misc:
    pegtl::seq<
    // w @ w w E
    > {};


  /* 
   * Actions attached to grammar rules.
   */
  template< typename Rule >
  struct action : pegtl::nothing< Rule > {};

  template<> struct action < label > {
    template< typename Input >
	static void apply( const Input & in, Program & p){
      if (p.entryPointLabel.empty()){
        p.entryPointLabel = in.string();
      } else {
        abort();
      }
    }
  };

  template<> struct action < function_name > {
    template< typename Input >
	static void apply( const Input & in, Program & p){
      auto newF = new Function();
      newF->name = in.string();
      p.functions.push_back(newF);
    }
  };

  template<> struct action < argument_number > {
    template< typename Input >
	static void apply( const Input & in, Program & p){
      auto currentF = p.functions.back();
      currentF->arguments = std::stoll(in.string());
    }
  };

  template<> struct action < local_number > {
    template< typename Input >
	static void apply( const Input & in, Program & p){
      auto currentF = p.functions.back();
      currentF->locals = std::stoll(in.string());
    }
  };

  template<> struct action < str_return > {
    template< typename Input >
	static void apply( const Input & in, Program & p){
      auto currentF = p.functions.back();
      auto i = new Instruction_ret();
      currentF->instructions.push_back(i);
    }
  };

  template<> struct action < Label_rule > {
    template< typename Input >
	static void apply( const Input & in, Program & p){
      Item i;
      i.labelName = in.string();
      parsed_registers.push_back(i);
    }
  };

  /*
   * Our works
   */



//  template<> struct action < reg > {
//    template < typename Input >
//        static void apply (const Input &in, Program &p) {

  Program parse_file (char *fileName){

    /* 
     * Check the grammar for some possible issues.
     */
    pegtl::analyze< grammar >();

    /*
     * Parse.
     */   
    file_input< > fileInput(fileName);
    Program p;
    parse< grammar, action >(fileInput, p);

    return p;
  }

}
