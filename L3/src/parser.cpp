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
#include <parser.h>

namespace L3 {
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
      if(newF->name == "main")
          newF->isMain = true;

      p.functions.push_back(newF);
    }
  };

  template<> struct action < argument > {
      template< typename Input >
          static void apply(const Input& in, Program& p) {
              Item it = new Item();
              it->labelName = in.string();
              it->type = Type::var;
              auto curr_F = p.functions.back();
              curr_F->arguments.push_back(it);
          }
  }

  template<> struct action < return_empty > {
    template< typename Input >
	static void apply( const Input & in, Program & p){
      auto currentF = p.functions.back();
      auto i = new Instruction(); // return instruction struct
      currentF->instructions.push_back(i);
    }
  };

  template<> struct action < return_value > {
    template< typename Input >
	static void apply( const Input & in, Program & p){
      auto currentF = p.functions.back();
      auto i = new Instruction(); // return instruction struct
      auto it = parsed_items.back();
      i->Items.push_back(it);
      parsed_item.clear();
      currentF->instructions.push_back(i);
    }
  };

  template<> struct action < Label_rule > {
    template< typename Input >
	static void apply( const Input & in, Program & p){
      Item* i = new Item();
      i->labelName = in.string();
      i->type = Type::label;
      parsed_items.push_back(i);
    }
  };

  /*
   * Our work
   */

  template<> struct action < var > {
    template < typename Input >
    static void apply (const Input &in, Program &p) {
        Item* i = new Item();
        i->type = Type::var;
	    i->labelName = in.string();
	    parsed_items.push_back(i);
    }
  };

  template<> struct action < assign_operator > {
    template < typename Input > static void apply (const Input &in, Program &p) {
       Item* i -> new Item();
       i->labelName = in.string();
       parsed_registers.push_back(i);
    }
  };

  template<> struct action < number > {
    template < typename Input > static void apply (const Input &in, Program &p) {
      Item* i = new item();
      i->number.value = std::stoi(in.string());
      i->type = Type::num;
      i->number.toString = in.string();
      parsed_items.push_back(i);
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
	Item* it = new Item();
	i->labelName = in.string();
    i->type = Type::arith_oper;
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

  template<> struct action < comparison_operator > {
    template < typename Input > static void apply (const Input &in, Program &p) {
	  Item* i = new Item();
	  i->labelName = in.string();
      i->type = Type::compare_oper;
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
	}
	parsed_registers.clear();
	currFunc->instructions.push_back(i);
    }
  };

  template<> struct action < goto_jump > {
    template < typename Input > static void apply (const Input &in, Program &p) {
    Instruction* i = new Instruction();
    auto currFunc = p.functions.back();
    i->identifier = 6;
    for(auto currItemP : parsed_registers) {
       i->items.push_back(currItemP);
    }
    parsed_registers.clear();
    currFunc->instructions.push_back(i);
    }
  };

  template<> struct action < cjump_onearg > {
    template < typename Input > static void apply (const Input &in, Program &p) {
    Instruction* i = new Instruction();
    auto currFunc = p.functions.back();
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
    Item* i = new Item();
    i->labelName = in.string();
    i->type = Type::runtime;
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
    i->identifier = 10;
    i->items.push_back(it);
    currFunc->instructions.push_back(i);
    parsed_registers.clear();
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
