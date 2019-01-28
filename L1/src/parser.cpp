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

  /*
   *
   * Our works
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
     pegtl::string<'m', 'e', 'm'> {};

  struct mem_op:
     pegtl::seq<mem, seps, reg, seps, number> {};

  /* 
   * instructions
   */
  struct assign_operator:
      pegtl::string<'<','-'>
     {};

  struct assignment:
    pegtl::seq<
	  // left side
      seps,
      pegtl::sor<
	mem_op,
	reg>,
      seps,

      assign_operator,

      // right side
      seps,
      pegtl::sor<
	mem_op,
        reg,
        number,
        label>,
      seps
    > {};
	// increment / decrement?

  struct arithmetic_operator:
	pegtl::seq<
		pegtl::sor<pegtl::one<'+'>,
		           pegtl::one<'-'>,
			   pegtl::one<'*'>,
			   pegtl::one<'&'>
		>,
		seps,
		pegtl::one<'='>
	// +=, -=, *=, &=
	> {};
  
  struct arithmetic:
	pegtl::seq<
	   seps,
	   pegtl::sor<
	   	mem_op,
	   	reg>,
	   seps,
	   arithmetic_operator,
	   seps,
	   pegtl::sor<
		mem_op,
		number,
		reg>,
	   seps
  	> {};

struct inc_dec_operator:
   pegtl::sor<
      pegtl::string<'+', '+'>,
      pegtl::string<'-', '-'>
   > {};

struct inc_dec:
pegtl::seq<
  seps,
  reg,
  seps,
  inc_dec_operator,
  seps
> {};


 struct comparison_operator:
  pegtl::sor<
   pegtl::string<'<', '='>,
   pegtl::one<'<'>,
   pegtl::one<'='>
   > {};

 struct compare:
pegtl::seq<
  pegtl::sor<reg, number>,
  seps,
  comparison_operator,
  seps,
  pegtl::sor<reg, number>
> {};

 struct assign_comparison:
pegtl::seq<
  seps,
  reg,
  seps,
  assign_operator,
  seps,
  compare,
  seps
> {};

 struct shifting_operator:
pegtl::seq<
  pegtl::sor<
   pegtl::string<'<', '<'>,
   pegtl::string<'>', '>'>
  >,
  pegtl::one<'='>
> {};

 struct shiftings:
   pegtl::seq<
  seps,
     reg,
     seps,
     shifting_operator,
     seps,
     pegtl::sor<
       reg,
       number>,
     seps
   > {};

 struct jumps:
   pegtl::seq<
  seps,
  pegtl::opt<
   pegtl::string<'g', 'o', 't', 'o'>,
   seps
  >,
     label,
  seps
   > {};

 struct cjump:
   pegtl::seq<
  seps,
  pegtl::string<'c', 'j', 'u', 'm', 'p'>,
  seps,
  compare,
  seps,
  label,
  pegtl::opt<
   seps,
   label
  >,
  seps
   > {};

 //not 100% sure we need this
struct runtime_func:
   pegtl::sor<
     pegtl::string<'p', 'r', 'i', 'n', 't'>,
       pegtl::string<'a', 'l', 'l', 'o', 'c', 'a', 't', 'e'>,
         pegtl::string<'a', 'r', 'r', 'a', 'y', '-', 'e', 'r', 'r', 'o' ,'r'>
            > {};

 struct call:
   pegtl::seq<
  seps,
  pegtl::string<'c', 'a', 'l', 'l'>,
  seps,
  pegtl::sor<
   label,
   runtime_func,
   reg
  >,
  seps,
  number
   > {};

 struct misc:
   pegtl::seq<
     seps,
     reg,
     seps,
     pegtl::one<'@'>,
     seps,
     reg,
     seps,
     number,
     seps
   > {};

  struct Instruction_rule:
      // insert all instructions
       pegtl::sor<
       pegtl::seq<pegtl::at<assign_comparison>, assign_comparison>,
       pegtl::seq< pegtl::at<assignment>, assignment>,
       pegtl::seq< pegtl::at<arithmetic>, arithmetic>,
       pegtl::seq< pegtl::at<inc_dec >, inc_dec>,
       pegtl::seq< pegtl::at<Instruction_return_rule>, Instruction_return_rule>
    > { };

  struct Instructions_rule:
    pegtl::star<
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
      auto i = new Instruction();
      i->identifier = 1;
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

  template<> struct action < reg > {
    template < typename Input >
    static void apply (const Input &in, Program &p) {
	   Item i;
	   i.labelName = in.string();
	   parsed_registers.push_back(i);
    }
  };

  template<> struct action < assign_operator  > {
    template < typename Input > static void apply (const Input &in, Program &p) {
       Item i;
       i.labelName = in.string();
       parsed_registers.push_back(i);
    }
  };

  template<> struct action < mem > {
    template < typename Input > static void apply (const Input &in, Program &p) {
	   Item i;
     	   i.labelName = in.string();
	   parsed_registers.push_back(i);
    }
  };

  template<> struct action < number > {
    template < typename Input > static void apply (const Input &in, Program &p) {
	   Item i;
           i.labelName = in.string();
	   parsed_registers.push_back(i);
    }
  };

  template<> struct action < assignment > {
    template < typename Input > static void apply (const Input &in, Program &p) {
	auto currFunc = p.functions.back();
        Instruction* i = new Instruction(); // instruction will be reversed when generating x86
	i->identifier = 0;
        for(std::vector<Item>::iterator it = parsed_registers.begin(); it != parsed_registers.end(); ++it) {
  	  auto currItemP = it;
	  i->items.push_back(*currItemP);
        }

        currFunc->instructions.push_back(i);
 	parsed_registers.clear(); 
    }
  };

  template<> struct action < arithmetic_operator > {
    template < typename Input > static void apply (const Input &in, Program &p) {
	Item i;
	i.labelName = in.string();
	parsed_registers.push_back(i);
    }
  };

  template<> struct action < arithmetic > {
    template < typename Input > static void apply (const Input &in, Program &p) {
	Instruction* i = new Instruction();
	auto currFunc = p.functions.back();
	i->identifier = 2;
	for(auto currItemP : parsed_registers) {
	   i->items.push_back(currItemP);
	}
	parsed_registers.clear();
	currFunc->instructions.push_back(i);
    }
  };

  template<> struct action < inc_dec_operator > {
    template < typename Input > static void apply (const Input &in, Program &p) {
	Item i;
	i.labelName = in.string();
	parsed_registers.push_back(i);
    }
  };

  template<> struct action < inc_dec > {
    template < typename Input > static void apply (const Input &in, Program &p) {
	Instruction* i = new Instruction();
	auto currFunc = p.functions.back();
	i->identifier = 3;
	for(auto currItemP : parsed_registers) {
	   i->items.push_back(currItemP);
	}
	parsed_registers.clear();
	currFunc->instructions.push_back(i);
    }
  };

  template<> struct action < comparison_operator > {
    template < typename Input > static void apply (const Input &in, Program &p) {
	Item i;
	i.labelName = in.string();
	parsed_registers.push_back(i);
    }
  };
  
  template<> struct action < assign_comparison > {
    template < typename Input > static void apply (const Input &in, Program &p) {
	Instruction* i = new Instruction();
	auto currFunc = p.functions.back();
	i->identifier = 4;
	for(auto currItemP : parsed_registers) {
	   i->items.push_back(currItemP);
	   std::cout << currItemP.labelName << '\n';
	}
	parsed_registers.clear();
	currFunc->instructions.push_back(i);
    }
  };

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
