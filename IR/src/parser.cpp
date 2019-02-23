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
#include <parser.h>
#include <map>

#include <IR.h>

namespace IR{
  
  /* 
   * Data required to parse
   */ 
  std::vector<Item*> parsed_items;
  std::vector<std::string> parsed_strings;
  std::string parsed_type = "";
  int num_dim = 0;

  //var_Item parsed_Arg;
  std::map<label_Item, basic_block> label_to_block;
  basic_block* block_buffer;

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

  template<> struct action < argument_define > {
      template< typename Input >
          static void apply(const Input& in, Program& p) {
              auto it = new Item();
              it->labelName = in.string();
              it->Type = Atomic_Type::var;
              auto curr_F = p.functions.back();
              curr_F->arguments.push_back(it);
          }
  };

  template<> struct action < return_empty > {
    template< typename Input >
	static void apply( const Input & in, Program & p){
      Instruction* i = new Instruction();
      i->Type = InstructionType::return_empty;
      auto currFunc = p.functions.back();

      //attach_pred_succ(i, currFunc);
      currFunc->instructions.push_back(i);
    }
  };

  template<> struct action < return_value > {
    template< typename Input >
	static void apply( const Input & in, Program & p){
      auto currentF = p.functions.back();
      Instruction* i = new Instruction(); // return instruction struct
      i->Type = InstructionType::return_value;
      auto it = parsed_items.back(); // return __(it)__ 
      i->Items.push_back(it);
      parsed_items.clear();

      currentF->instructions.push_back(i);
    }
  };

  template<> struct action < Label_rule > {
    template< typename Input >
	static void apply( const Input & in, Program & p){
      auto i = new Item();
      i->labelName = in.string();
      i->Type = Atomic_Type::label;
      parsed_items.push_back(i);
    }
  };

  template<> struct action < var > {
    template < typename Input >
    static void apply (const Input &in, Program &p) {
        auto i = new Item();
        i->Type = Atomic_Type::var;
	    i->labelName = in.string();
	    parsed_items.push_back(i);
    }
  };

  template<> struct action < number > {
    template < typename Input > static void apply (const Input &in, Program &p) {
      auto i = new Item();
      i->Type = Atomic_Type::num;
      i->labelName = in.string();
      parsed_items.push_back(i);
    }
  };

  template<> struct action < assign > { // simple assign
    template < typename Input > static void apply (const Input &in, Program &p) {
	    auto currFunc = p.functions.back();
        Instruction* i = new Instruction();
        i->Type = InstructionType::assign;
        for(auto& it : parsed_items) {
            i->Items.push_back(it);
        }
        parsed_items.clear();
        //attach_pred_succ(i, currFunc);
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

//  template<> struct action < > {
//    template < typename Input > static void apply (const Input &in, Program &p) {
//        std::cerr << "runtime called: " << in.string() << '\n';
//    }
//  };

  template<> struct action < assign_arithmetic > {
    template < typename Input > static void apply (const Input &in, Program &p) {
	    auto currFunc = p.functions.back();
        Instruction* i = new Instruction();
        i->Type = InstructionType::assign_arithmetic;
        for(auto& it : parsed_items) {
            i->Items.push_back(it);
        }
        
        std::string oper = parsed_strings.back();
        if(oper == ">>")
            i->Arith_Oper = Arith_Operator::shift_right; 
        else if(oper == "<<")
            i->Arith_Oper = Arith_Operator::shift_left; 
        else if(oper == "+")
            i->Arith_Oper = Arith_Operator::plus; 
        else if(oper == "-")
            i->Arith_Oper = Arith_Operator::minus; 
        else if(oper == "*")
            i->Arith_Oper = Arith_Operator::multiply; 
        else
            i->Arith_Oper = Arith_Operator::bw_and; 

        parsed_items.clear();
        parsed_strings.clear();
        //attach_pred_succ(i, currFunc);
        currFunc->instructions.push_back(i);

    }
  };

  template<> struct action < assign_comparison > {
    template < typename Input > static void apply (const Input &in, Program &p) {
	    auto currFunc = p.functions.back();
        Instruction* i = new Instruction();
        i->Type = InstructionType::assign_compare;
        for(auto& it : parsed_items) {
            i->Items.push_back(it);
        }
        
        std::string oper = parsed_strings.back();
        if(oper == ">")
            i->Comp_Oper = Compare_Operator::gr; 
        else if(oper == ">=")
            i->Comp_Oper = Compare_Operator::geq; 
        else if(oper == "<")
            i->Comp_Oper = Compare_Operator::le; 
        else if(oper == "<=")
            i->Comp_Oper = Compare_Operator::leq; 
        else
            i->Comp_Oper = Compare_Operator::eq; 

        parsed_items.clear();
        parsed_strings.clear();

        //attach_pred_succ(i, currFunc);
        currFunc->instructions.push_back(i);
    }
  };
  
  template<> struct action < assign_load > {
    template < typename Input > static void apply (const Input &in, Program &p) {
	    auto currFunc = p.functions.back();
        Instruction* i = new Instruction();
        i->Type = InstructionType::assign_load;
        for(auto& it : parsed_items) {
            i->Items.push_back(it);
        }
        parsed_items.clear();
        //attach_pred_succ(i, currFunc);
        currFunc->instructions.push_back(i);
    }
  };

  template<> struct action < assign_store > {
    template < typename Input > static void apply (const Input &in, Program &p) {
	    auto currFunc = p.functions.back();
        Instruction* i = new Instruction();
        i->Type = InstructionType::assign_store;
        for(auto& it : parsed_items) {
            i->Items.push_back(it);
        }
        parsed_items.clear();
        //attach_pred_succ(i, currFunc);
        currFunc->instructions.push_back(i);
    }
  };

  template<> struct action < br_unconditional > {
    template < typename Input > static void apply (const Input &in, Program &p) {
	    auto currFunc = p.functions.back();
        Instruction* i = new Instruction();
        i->Type = InstructionType::br_unconditional;
        for(auto& it : parsed_items) {
            i->Items.push_back(it);
        }
        parsed_items.clear();
        //attach_pred_succ(i, currFunc);
        currFunc->instructions.push_back(i);

    }
  };
  
  template<> struct action < br_conditional > {
    template < typename Input > static void apply (const Input &in, Program &p) {
	    auto currFunc = p.functions.back();
        Instruction* i = new Instruction();
        i->Type = InstructionType::br_conditional;
        for(auto& it : parsed_items) {
            i->Items.push_back(it);
        }
        parsed_items.clear();
        //attach_pred_succ(i, currFunc);
        currFunc->instructions.push_back(i);

    }
  };

  template<> struct action < call > {
    template < typename Input > static void apply (const Input &in, Program &p) {
	    auto currFunc = p.functions.back();
        Instruction* i = new Instruction();
        i->Type = InstructionType::call;
        // handle runtime function calls
        if(!parsed_strings.empty()) {
            std::string runtime = parsed_strings.back();

            if(runtime == "print")
                i->calleeType = CalleeType::print;
            else if(runtime == "allocate")
                i->calleeType = CalleeType::allocate;
            else // array-error
                i->calleeType = CalleeType::array_error;

            for(auto& it : parsed_items)
                i->arguments.push_back(it);
        }

        // handle vars and labels; know that parsed_items has exactly one thing
        else {
            auto it = parsed_items[0];
            i->Items.push_back(it);
            if(it->Type == Atomic_Type::var)
               i->calleeType = CalleeType::var;
            else if(it->Type == Atomic_Type::label)
               i->calleeType = CalleeType::label;

            // we know that the first Item in parsed_items is the function label
            // therefore we can skip it
            for(int j = 1; j < parsed_items.size(); ++j)
                i->arguments.push_back(parsed_items[j]);
        }

        parsed_items.clear();
        parsed_strings.clear();
        //attach_pred_succ(i, currFunc);
        currFunc->instructions.push_back(i);
    }
  };

  template<> struct action < call_assign > {
    template < typename Input > static void apply (const Input &in, Program &p) {
	    auto currFunc = p.functions.back();
        Instruction* i = new Instruction();
        i->Type = InstructionType::call_assign;

        //handle assignment to variable
        Item* destination = parsed_items[0];
        i->Items.push_back(destination);
        
        // handle runtime function calls
        if(!parsed_strings.empty()) {
            std::string runtime = parsed_strings.back();
            if(runtime == "print")
                i->calleeType = CalleeType::print;
            else if(runtime == "allocate")
                i->calleeType = CalleeType::allocate;
            else // array-error
                i->calleeType = CalleeType::array_error;

            for(int j = 1; j < parsed_items.size(); ++j)
                i->arguments.push_back(parsed_items[j]);
        }

        // handle vars and labels; know that this is second thing in parsed_items
        else {
            Item* it = parsed_items[1];
            i->Items.push_back(it);
            if(it->Type == Atomic_Type::var)
               i->calleeType = CalleeType::var;
            else if(it->Type == Atomic_Type::label)
               i->calleeType = CalleeType::label; 

            for(int j = 2; j < parsed_items.size(); ++j)
                i->arguments.push_back(parsed_items[j]);
        }

        parsed_items.clear();
        parsed_strings.clear();
        //attach_pred_succ(i, currFunc);
        currFunc->instructions.push_back(i);

    }
  };

  template<> struct action < runtime_func > {
    template < typename Input > static void apply (const Input &in, Program &p) {
    // empty type; Instruction_runtime holds the function type information
    parsed_strings.push_back(in.string());
    }
  };

  template<> struct action < label_instruction > {
    template < typename Input > static void apply (const Input &in, Program &p) {
	    Function* currFunc = p.functions.back();
        Instruction* i = new Instruction();
        i->Type = InstructionType::label;
        for(Item* it : parsed_items) {
            i->Items.push_back(it);
        }
        parsed_items.clear();
        //attach_pred_succ(i, currFunc);
        currFunc->instructions.push_back(i);
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
        Function* currFunc = p.functions.back();
        BB* currBlock = currFunc.block.back();
        Instruction* i = new Instruction();
        if (parsed_type == "int64") {
             int64_Var* var
             i->Items.push_back(it);
        } else if (parsed_type == "tuple") {          
        
        parsed_items.clear();
        currBlock->instructions.push_back(i);
    }
  };

   template<> struct action < Function_return_type > {
    template < typename Input >
    static void apply (const Input &in, Program &p) {
        Function* currF = p.functions.back();

        if(parsed_type == "int64") {
            currF->returnType = VarType::int64_type;
        }
        else if(parsed_type == "tuple") {
            currF->returnType = VarType::tuple_type;
        }
        else if(parsed_type == "code") {
            currF->returnType = VarType::code_type;
        }
        else if(parsed_type == "void") {
            currF->returnType = VarType::void_type;
        }
        else { // array type
            // TODO: store num_dim in function?
            currF->returnType = VarType::arr_type;
        }
        num_dim = 0;

        parsed_type = "";
    }
  };

//   template<> struct action < arg_type > {
//    template < typename Input >
//    static void apply (const Input &in, Program &p) {
//        if(parsed_type == "int64") {
//            parsed_Arg.varType = VarType::int64_type;
//        }
//        else if(parsed_type == "tuple") {
//            parsed_Arg.varType = VarType::tuple_type;
//        }
//        else if(parsed_type == "code") {
//            parsed_Arg.varType = VarType::code_type;
//        }
//        else {
//            parsed_Arg.varType = VarType::arr_type;
//            newItem->numDimensions = num_dim;
//        }
//
//        parsed_Arg = newItem;
//        parsed_type = "";
//    }
//  };

   template<> struct action < arg_var > {
    template < typename Input >
    static void apply (const Input &in, Program &p) {
        Function* currF = p.functions.back();
        var_Item* newItem;
        if(parsed_type == "int64") {
            newItem = new int64_Var();
            newItem->varType = VarType::int64_type;
        }
        else if(parsed_type == "tuple") {
            newItem = new tuple_Var();
            newItem->varType = VarType::tuple_type;
        }
        else if(parsed_type == "code") {
            newItem = new code_Var();
            newItem->varType = VarType::code_type;
        }
        else {
            newItem = new arr_Var();
            newItem->varType = VarType::arr_type;
            newItem->numDimensions = num_dim;
        }

        newItem->labelName = in.string();
        num_dim = 0;
        currF->arguments.push_back(newItem);
        parsed_Arg = {};
    }
  };

   template<> struct action < start_block > {
    template < typename Input >
    static void apply (const Input &in, Program &p) {
        block_buffer = new basic_block();
        label_Item* first_label = new label_Item();
        block_buffer->starting_label;


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
