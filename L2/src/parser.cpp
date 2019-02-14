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

namespace L2 {

  /* 
   * Data required to parse
   */ 
  std::vector<Item> parsed_registers;
  std::vector<std::string> parsed_labels;
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

  struct l1_keyword:
       pegtl::sor<
          pegtl::string<'c', 'a', 'l', 'l'>,
          pegtl::string<'c', 'j', 'u', 'm', 'p'>,
          pegtl::string<'g','o','t','o'>,
          pegtl::one<'@'>
       > {};

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
      pegtl::string<'r','s','p'>, 
      pegtl::string<'r','b','p'>,
      var
    > {};

  struct mem:
	  pegtl::string<'m', 'e', 'm'> {};

  struct mem_op:
	  pegtl::seq<mem, seps, reg, seps, number> {};

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
		seps,
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

  struct stack_arg_keyword:
        pegtl::string<'s', 't', 'a', 'c', 'k', '-', 'a', 'r', 'g'> {};

  struct stack_arg_instruction:
      pegtl::seq<
        seps,
        reg,
        seps,
        assign_operator,
        seps,
        stack_arg_keyword,
        seps,
        number,
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
	 seps,
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
     pegtl::eol
   > {};

 struct cjump_twoargs:
  pegtl::seq<
    seps,
    l1_keyword, // cjump
    seps,
    compare,
    seps,
    Label_rule,
    seps,
    Label_rule,
    pegtl::eol
  > {};

//  struct cjump:
//    pegtl::sor<
//      pegtl::seq<pegtl::at<cjump_onearg>, cjump_onearg>,
//      pegtl::seq<pegtl::at<cjump_twoargs>, cjump_twoargs>
//    > {};

 struct lea:
   pegtl::seq<
     seps,
     reg,
     seps,
     l1_keyword, // @
     seps,
     reg,
     seps,
     reg,
     seps,
     number,
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
  l1_keyword,
  seps,
  pegtl::sor<
   Label_rule,
   runtime_func,
   reg
  >,
  seps,
  number
   > {};

  struct Instruction_rule:
    pegtl::sor<
    pegtl::seq< pegtl::at<Instruction_return_rule>, Instruction_return_rule>, 
    pegtl::seq< pegtl::at<assign_comparison>, assign_comparison>,
    pegtl::seq< pegtl::at<label_instruction>, label_instruction>,
    pegtl::seq< pegtl::at<cjump_onearg>, cjump_onearg>,
    pegtl::seq< pegtl::at<cjump_twoargs>, cjump_twoargs>,
    pegtl::seq<pegtl::at<stack_arg_instruction>, stack_arg_instruction>,
    pegtl::seq< pegtl::at<call>, call>,
    pegtl::seq< pegtl::at<lea>, lea>,
    pegtl::seq< pegtl::at<assignment>, assignment>,
    pegtl::seq< pegtl::at<arithmetic>, arithmetic>,
    pegtl::seq< pegtl::at<inc_dec>, inc_dec>,
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
  
  struct spill_arg:
	var {};

  struct spill_args:
	pegtl::seq<
	  spill_arg,
	  seps,
	  spill_arg
	> {};

  struct spill_grammar:
    pegtl::must<  
      pegtl::seq<
        Function_rule,
		seps,
        spill_args,
		seps
      >
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
      Item it;
      it.labelName = "return";
      i->items.push_back(it);
      std::cout << "Return called\n";
      currentF->instructions.push_back(i);
    }
  };

  template<> struct action < Label_rule > {
    template< typename Input >
	static void apply( const Input & in, Program & p){
      Item i;
      std::cout << "Label_rule action called.\n"; 
      i.labelName = in.string();
      i.type = Type::label;
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
      //std::cout << "reg action called\n";
      if(in.string()[0] == 'r')
        i.type = Type::reg;
      else
        i.type = Type::var;
	    i.labelName = in.string();
	    parsed_registers.push_back(i);
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

  template<> struct action < mem > {
    template < typename Input > static void apply (const Input &in, Program &p) {
	    Item i;
      i.type = Type::mem;
      i.labelName = in.string();
	    parsed_registers.push_back(i);
    }
  };

  template<> struct action < number > {
    template < typename Input > static void apply (const Input &in, Program &p) {
	    Item i;
      i.labelName = in.string();
      i.type = Type::num;
      //std::cout << "number action called.\n";
	    parsed_registers.push_back(i);
    }
  };

  template<> struct action < assignment > {
    template < typename Input > static void apply (const Input &in, Program &p) {
	    auto currFunc = p.functions.back();
      Instruction* i = new Instruction(); // instruction will be reversed when generating x86
      i->identifier = 0;
      std::cout << "ASSIGNMENT\n";
      for(std::vector<Item>::iterator it = parsed_registers.begin(); it != parsed_registers.end(); ++it) {
  	    auto currItemP = it;
	    i->items.push_back(*currItemP);
	    std::cout << it->labelName << ' ';
      }
      currFunc->instructions.push_back(i);
      parsed_registers.clear(); 
      std::cout << '\n';
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

  template<> struct action < inc_dec_operator > {
    template < typename Input > static void apply (const Input &in, Program &p) {
	Item i;
	i.labelName = in.string();
    i.type = Type::inc_dec_oper;
	parsed_registers.push_back(i);
    }
  };

  template<> struct action < inc_dec > {
    template < typename Input > static void apply (const Input &in, Program &p) {
	Instruction* i = new Instruction();
	auto currFunc = p.functions.back();
    std::cout << "inc_dec called" << ' ';
	i->identifier = 3;
	for(auto currItemP : parsed_registers) {
       std::cout << currItemP.labelName << ' ';
	   i->items.push_back(currItemP);
	}
    std::cout << '\n';
	parsed_registers.clear();
	currFunc->instructions.push_back(i);
    }
  };

  template<> struct action < comparison_operator > {
    template < typename Input > static void apply (const Input &in, Program &p) {
	  Item i;
	  i.labelName = in.string();
      i.type = Type::compare_oper;
      std::cout << "comparison operator called\n";
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
	   std::cout << currItemP.labelName << ' ';
	}
    std::cout << '\n';
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
    std::cout << "cjump_onearg action called\n";
    i->identifier = 11;
    for(auto currItemP : parsed_registers) {
       i->items.push_back(currItemP);
    }
    parsed_registers.clear();
    currFunc->instructions.push_back(i);
    }
  };

  template<> struct action < cjump_twoargs > {
    template < typename Input > static void apply (const Input &in, Program &p) {
    Instruction* i = new Instruction();
    auto currFunc = p.functions.back();
    std::cout << "cjump_twoarg action called\n";
    i->identifier = 7;
    for(auto currItemP : parsed_registers) {
       i->items.push_back(currItemP);
    }
    parsed_registers.clear();
    currFunc->instructions.push_back(i);
    }
  };

  template<> struct action < lea > {
    template < typename Input > static void apply (const Input &in, Program &p) {
    Instruction* i = new Instruction();
    auto currFunc = p.functions.back();
    i->identifier = 8;
    for(auto currItemP : parsed_registers) {
       i->items.push_back(currItemP);
       //std::cout << currItemP.labelName << ' '; //FOR TEST
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

  template<> struct action < l1_keyword > {
    template < typename Input > static void apply (const Input &in, Program &p) {
      Item i;
      i.labelName = in.string();
      //std::cout << "l1_keyword action called.\n";
      parsed_registers.push_back(i);
    }
  };

  template<> struct action < stack_arg_keyword > {
    template < typename Input > static void apply (const Input &in, Program &p) {
      Item i;
      i.labelName = in.string();
      parsed_registers.push_back(i);
    }
  };

  template<> struct action < stack_arg_instruction > {
    template < typename Input > static void apply (const Input &in, Program &p) {
    Instruction* i = new Instruction();
    auto currFunc = p.functions.back();
    i->identifier = 12;
    for(auto currItemP : parsed_registers) {
       i->items.push_back(currItemP);
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

  template<> struct action < spill_arg > {
    template < typename Input > static void apply (const Input &in, Program &p) {
	  Item i;
      i.type = Type::var;
      i.labelName = in.string();
	  parsed_registers.push_back(i);
    }
  };

  template<> struct action < spill_args > {
    template < typename Input > static void apply (const Input &in, Program &p) {
      for(auto currItemP : parsed_registers) {
		p.spill_extras.push_back(currItemP);
       //std::cout << currItemP.labelName << ' '; //FOR TEST
	  }
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

  Program parse_function (char *fileName) {
    pegtl::analyze<function_grammar>();
    
    file_input< > fileInput(fileName);
    Program p;
    parse< function_grammar, action>(fileInput, p);

    return p;
  }

  Program spill_function (char *fileName) {
    pegtl::analyze<spill_grammar>();
    
    file_input< > fileInput(fileName);
    Program p;
    parse< spill_grammar, action>(fileInput, p);

    return p;

  }
}
