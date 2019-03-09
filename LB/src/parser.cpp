#include <string>
#include <vector>
#include <utility>
#include <algorithm>
#include <iterator>
#include <cstring>
#include <cctype>
#include <cstdlib>
#include <stdint.h>
#include <parser.h>
#include <iostream>

#include <LB.h>

namespace LB{
  
  /* 
   * Data required to parse
   */ 
  std::vector<Item*> parsed_items;
  std::vector<std::string> parsed_strings;
  std::string parsed_type = "";
  int num_dim = 0;

  Instruction* currI;
  std::vector<std::string> fp_list;
  Scope* currS = nullptr; // this scope will have instructions pushed into it
  std::vector<Scope*> scopeStack; // holds parent scopes
  int scope_level = -1; // function's base scope is level 0

  /* 
   * Actions attached to grammar rules.
   */
  template< typename Rule >
  struct action : pegtl::nothing< Rule > {};

  template<> struct action < Label_rule > {
    template< typename Input >
	static void apply( const Input & in, Program & p){
      auto i = new Item();
      i->labelName = in.string();
      i->itemType = Atomic_Type::label;
      parsed_items.push_back(i);
    }
  };

  template<> struct action < var > {
    template < typename Input >
    static void apply (const Input &in, Program &p) {
        auto i = new Item();
        i->itemType = Atomic_Type::var;
	    i->labelName = in.string();
	    parsed_items.push_back(i);
		if(currS)
          currS->varName_to_Item[in.string()] = i; //put var into currS's varName_to_Item map
    }
  };

  template<> struct action < number > {
    template < typename Input > static void apply (const Input &in, Program &p) {
      auto i = new Item();
      i->itemType = Atomic_Type::num;
      i->labelName = in.string();
      parsed_items.push_back(i);
    }
  };

  template<> struct action < assign > { // simple assign
    template < typename Input > static void apply (const Input &in, Program &p) {
		auto currF = p.functions.back();
        Instruction* i = new Instruction();
        i->Type = InstructionType::assign;
        for(auto& it : parsed_items) {
            i->Items.push_back(it);
        }
        parsed_items.clear();
        currF->func_scope->Instructions.push_back(i);
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

//  template<> struct action < assign_arithmetic > {
//    template < typename Input > static void apply (const Input &in, Program &p) {
//		auto currF = p.functions.back();
//        Instruction* i = new Instruction();
//        i->Type = InstructionType::assign_arithmetic;
//        for(auto& it : parsed_items) {
//            i->Items.push_back(it);
//        }
//        
//        std::string oper = parsed_strings.back();
//        if(oper == ">>")
//            i->Arith_Oper = Arith_Operator::shift_right; 
//        else if(oper == "<<")
//            i->Arith_Oper = Arith_Operator::shift_left; 
//        else if(oper == "+")
//            i->Arith_Oper = Arith_Operator::plus; 
//        else if(oper == "-")
//            i->Arith_Oper = Arith_Operator::minus; 
//        else if(oper == "*")
//            i->Arith_Oper = Arith_Operator::multiply; 
//        else
//            i->Arith_Oper = Arith_Operator::bw_and; 
//
//        parsed_items.clear();
//        parsed_strings.clear();
//
//        currF->instructions.push_back(i);
//    }
//  };

//  template<> struct action < assign_comparison > {
//    template < typename Input > static void apply (const Input &in, Program &p) {
//		auto currF = p.functions.back();
//        Instruction* i = new Instruction();
//        i->Type = InstructionType::assign_compare;
//        for(auto& it : parsed_items) {
//            i->Items.push_back(it);
//        }
//        
//        std::string oper = parsed_strings.back();
//        if(oper == ">")
//            i->Comp_Oper = Compare_Operator::gr; 
//        else if(oper == ">=")
//            i->Comp_Oper = Compare_Operator::geq; 
//        else if(oper == "<")
//            i->Comp_Oper = Compare_Operator::le; 
//        else if(oper == "<=")
//            i->Comp_Oper = Compare_Operator::leq; 
//        else
//            i->Comp_Oper = Compare_Operator::eq; 
//
//        parsed_items.clear();
//        parsed_strings.clear();
//
//        currF->instructions.push_back(i);
//    }
//  };
  
  template<> struct action < assign_load_array > {
    template < typename Input > static void apply (const Input &in, Program &p) {
		auto currF = p.functions.back();
        Instruction* i = new Instruction();
        i->Type = InstructionType::assign_load_array;
        i->Items.push_back(parsed_items[0]);
        i->Items.push_back(parsed_items[1]);
        for(int j = 2; j < parsed_items.size(); ++j) {
            i->array_access_location.push_back(parsed_items[j]);
        }
        currF->func_scope->Instructions.push_back(i);
        parsed_items.clear();
    }
  };

  template<> struct action < assign_store_array > {
    template < typename Input > static void apply (const Input &in, Program &p) {
		auto currF = p.functions.back();
        Instruction* i = new Instruction();
        i->Type = InstructionType::assign_store_array;
        i->Items.push_back(parsed_items[0]);
        i->Items.push_back(parsed_items.back());
        for(int j = 1; j < parsed_items.size() - 1; ++j) {
            i->array_access_location.push_back(parsed_items[j]);
        }
        currF->func_scope->Instructions.push_back(i);
        parsed_items.clear();
    }
  };

  template<> struct action < assign_length > {
    template < typename Input > static void apply (const Input &in, Program &p) {
		auto currF = p.functions.back();
        Instruction* i = new Instruction();
        i->Type = InstructionType::assign_length;
        for(auto& it : parsed_items) {
            i->Items.push_back(it);
        }

        currF->func_scope->Instructions.push_back(i);
        parsed_items.clear();
    }
  };

  template<> struct action < assign_new_array > {
    template < typename Input > static void apply (const Input &in, Program &p) {
		auto currF = p.functions.back();
        Instruction* i = new Instruction();
        i->Type = InstructionType::assign_new_array;

        i->Items.push_back(parsed_items[0]);
        for(int it = 1; it < parsed_items.size(); ++it) {
            i->arguments.push_back(parsed_items[it]); // dimensions and sizes of each dimension
        }

        currF->func_scope->Instructions.push_back(i);
        parsed_items.clear();
    }
  };

  template<> struct action < assign_new_tuple > {
    template < typename Input > static void apply (const Input &in, Program &p) {
		auto currF = p.functions.back();
        Instruction* i = new Instruction();
        i->Type = InstructionType::assign_new_tuple;

        i->Items.push_back(parsed_items[0]);
        i->arguments.push_back(parsed_items.back());

        currF->func_scope->Instructions.push_back(i);
        parsed_items.clear();
    }
  };

  template<> struct action < call > {
    template < typename Input > static void apply (const Input &in, Program &p) {
		auto currF = p.functions.back();
        Instruction* i = new Instruction();
        i->Type = InstructionType::call;

        // no runtime func in LB
        // handle vars and labels; know that parsed_items has exactly one thing
        auto it = parsed_items[0];
        i->Items.push_back(it);

		// if there is a function pointer decalred with the same name as labelname, this is var
		for(auto fp_var : fp_list) {
		  if(it->labelName == fp_var) {
			i->calleeType = CalleeType::var;
			break;
		  }
		}
		// Otherwise it is function name (label without :)
		if(i->calleeType == CalleeType::no_callee) {
		  i->calleeType = CalleeType::label;
		  it->itemType = Atomic_Type::label;
		}

        // we know that the first Item in parsed_items is the function label
        // therefore we can skip it
        for(int j = 1; j < parsed_items.size(); ++j)
            i->arguments.push_back(parsed_items[j]);

        parsed_items.clear();

        currF->func_scope->Instructions.push_back(i);
    }
  };

  template<> struct action < call_assign > {
    template < typename Input > static void apply (const Input &in, Program &p) {
		auto currF = p.functions.back();
        Instruction* i = new Instruction();
        i->Type = InstructionType::call_assign;

        //handle assignment to variable
        Item* destination = parsed_items[0];
        i->Items.push_back(destination);
        
        // no runtime func in LA
        // handle vars and labels; know that this is second thing in parsed_items
        Item* it = parsed_items[1];
        i->Items.push_back(it);

		// if there is a function pointer decalred with the same name as labelname, this is var
		for(auto fp_var : fp_list) {
		  if(it->labelName == fp_var) {
			i->calleeType = CalleeType::var;
			break;
		  }
		}
		// Otherwise it is function name (label without :)
		if(i->calleeType == CalleeType::no_callee) {
		  i->calleeType = CalleeType::label;
		  it->itemType = Atomic_Type::label;
		}

        for(int j = 2; j < parsed_items.size(); ++j)
            i->arguments.push_back(parsed_items[j]);

        parsed_items.clear();

        currF->func_scope->Instructions.push_back(i);
    }
  };

  template<> struct action < print > {
    template < typename Input > static void apply (const Input &in, Program &p) {
	  auto currF = p.functions.back();
      Instruction* i = new Instruction();
      i->Type = InstructionType::print;
      for(Item* it : parsed_items) {
          i->Items.push_back(it);
      }
      parsed_items.clear();

      currF->func_scope->Instructions.push_back(i);
    }
  };

  template<> struct action < label_instruction > {
    template < typename Input > static void apply (const Input &in, Program &p) {
	  auto currF = p.functions.back();
      Instruction* i = new Instruction();
      i->Type = InstructionType::label;
      for(Item* it : parsed_items) {
          i->Items.push_back(it);
      }
      parsed_items.clear();

      currF->func_scope->Instructions.push_back(i);
    }
  };

  template<> struct action < return_empty > {
    template< typename Input >
	static void apply( const Input & in, Program & p){
	  auto currF = p.functions.back();
      Instruction* i = new Instruction();
      i->Type = InstructionType::return_empty;

      currF->func_scope->Instructions.push_back(i);
    }
  };

  template<> struct action < return_value > {
    template< typename Input >
	static void apply( const Input & in, Program & p){
	  auto currF = p.functions.back();
      Instruction* i = new Instruction(); // return instruction struct
      i->Type = InstructionType::return_value;

      auto it = parsed_items.back(); // return __(it)__ 
      i->Items.push_back(it);
      parsed_items.clear();

      currF->func_scope->Instructions.push_back(i);
    }
  };

  template<> struct action < br_unconditional > {
    template < typename Input > static void apply (const Input &in, Program &p) {
	    auto currF = p.functions.back();
        Instruction* i = new Instruction();
        i->Type = InstructionType::br_unconditional;
        for(auto& it : parsed_items) {
            i->Items.push_back(it);
        }
        parsed_items.clear();

        currF->func_scope->Instructions.push_back(i);
    }
  };
  
  // t op t
  template<> struct action < operation > {
    template < typename Input > static void apply (const Input &in, Program &p) {
//	    auto currF = p.functions.back();
//
//        for(auto& it : parsed_items) {
//            i->Items.push_back(it);
//        }
//        parsed_items.clear();
//
//        currF->instructions.push_back(i);
    }
  };

  template<> struct action < if_instruction > {
    template < typename Input > static void apply (const Input &in, Program &p) {
	    auto currF = p.functions.back();
        Instruction* i = new Instruction();
        i->Type = InstructionType::if_instruction;
        // t, t, label1, label2
        for(auto& it : parsed_items) {
            i->Items.push_back(it);
        }
        parsed_items.clear();

        std::string oper = parsed_strings.back();
        if(oper == ">>")
            i->Operator = Oper::shift_right; 
        else if(oper == "<<")
            i->Operator = Oper::shift_left; 
        else if(oper == "+")
            i->Operator = Oper::plus; 
        else if(oper == "-")
            i->Operator = Oper::minus; 
        else if(oper == "*")
            i->Operator = Oper::multiply; 
        else if(oper == "&")
            i->Operator = Oper::bw_and; 
        else if(oper == ">")
            i->Operator = Oper::gr; 
        else if(oper == ">=")
            i->Operator = Oper::geq; 
        else if(oper == "<")
            i->Operator = Oper::le; 
        else if(oper == "<=")
            i->Operator = Oper::leq; 
        else
            i->Operator = Oper::eq; 

        parsed_items.clear();
        parsed_strings.clear();

        currF->func_scope->Instructions.push_back(i);
    }
  };

  template<> struct action < while_instruction > {
    template < typename Input > static void apply (const Input &in, Program &p) {
	    auto currF = p.functions.back();
        Instruction* i = new Instruction();
        i->Type = InstructionType::while_instruction;
        // t, t, label1, label2
        for(auto& it : parsed_items) {
            i->Items.push_back(it);
        }
        parsed_items.clear();

        std::string oper = parsed_strings.back();
        if(oper == ">>")
            i->Operator = Oper::shift_right; 
        else if(oper == "<<")
            i->Operator = Oper::shift_left; 
        else if(oper == "+")
            i->Operator = Oper::plus; 
        else if(oper == "-")
            i->Operator = Oper::minus; 
        else if(oper == "*")
            i->Operator = Oper::multiply; 
        else if(oper == "&")
            i->Operator = Oper::bw_and; 
        else if(oper == ">")
            i->Operator = Oper::gr; 
        else if(oper == ">=")
            i->Operator = Oper::geq; 
        else if(oper == "<")
            i->Operator = Oper::le; 
        else if(oper == "<=")
            i->Operator = Oper::leq; 
        else
            i->Operator = Oper::eq; 

        parsed_items.clear();
        parsed_strings.clear();

        currF->func_scope->Instructions.push_back(i);
    }
  };

  template<> struct action < continue_instruction > {
    template< typename Input >
        static void apply( const Input & in, Program & p){
            auto currF = p.functions.back();
            Instruction* i = new Instruction();
            i->Type = InstructionType::continue_instruction;
            currF->func_scope->Instructions.push_back(i);
        }
  };
    
  template<> struct action < break_instruction > {
    template< typename Input >
        static void apply( const Input & in, Program & p){
            auto currF = p.functions.back();
            Instruction* i = new Instruction();
            i->Type = InstructionType::break_instruction;
            currF->func_scope->Instructions.push_back(i);
         }
  };
        
  template<> struct action < int64_type > {
    template < typename Input >
    static void apply (const Input &in, Program &p) {
        parsed_type = in.string();
    }
  };

  template<> struct action < void_type > {
    template < typename Input >
    static void apply (const Input &in, Program &p) {
        parsed_type = in.string();
    }
  };

  template<> struct action < tuple_type > {
    template < typename Input >
    static void apply (const Input &in, Program &p) {
        parsed_type = in.string();
    }
  };

  template<> struct action < code_type > {
    template < typename Input >
    static void apply (const Input &in, Program &p) {
        parsed_type = in.string();
    }
  };

  template<> struct action < array_num_dim > {
    template < typename Input >
    static void apply (const Input &in, Program &p) {
        num_dim++;
    }
  };
 
  template<> struct action < array_type > {
    template < typename Input >
    static void apply (const Input &in, Program &p) {
        parsed_type = "array";
    }
  };

  template<> struct action < init_var > {
    template < typename Input >
    static void apply (const Input &in, Program &p) {
	    auto currF = p.functions.back();
        Instruction* i = new Instruction();
        i->Type = InstructionType::init_var;

        VarType type;
        if(parsed_type == "int64") {
		  type = VarType::int64_type;
        }
        else if(parsed_type == "tuple") {
		  type = VarType::tuple_type;
        }
        else if(parsed_type == "code") {
		  type = VarType::code_type;
        }
        else if(parsed_type == "array") { // array type
		  type = VarType::arr_type;
        }
        else {
          std::cerr << "Incorrect var type: " << parsed_type << '\n';
		}

        // all declarations are stored in parsed_items
        // all declarations have the same type
        for(auto& var : parsed_items) {
            if(type == VarType::arr_type)
		        var->numDimensions = num_dim;
            else if(type == VarType::code_type) {
		        // To process this in call function
		        fp_list.push_back(var->labelName);
            }

            var->varType = type; 
            i->Items.push_back(var);
        }

        num_dim = 0;
        parsed_type = "";
        parsed_items.clear();

        currF->func_scope->Instructions.push_back(i);
      }
  };

   template<> struct action < arg_var > {
    template < typename Input >
    static void apply (const Input &in, Program &p) {
        Function* currF = p.functions.back();
        Item* newItem = new Item();
        newItem->itemType = Atomic_Type::var;
        if(parsed_type == "int64") {
            newItem->varType = VarType::int64_type;
        }
        else if(parsed_type == "tuple") {
            newItem->varType = VarType::tuple_type;
            newItem->numDimensions = num_dim;
        }
        else if(parsed_type == "code") {
            newItem->varType = VarType::code_type;
        }
        else {
            newItem->varType = VarType::arr_type;
            newItem->numDimensions = num_dim;
        }

        newItem->labelName = in.string();
        num_dim = 0;
        currF->arguments.push_back(newItem);
    }
  };

   template<> struct action < Function_declare > {
    template < typename Input >
    static void apply (const Input &in, Program &p) {
      auto newF = new Function();

      // function level scope; parent is nullptr
      auto newS = new Scope(); 
      newS->level = scope_level; 
      newF->func_scope = newS;
      currS = newS;

	  // Function return type
      if(parsed_type == "int64") {
          newF->returnType = VarType::int64_type;
      }
      else if(parsed_type == "tuple") {
          newF->returnType = VarType::tuple_type;
      }
      else if(parsed_type == "code") {
          newF->returnType = VarType::code_type;
      }
      else if(parsed_type == "void") {
          newF->returnType = VarType::void_type;
      }
      else { // array type
          newF->returnType = VarType::arr_type;
		  newF->numDimensions = num_dim;
      }
      num_dim = 0;
      parsed_type = "";

	  // Function name
      newF->name = parsed_items.back()->labelName;
	  parsed_items.clear();

      p.functions.push_back(newF);

	  // Prepare for call
	  fp_list.clear();
    }
  };

   template<> struct action < scope_begin > {
       template < typename Input >
       static void apply(const Input &in, Program &p) {
           scope_level++; // we've gone down one level
           // level 0 is function scope; function scope created in Function_declare
           if(scope_level == 0)
              return;

           // new scope deeper than function scope
           auto newS = new Scope();
           newS->parent_scope = currS;
           scopeStack.push_back(currS);

           // set new scope to current scope
           currS = newS;
       }
   };
   
   template<> struct action < scope_end > {
       template < typename Input >
       static void apply(const Input &in, Program &p) {
         scope_level--; // we've gone up one level
         std::cerr << "scope end called";

		 if(scopeStack.size() > 0) { // scope above function scope exists
           auto prevS = scopeStack.back(); 
           scopeStack.pop_back();
           prevS->children_scopes.push_back(currS); // store current scope into higher level scope
           
           // set higher level scope to current scope
           currS = prevS;
		 }
       }
   };

  //Scope* currS = nullptr; // this scope will have instructions pushed into it
  //std::vector<Scope*> scopeStack; // holds parent scopes

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
