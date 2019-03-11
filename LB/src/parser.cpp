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
  std::string currFuncName = "";

  Instruction* currI;
  std::vector<std::string> fp_list;
  Scope* currS = nullptr; // this scope will have instructions pushed into it
  std::vector<Scope*> scopeStack; // holds parent scopes
  int scope_level = -1; // function's base scope is level 0

  Oper get_oper_enum(std::string oper) {
	Oper oper_enum;

    if(oper == ">>")
        oper_enum = Oper::shift_right; 
    else if(oper == "<<")
        oper_enum = Oper::shift_left; 
    else if(oper == "+")
		oper_enum = Oper::plus; 
    else if(oper == "-")
        oper_enum = Oper::minus; 
    else if(oper == "*")
        oper_enum = Oper::multiply; 
    else if(oper == "&")
        oper_enum = Oper::bw_and; 
    else if(oper == ">")
        oper_enum = Oper::gr; 
    else if(oper == ">=")
		oper_enum = Oper::geq; 
    else if(oper == "<")
        oper_enum = Oper::le; 
    else if(oper == "<=")
        oper_enum = Oper::leq; 
    else
        oper_enum = Oper::eq; 

    return oper_enum;
  }

  /* 
   * Actions attached to grammar rules.
   */
  template< typename Rule >
  struct action : pegtl::nothing< Rule > {};

  template<> struct action < Label_rule > {
    template< typename Input >
	static void apply( const Input & in, Program & p){
      // no need to save this into the name to Item map as it isn't a variable
      auto i = new Item();
      i->labelName = in.string();
      i->itemType = Atomic_Type::label;
      parsed_items.push_back(i);
    }
  };

  /*
   * For all vars that aren't function arguments.
   * If the var is within init_var instruction then create new Item and store in scope map (actual storing done in init_var action).
   * If the var has already been initialized inside curr scope, then just use the Item created by a previous initialization (recall Item from scope map).
   */
  template<> struct action < var > {
    template < typename Input >
    static void apply (const Input &in, Program &p) {
        std::cerr << "var action for " << in.string() << '\n';
        // at function header, so no scope has been declared; this is for function arguments
        // function arguments will be completely dealt with in arg_var
        if(currS == nullptr) {
//            auto i = new Item();
//            i->itemType = Atomic_Type::var;
//    	      i->labelName = in.string();
//            parsed_items.push_back(i);
            return;
        }

        // in the middle of a scope
        auto isItemInScope = currS->varName_to_Item.find(in.string());
        // if the item is already initialized in the scope then don't create a new Item and return the already created Item
        if(isItemInScope != currS->varName_to_Item.end()) {
            parsed_items.push_back(isItemInScope->second);
        }
        // create new item as this is the first time the Item has been encountered (init_var)
        else {
            auto i = new Item();
            i->itemType = Atomic_Type::var;
    	    i->labelName = in.string();
    	    parsed_items.push_back(i);
        }
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
        currS->Instructions.push_back(i);
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

  template<> struct action < assign_operation > {
    template < typename Input > static void apply (const Input &in, Program &p) {
		auto currF = p.functions.back();
        Instruction* i = new Instruction();
        i->Type = InstructionType::assign_operation;
        for(auto& it : parsed_items) {
            i->Items.push_back(it);
        }

        std::string oper = parsed_strings.back();
		i->Operator = get_oper_enum(oper);

        parsed_items.clear();
        parsed_strings.clear();

        currS->Instructions.push_back(i);
    }
  };

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
        currS->Instructions.push_back(i);
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
        currS->Instructions.push_back(i);
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

        currS->Instructions.push_back(i);
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

        currS->Instructions.push_back(i);
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

        currS->Instructions.push_back(i);
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

        currS->Instructions.push_back(i);
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

        currS->Instructions.push_back(i);
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

      currS->Instructions.push_back(i);
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

      currS->Instructions.push_back(i);
    }
  };

  template<> struct action < return_empty > {
    template< typename Input >
	static void apply( const Input & in, Program & p){
      Instruction* i = new Instruction();
      auto it = new Item();
      it->labelName = "return";
      i->Items.push_back(it);
      i->Type = InstructionType::return_empty;

      currS->Instructions.push_back(i);
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

      currS->Instructions.push_back(i);
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

        currS->Instructions.push_back(i);
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

        std::string oper = parsed_strings.back();
		i->Operator = get_oper_enum(oper);

        parsed_items.clear();
        parsed_strings.clear();

        currS->Instructions.push_back(i);
    }
  };

  template<> struct action < while_instruction > {
    template < typename Input > static void apply (const Input &in, Program &p) {
	    //auto currF = p.functions.back();
        Instruction* i = new Instruction();
        i->Type = InstructionType::while_instruction;
        // t, t, label1, label2
        for(auto& it : parsed_items) {
            i->Items.push_back(it);
        }

        std::string oper = parsed_strings.back();
		i->Operator = get_oper_enum(oper);

        parsed_items.clear();
        parsed_strings.clear();

        currS->Instructions.push_back(i);
    }
  };

  template<> struct action < continue_instruction > {
    template< typename Input >
        static void apply( const Input & in, Program & p){
            auto currF = p.functions.back();
            Instruction* i = new Instruction();
            i->Type = InstructionType::continue_instruction;
            currS->Instructions.push_back(i);
        }
  };
    
  template<> struct action < break_instruction > {
    template< typename Input >
        static void apply( const Input & in, Program & p){
            auto currF = p.functions.back();
            Instruction* i = new Instruction();
            i->Type = InstructionType::break_instruction;
            currS->Instructions.push_back(i);
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

        // all Items have already been created in var action and are stored in parsed_items
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
            // put var into scope's map of var name to Item*
            if(currS->varName_to_Item.find(var->labelName) != currS->varName_to_Item.end())
                std::cerr << "Var already exists in this scope. Why are you reinitializing it, you twat.\n";
            else 
                currS->varName_to_Item[var->labelName] = var;
        }

        num_dim = 0;
        parsed_type = "";
        parsed_items.clear();

        currS->Instructions.push_back(i);
      }
  };

  /*
   * For function arguments.
   * Creates a new Item for the function argument.
   * Stores the Item within arguments vector.
   * Stores the Item within scope map.
   */
   template<> struct action < arg_var > {
    template < typename Input >
    static void apply (const Input &in, Program &p) {
        std::cerr << "in arg_var\n";
        std::cerr << in.string() << "\n";
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

        Function* currF = p.functions.back();
        currF->arguments.push_back(newItem);
        
        if(currF->func_scope == nullptr) {
            std::cerr << "nullptr dereference in arg_var.\n";
        } else {
            // store the new argument inside the scope's map
            currS->varName_to_Item[newItem->labelName] = newItem;
        }
    }
  };

  template<> struct action < function_name > {
    template < typename Input >
    static void apply (const Input &in, Program &p) {
        currFuncName = in.string();
    }
  };

   template<> struct action < Function_declare > {
    template < typename Input >
    static void apply (const Input &in, Program &p) {
      //std::cerr << "In function_declare for " << in.string() << "\n";
      auto newF = new Function();
      // create function scope here so we can immediately push function args into it
      auto newS = new Scope(); 
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
      //newF->name = parsed_items.back()->labelName;
      newF->name = currFuncName;
      //std::cerr << "newF name is " << currFuncName << ".\n";
      currFuncName = "";

      p.functions.push_back(newF);

	  // Prepare for call
	  fp_list.clear();
    }
  };

   template<> struct action < scope_begin > {
       template < typename Input >
       static void apply(const Input &in, Program &p) {
           std::cerr << "scope begin\n";
           scope_level++; // we've gone down one level

           // level 0 is function scope and now taken care of in Function_declare
           if(scope_level == 0) {
               auto i = new Instruction();
               i->Type = InstructionType::scope_end;
               auto it = new Item();
               it->labelName = in.string();
               i->Items.push_back(it);
               currS->Instructions.push_back(i);

               // function level scope's parent is nullptr
               //newS->level = scope_level; 
               //newF->func_scope = newS;
               //currS = newS;
            
               // taking care of function arguments 
//               for(auto& itp : parsed_items) {
//                   currS->varName_to_Item[itp->labelName] = itp;
//               }
//               parsed_items.clear();
               return;
           }

           else {
               // store the new scope as an instruction to know when to go to child scope
//               Instruction* i = new Instruction();
//               i->Type = InstructionType::scope_begin;
//               currS->Instructions.push_back(i);

               auto newS = new Scope();
               newS->parent_scope = currS; // new scope deeper than function scope
               scopeStack.push_back(currS); // store currS for later
               currS = newS;
           }
       }
   };
   
//   template<> struct action < scope_begin_instruction > {
//     template< typename Input >
//         static void apply( const Input & in, Program & p){
//             auto currF = p.functions.back();
//             Instruction* i = new Instruction();
//             i->Type = InstructionType::scope_begin;
//             currS->Instructions.push_back(i);
//         }
//   };

    template<> struct action < scope_end > {
       template < typename Input >
       static void apply(const Input &in, Program &p) {
         //std::cerr << currS->Instructions.size() << '\n';
         Instruction* i = new Instruction();
         i->Type = InstructionType::scope_end;
         auto newItem = new Item();
         newItem->labelName = in.string();
         i->Items.push_back(newItem);
         currS->Instructions.push_back(i);

         std::cerr << "scope level: " << scope_level << ".\n";
         for(auto& ip : currS->Instructions) {
             for(auto& itp : ip->Items) {
                 std::cerr << itp->labelName << ' ';
             }
             std::cerr << '\n';
         }
         
         scope_level--; // we've gone up one level
		 if(scopeStack.size() > 0) { // scope above function scope exists
           auto prevS = scopeStack.back(); 
           //std::cerr << "New upper level has " << prevS->children_scopes.size() << " child scopes before push back.\n";
           scopeStack.pop_back();
           prevS->children_scopes.push_back(currS); // store current scope into higher level scope
           //std::cerr << "New upper level has " << prevS->children_scopes.size() << " child scopes after push back.\n";
           //std::cerr << prevS->children_scopes[0]->Instructions.size() << '\n';
           
           // set higher level scope to current scope
           currS = prevS;

           //std::cerr << currS->Instructions.size() << '\n';
		 }
       std::cerr << "scope end called\n";
       }
   };

//   template<> struct action < scope_end_instruction > {
//     template< typename Input >
//         static void apply( const Input & in, Program & p){
//             Instruction* i = new Instruction();
//             i->Type = InstructionType::scope_end;
//             currS->Instructions.push_back(i);
//         }
//   };

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
