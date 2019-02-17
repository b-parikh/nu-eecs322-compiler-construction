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
   * Data required to parse
   */ 
  std::vector<Item> parsed_items;
  std::vector<std::string> parsed_strings;
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
              it->Type = Atomic_Type::var;
              auto curr_F = p.functions.back();
              curr_F->arguments.push_back(it);
          }
  }

  template<> struct action < return_empty > {
    template< typename Input >
	static void apply( const Input & in, Program & p){
      Instruction_return_empty* i = new Instruction_return_empty();
      auto currentF = p.functions.back();
      currentF->instructions.push_back(i);
    }
  };

  template<> struct action < return_value > {
    template< typename Input >
	static void apply( const Input & in, Program & p){
      auto currentF = p.functions.back();
      auto i = new Instruction_return_value(); // return instruction struct
      auto it = parsed_items.back(); // return __(it)__ 
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
      i->Type = Atomic_Type::label;
      parsed_items.push_back(i);
    }
  };

  template<> struct action < var > {
    template < typename Input >
    static void apply (const Input &in, Program &p) {
        Item* i = new Item();
        i->Type = Atomic_Type::var;
	    i->labelName = in.string();
	    parsed_items.push_back(i);
    }
  };

  template<> struct action < number > {
    template < typename Input > static void apply (const Input &in, Program &p) {
      Item* i = new item();
      i->number.value = std::stoi(in.string());
      i->Type = Atomic_Type::num;
      i->number.toString = in.string();
      parsed_items.push_back(i);
    }
  };

  template<> struct action < assign > { // simple assign
    template < typename Input > static void apply (const Input &in, Program &p) {
	    auto currFunc = p.functions.back();
        auto i = new Instruction_assign();
        for(auto& it : parsed_items) {
            i->Items.push_back(it);
        }
        parsed_items.clear();
	    i->items.push_back(*currItemP);
        currFunc->instructions.push_back(i);
    }
  };

  template<> struct action < arithmetic_operator > {
    template < typename Input > static void apply (const Input &in, Program &p) {
	    parsed_strings.push_back(in.string());
    }
  };

  template<> struct action < comparison_operator > {
    template < typename Input > static void apply (const Input &in, Program &p) {
	    parsed_strings.push_back(in.string());
    }
  };

  template<> struct action < assign_arithmetic > {
    template < typename Input > static void apply (const Input &in, Program &p) {
	    auto currFunc = p.functions.back();
        auto i = new Instruction_assign_arithmetic();
        for(auto& it : parsed_items) {
            i->Items.push_back(it);
        }
        
        std::string oper = parsed_strings.back();
        if(oper == ">>")
            i->Oper = Arith_Operator::shift_right; 
        else if(oper == "<<")
            i->Oper = Arith_Operator::shift_left; 
        else if(oper == "+")
            i->Oper = Arith_Operator::plus; 
        else if(oper == "-")
            i->Oper = Arith_Operator::minus; 
        else if(oper == "*")
            i->Oper = Arith_Operator::multiply; 
        else
            i->Oper = Arith_Operator::bw_and; 

        parsed_items.clear();
        parsed_strings.clear();
	    i->items.push_back(*currItemP);
        currFunc->instructions.push_back(i);
    }
  };

  template<> struct action < assign_comparison > {
    template < typename Input > static void apply (const Input &in, Program &p) {
	    auto currFunc = p.functions.back();
        auto i = new Instruction_assign_comparison();
        for(auto& it : parsed_items) {
            i->Items.push_back(it);
        }
        
        std::string oper = parsed_strings.back();
        if(oper == ">")
            i->Oper = Compare_Operator::gr; 
        else if(oper == ">=")
            i->Oper = Compare_Operator::geq; 
        else if(oper == "<")
            i->Oper = Compare_Operator::le; 
        else if(oper == "<=")
            i->Oper = Compare_Operator::leq; 
        else
            i->Oper = Compare_Operator::eq; 

        parsed_items.clear();
        parsed_strings.clear();
	    i->items.push_back(currItemP);
        currFunc->instructions.push_back(i);
    }
  };
  
  template<> struct action < assign_load > {
    template < typename Input > static void apply (const Input &in, Program &p) {
	    auto currFunc = p.functions.back();
        auto i = new Instruction_assign_load();
        for(auto& it : parsed_items) {
            i->Items.push_back(it);
        }
        parsed_items.clear();
	    i->items.push_back(*currItemP);
        currFunc->instructions.push_back(i);
    }
  };

  template<> struct action < assign_store > {
    template < typename Input > static void apply (const Input &in, Program &p) {
	    auto currFunc = p.functions.back();
        auto i = new Instruction_assign_store();
        for(auto& it : parsed_items) {
            i->Items.push_back(it);
        }
        parsed_items.clear();
	    i->items.push_back(*currItemP);
        currFunc->instructions.push_back(i);
    }
  };

  template<> struct action < br_unconditional > {
    template < typename Input > static void apply (const Input &in, Program &p) {
	    auto currFunc = p.functions.back();
        auto i = new Instruction_br_unconditional();
        for(auto& it : parsed_items) {
            i->Items.push_back(it);
        }
        parsed_items.clear();
	    i->items.push_back(*currItemP);
        currFunc->instructions.push_back(i);
    }
  };
  
  template<> struct action < br_conditional > {
    template < typename Input > static void apply (const Input &in, Program &p) {
	    auto currFunc = p.functions.back();
        auto i = new Instruction_br_conditional();
        for(auto& it : parsed_items) {
            i->Items.push_back(it);
        }
        parsed_items.clear();
	    i->items.push_back(*currItemP);
        currFunc->instructions.push_back(i);
    }
  };

  template<> struct action < call > {
    template < typename Input > static void apply (const Input &in, Program &p) {
	    auto currFunc = p.functions.back();
        auto i = new Instruction_call();

        // handle runtime function calls
        if(!parsed_strings.empty()) {
            std::string runtime = parsed_strings.back();

            if(runtime == "print")
                i->Type = Instruction_call::CalleeType::print;
            else if(runtime == "allocate")
                i->Type = Instruction_call::CalleeType::allocate;
            else // array-error
                i->Type = Instruction_call:CalleeType::array-error;
        }

        // handle vars and labels; know that parsed_items has exactly one thing
        else {
            auto it = parsed_items.back();
            i->Items.push_back(it);
            if(it->Type == Atomic_Type::var)
               i->Type = CalleeType::var;
            else(it->Type == Atomic_Type::label)
               i->Type - CalleeType::label; 
        }

        parsed_items.clear();
        parsed_strings.clear();
	    i->items.push_back(*currItemP);
        currFunc->instructions.push_back(i);
    }
  };

  template<> struct action < call_assign > {
    template < typename Input > static void apply (const Input &in, Program &p) {
	    auto currFunc = p.functions.back();
        auto i = new Instruction_call();

        //handle assignment to variable
        auto destination = parsed_items[0];
        i->items.push_back(destination);
        
        // handle runtime function calls
        if(!parsed_strings.empty()) {
            std::string runtime = parsed_strings.back();
            if(runtime == "print")
                i->Type = Instruction_call::CalleeType::print;
            else if(runtime == "allocate")
                i->Type = Instruction_call::CalleeType::allocate;
            else // array-error
                i->Type = Instruction_call:CalleeType::array-error;
        }

        // handle vars and labels; know that this is second thing in parsed_items
        else {
            auto it = parsed_items.back();
            i->Items.push_back(it);
            if(it->Type == Atomic_Type::var)
               i->Type = CalleeType::var;
            else(it->Type == Atomic_Type::label)
               i->Type - CalleeType::label; 
        }

        parsed_items.clear();
        parsed_strings.clear();
	    i->items.push_back(*currItemP);
        currFunc->instructions.push_back(i);
    }
  };

  template<> struct action < runtime_func > {
    template < typename Input > static void apply (const Input &in, Program &p) {
    Item* i = new Item();
    i->labelName = in.string();
    // empty type; Instruction_runtime holds the function type information
    parsed_registers.push_back(i);
    }
  };

  template<> struct action < call > {
    template < typename Input > static void apply (const Input &in, Program &p) {
    Instruction* i = new Instruction();
    auto currFunc = p.functions.back();
    for(auto currItemP : parsed_registers) {
       i->items.push_back(currItemP);
    }
    parsed_registers.clear();
    currFunc->instructions.push_back(i);
    }
  };

  template<> struct action < label_instruction > {
    template < typename Input > static void apply (const Input &in, Program &p) {
	    auto currFunc = p.functions.back();
        auto i = new Instruction_label_instruction();
        for(auto& it : parsed_items) {
            i->Items.push_back(it);
        }
        parsed_items.clear();
	    i->items.push_back(*currItemP);
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