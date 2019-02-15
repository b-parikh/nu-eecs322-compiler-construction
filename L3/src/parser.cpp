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

#include <parser.h>

namespace pegtl = tao::TAO_PEGTL_NAMESPACE;

using namespace pegtl;
using namespace std;

namespace L3 {

  /* 
   * Data required to parse
   */ 
  std::vector<Item> parsed_variables;
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

   struct Label_rule:
    label {};

  struct function_name:
    Label_rule {};

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

  struct Instruction_return_rule:
      pegtl::seq<
        str_return
      >{};

  /*
   *
   * Our works
   *
   */

  struct var:
    pegtl::seq<
      pegtl::one<'%'>,
      name
    > {}; 

  /* 
   * instructions
   */
  struct assign_operator:
	pegtl::string<'<','-'> {};

  struct assignment:
	pegtl::seq<
	  seps,
	  pegtl::sor<mem_op, reg>,
	  seps,
	  assign_operator,
	  seps,
	  pegtl::sor<mem_op, reg, number, Label_rule>,
      seps
    > {};

  struct arithmetic_operator:
	pegtl::seq<
		pegtl::sor<pegtl::one<'+'>,
		           pegtl::one<'-'>,
			   pegtl::one<'*'>,
			   pegtl::one<'&'>
		>,
		pegtl::one<'='>
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

 struct shift_operator:
pegtl::seq<
  pegtl::sor<
   pegtl::string<'<', '<'>,
   pegtl::string<'>', '>'>
  >,
  pegtl::one<'='>
> {};

 struct shift:
   pegtl::seq<
  seps,
     reg,
     seps,
     shift_operator,
     seps,
     pegtl::sor<reg, number>,
     seps
   > {};

 struct goto_jump:
   pegtl::seq<
	     seps,
	     l1_keyword, // goto
	     seps,
 	     Label_rule,
	     seps
  > {};

 struct label_instruction:
   pegtl::seq<
	 //seps,
     Label_rule
   > {};

struct cjump_onearg:
   pegtl::seq<
     seps,
     l1_keyword, // cjump
     seps,
     compare,
     seps,
     Label_rule,
     seps
     //pegtl::eol
   > {};

struct runtime_func:
   pegtl::sor<
     pegtl::string<'p', 'r', 'i', 'n', 't'>,
     pegtl::string<'a', 'l', 'l', 'o', 'c', 'a', 't', 'e'>,
     pegtl::string<'a', 'r', 'r', 'a', 'y', '-', 'e', 'r', 'r', 'o' ,'r'>
   > {};

 struct call:
   pegtl::seq<
  seps,
  l1_keyword,
  seps,
  pegtl::sor<
   Label_rule,
   runtime_func,
   reg
  >,
  seps,
  number,
	seps
   > {};

  struct Instruction_rule:
    pegtl::sor<
    pegtl::seq<Instruction_return_rule>, 
    pegtl::seq< pegtl::at<assign_comparison>, assign_comparison>,
    pegtl::seq<label_instruction>,
    pegtl::seq< pegtl::at<cjump_onearg>, cjump_onearg>,
    pegtl::seq< pegtl::at<call>, call>,
    pegtl::seq< pegtl::at<assignment>, assignment>,
    pegtl::seq< pegtl::at<arithmetic>, arithmetic>,
    pegtl::seq< pegtl::at<shift>, shift>,
    pegtl::seq< pegtl::at<goto_jump>, goto_jump>
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
  
  struct function_grammar:
    pegtl::must<
      Functions_rule
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
      //std::cout << "function_name action called for " << in.string() << '\n';
      p.functions.push_back(newF);
    }
  };

  template<> struct action < str_return > {
    template< typename Input >
	static void apply( const Input & in, Program & p){
      auto currentF = p.functions.back();
      auto i = new Instruction();
      i->identifier = 1;
      Item it;
      it.labelName = "return";
      i->items.push_back(it);
     // std::cout << "Return called\n";
      currentF->instructions.push_back(i);
    }
  };

  template<> struct action < Label_rule > {
    template< typename Input >
	static void apply( const Input & in, Program & p){
      Item i;
      //std::cout << "Label_rule action called.\n"; 
      i.labelName = in.string();
      i.type = Type::label;
      parsed_variables.push_back(i);
    }
  };

  /*
   * Our works
   */

  template<> struct action < var > {
    template < typename Input >
    static void apply (const Input &in, Program &p) {
	    Item i;
        //std::cout << "var action called\n";
        i.type = Type::var;
	    i.labelName = in.string();
	    parsed_variables.push_back(i);
    }
  };

  template<> struct action < assign_operator  > {
    template < typename Input > static void apply (const Input &in, Program &p) {
       Item i;
       i.labelName = in.string();
       //std::cout << "assign_operator action called\n";
       parsed_registers.push_back(i);
    }
  };

  template<> struct action < number > {
    template < typename Input > static void apply (const Input &in, Program &p) {
      Item i;
      i.labelName = in.string();
      i.type = Type::num;
      //std::cout << "number action called.\n";
      parsed_variables.push_back(i);
    }
  };

  template<> struct action < assignment > {
    template < typename Input > static void apply (const Input &in, Program &p) {
	    auto currFunc = p.functions.back();
      Instruction* i = new Instruction(); // instruction will be reversed when generating x86
      i->identifier = 0;
      //std::cout << "ASSIGNMENT\n";
      for(std::vector<Item>::iterator it = parsed_registers.begin(); it != parsed_registers.end(); ++it) {
  	    auto currItemP = it;
	    i->items.push_back(*currItemP);
	    //std::cout << it->labelName << ' ';
      }
      currFunc->instructions.push_back(i);
      parsed_registers.clear(); 
      //std::cout << '\n';
    }
  };

  template<> struct action < arithmetic_operator > {
    template < typename Input > static void apply (const Input &in, Program &p) {
	Item i;
	i.labelName = in.string();
    i.type = Type::arith_oper;
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
        //std::cout << '\n';
	currFunc->instructions.push_back(i);
    }
  };

  template<> struct action < comparison_operator > {
    template < typename Input > static void apply (const Input &in, Program &p) {
	  Item i;
	  i.labelName = in.string();
      i.type = Type::compare_oper;
      //std::cout << "comparison operator called\n";
	  parsed_registers.push_back(i);
    }
  };
  
  template<> struct action < assign_comparison > {
    template < typename Input > static void apply (const Input &in, Program &p) {
	Instruction* i = new Instruction();
	auto currFunc = p.functions.back();
	i->identifier = 4;
	//std::cout << "assign_comparison called" << '\n';
	for(auto currItemP : parsed_registers) {
	   i->items.push_back(currItemP);
	   //std::cout << currItemP.labelName << ' ';
	}
    //std::cout << '\n';
	parsed_registers.clear();
	currFunc->instructions.push_back(i);
    }
  };

  template<> struct action < shift_operator > {
    template < typename Input > static void apply (const Input &in, Program &p) {
    Item i;
    i.labelName = in.string();
    i.type = Type::shift_oper;
    parsed_registers.push_back(i);
    }
  };

  template<> struct action < shift > {
    template < typename Input > static void apply (const Input &in, Program &p) {
    Instruction* i = new Instruction();
    auto currFunc = p.functions.back();
    i->identifier = 5;
    //std::cout << "shift called" << '\n';
    for(auto currItemP : parsed_registers) {
       i->items.push_back(currItemP);
       //std::cout << currItemP.labelName << ' '; //FOR TEST
    }
    //std::cout << '\n';
    parsed_registers.clear();
    currFunc->instructions.push_back(i);
    }
  };

  template<> struct action < goto_jump > {
    template < typename Input > static void apply (const Input &in, Program &p) {
    Instruction* i = new Instruction();
    auto currFunc = p.functions.back();
    //std::cout << "goto_jump action called\n";
    i->identifier = 6;
    for(auto currItemP : parsed_registers) {
       i->items.push_back(currItemP);
       //std::cout << currItemP.labelName << ' '; //FOR TEST
    }
    //std::cout << '\n';
    parsed_registers.clear();
    currFunc->instructions.push_back(i);
    }
  };

  template<> struct action < cjump_onearg > {
    template < typename Input > static void apply (const Input &in, Program &p) {
    Instruction* i = new Instruction();
    auto currFunc = p.functions.back();
    //std::cout << "cjump_onearg action called\n";
    i->identifier = 11;
    for(auto currItemP : parsed_registers) {
       i->items.push_back(currItemP);
    }
    parsed_registers.clear();
    currFunc->instructions.push_back(i);
    }
  };

  template<> struct action < runtime_func > {
    template < typename Input > static void apply (const Input &in, Program &p) {
    Item i;
    i.labelName = in.string();
    i.type = Type::runtime;
    parsed_registers.push_back(i);
    }
  };

  template<> struct action < call > {
    template < typename Input > static void apply (const Input &in, Program &p) {
    Instruction* i = new Instruction();
    auto currFunc = p.functions.back();
    i->identifier = 9;
    for(auto currItemP : parsed_registers) {
       i->items.push_back(currItemP);
       //std::cout << currItemP.labelName << ' '; //FOR TEST
    }
    parsed_registers.clear();
    currFunc->instructions.push_back(i);
    }
  };

  template<> struct action < label_instruction > {
    template < typename Input > static void apply (const Input &in, Program &p) {
    Item it;
    it.labelName = in.string();
    Instruction* i = new Instruction();
    auto currFunc = p.functions.back();
    //std::cout << "label_instruction called\n";
    i->identifier = 10;
    i->items.push_back(it);
    currFunc->instructions.push_back(i);
    parsed_registers.clear();
    }
  };

  /*
   * parse structures
   */

  Program parse_file (char *fileName){

    /* 
     * Check the grammar for some possible issues.
     */
    pegtl::analyze< grammar >();
    pegtl::analyze< function_grammar >();
    /*
     * Parse.
     */   
    file_input< > fileInput(fileName);
    Program p;
    parse< grammar, action >(fileInput, p);

    return p;
  }
}
