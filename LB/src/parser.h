#pragma once

#include <tao/pegtl.hpp>
#include <tao/pegtl/analyze.hpp>
#include <tao/pegtl/contrib/raw_string.hpp>
#include <iostream>
#include <LB.h>

namespace pegtl = tao::TAO_PEGTL_NAMESPACE;

using namespace pegtl;
using namespace std;

namespace LB{

  /* 
   * Grammar rules from now on.
   */

  // NOT the same as the "name" in the slides
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

  // same function as "name", which is used in the slides
  struct var:
	name {};

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

  //struct function_name:
  //  Label_rule {};

  struct comment: 
    pegtl::disable< 
      TAOCPP_PEGTL_STRING( "//" ), 
      pegtl::until< pegtl::eolf > 
    > {};

  struct comma:
      pegtl::one<','> {};

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

  /*
   * Variable types
   */
  struct int64_type:
        pegtl::string<'i','n','t','6','4'>
  {};

  struct void_type:
        pegtl::string<'v','o','i','d'>
  {};

  struct tuple_type:
        pegtl::string<'t','u','p','l','e'>
  {};

  struct code_type:
        pegtl::string<'c','o','d','e'>
  {};
  
  struct array_num_dim:
      pegtl::string<'[',']'> {};
  
  struct array_type:
      pegtl::seq<
        int64_type,
        pegtl::plus<
          seps,
          array_num_dim
        > 
      > {};
          
  struct var_type:
      pegtl::sor<
        pegtl::seq<pegtl::at<array_type>, array_type>,
        pegtl::seq<pegtl::at<int64_type>, int64_type>,
        tuple_type,
        code_type
      >
  {};

  /* 
   * Instructions
   */
  struct assign_operator:
	pegtl::string<'<','-'> {};

  // ok
  struct assign:
	pegtl::seq<
	  seps,
	  var,
	  seps,
	  assign_operator,
	  seps,
	  pegtl::sor<var, number, Label_rule>,
      seps
    > {};

  // ok
  struct arithmetic_operator:
	pegtl::sor<
           pegtl::one<'+'>,
	       pegtl::one<'-'>,
		   pegtl::one<'*'>,
		   pegtl::one<'&'>,
           pegtl::string<'<','<'>,
           pegtl::string<'>','>'>
	> {};
  
 // ok
 struct comparison_operator:
  pegtl::sor<
   pegtl::string<'<', '='>,
   pegtl::string<'>','='>,
   pegtl::one<'<'>,
   pegtl::one<'>'>,
   pegtl::one<'='>
   > {};

  //TODO: Implement action maybe??
  struct operation:
    pegtl::seq<
      pegtl::sor<var, number>,
      seps,
      pegtl::sor<
        arithmetic_operator,
        comparison_operator
      >,
      seps,
      pegtl::sor<var, number>
    > {};

  //TODO: Implement action
  struct assign_operation:
    pegtl::seq<
      seps,
      var,
      seps,
      assign_operator,
      seps,
      operation,
      seps
//      pegtl::sor<
//        arithmetic_operator,
//        comparison_operator
//      >,
//      seps,
//      pegtl::sor<var, number>
    > {};

  struct label_instruction:
   pegtl::seq<
     Label_rule
   > {};

  struct return_value:
      pegtl::seq<
        seps,
        str_return,
        seps,
        pegtl::sor<
           var,
           number
        >,
        seps
      > {};

 struct return_empty:
      //pegtl::seq<
        str_return {};
      //>{};

 struct br_unconditional:
   pegtl::seq<
	     seps,
	     pegtl::string<'b','r'>,
	     seps,
 	     Label_rule,
	     seps
  > {};

  //TODO: Implement action
  struct if_instruction:
    pegtl::seq<
     seps,
     pegtl::string<'i','f'>,
     seps,
     pegtl::one<'('>,
     seps,
     operation,
     seps,
     pegtl::one<')'>,
     seps,
     Label_rule,
     seps,
     Label_rule,
     seps
   > {};
 
  //TODO: Implement action
 struct while_instruction:
    pegtl::seq<
     seps,
     pegtl::string<'w', 'h', 'i', 'l','e'>,
     seps,
     pegtl::one<'('>,
     seps,
     operation,
     seps,
     pegtl::one<')'>,
     seps,
     Label_rule,
     seps,
     Label_rule,
     seps
   > {};

 struct assign_load_array:
     pegtl::seq<
        seps,
        var,
        seps,
        assign_operator,
        seps,
		var,
		seps,
		pegtl::plus<
		  pegtl::seq<
			pegtl::one<'['>,
			seps,
			pegtl::sor<
			  var,
			  number
			>,
			seps,
			pegtl::one<']'>,
			seps
		  >
		>
     > {};

 struct assign_store_array:
     pegtl::seq<
        seps,
        var,
        seps,
		pegtl::plus<
		  pegtl::seq<
			pegtl::one<'['>,
			seps,
			pegtl::sor<
			  var,
			  number
			>,
			seps,
			pegtl::one<']'>,
			seps
		  >
		>,
        assign_operator,
        seps,
        pegtl::sor<
            Label_rule,
            var,
            number
        >,
        seps
     > {};

  struct assign_length:
	pegtl::seq<
        seps,
        var,
        seps,
        assign_operator,
        seps,
        pegtl::string<'l','e','n','g','t','h'>,
        seps,
        var,
        seps,
        pegtl::sor<
            var,
            number
        >,
        seps
    > {};

  struct args:
    pegtl::seq<
      pegtl::sor<
        var,
        number
      >,
      seps,
      pegtl::opt<
		pegtl::seq<
		  pegtl::one<','>,
		  seps
		>
      >
    > {};

  struct assign_new_array:
	pegtl::seq<
	  seps,
	  var,
      seps,
      assign_operator,
	  seps,
	  pegtl::string<'n', 'e', 'w'>,
	  seps,
	  pegtl::string<'A', 'r', 'r', 'a', 'y'>,
	  seps,
	  pegtl::one<'('>,
	  seps,
	  pegtl::plus<args>,
	  pegtl::one<')'>,
	  seps
	> {};

  struct assign_new_tuple:
	pegtl::seq<
	  seps,
	  var,
      seps,
      assign_operator,
	  seps,
	  pegtl::string<'n', 'e', 'w'>,
	  seps,
	  pegtl::string<'T', 'u', 'p', 'l', 'e'>,
	  seps,
	  pegtl::one<'('>,
	  seps,
      pegtl::sor<
        var,
        number
      >,
      seps,
	  pegtl::one<')'>,
	  seps

	> {};

  struct print:
    pegtl::seq<
	  seps,
      pegtl::string<'p', 'r', 'i', 'n', 't'>,
	  seps,
	  pegtl::one<'('>,
	  seps,
	  pegtl::sor<
        var,
        number
      >,
	  seps,
	  pegtl::one<')'>,
	  seps
    > {};

 // The instruction for initialziation of var
 //TODO: Implement action
 struct init_var: 
     pegtl::seq<
       var_type,
       seps,
       pegtl::plus<
         var,
         seps,
         comma,
         seps
       > 
     > {};


 struct call:
   pegtl::seq<
    seps,
    var,
    seps,
    pegtl::one<'('>,
	seps,
    pegtl::star<args>,
    pegtl::one<')'>,
    seps
   > {};

  struct call_assign:
   pegtl::seq<
      seps,
      var,
      seps,
      assign_operator,
      seps,
      var,
      seps,
      pegtl::one<'('>,
	  seps,
      pegtl::star<args>,
      pegtl::one<')'>,
      seps
   > {};

  //TODO: Implement action
  struct scope_begin:
      pegtl::one<'{'> {};

  //TODO: Implement action
  struct scope_end:
      pegtl::one<'}'> {};

  struct continue_instruction:
    pegtl::string<'c','o','n','t','i','n','u','e'> {};
    
  struct break_instruction:
    pegtl::string<'b','r','e','a','k'> {};
        
 struct Instruction_rule:
   pegtl::sor<
     pegtl::seq<pegtl::at<assign_operation>, assign_operation>,
     pegtl::seq<pegtl::at<assign_load_array>, assign_load_array>,
     pegtl::seq<pegtl::at<assign_store_array>, assign_store_array>,
     pegtl::seq<pegtl::at<assign_length>, assign_length>,
     pegtl::seq<pegtl::at<assign_new_array>, assign_new_array>,
     pegtl::seq<pegtl::at<assign_new_tuple>, assign_new_tuple>,
     pegtl::seq<pegtl::at<call_assign>, call_assign>,
     pegtl::seq<pegtl::at<if_instruction>, if_instruction>,
     pegtl::seq<pegtl::at<while_instruction>, while_instruction>,
     pegtl::seq<pegtl::at<continue_instruction>, continue_instruction>,
     pegtl::seq<pegtl::at<break_instruction>, break_instruction>,
     pegtl::seq<pegtl::at<while_instruction>, while_instruction>,
     pegtl::seq<pegtl::at<br_unconditional>, br_unconditional>,
     pegtl::seq<pegtl::at<label_instruction>, label_instruction>,
     pegtl::seq<pegtl::at<init_var>, init_var>,
     pegtl::seq<pegtl::at<return_value>, return_value>,
     pegtl::seq<pegtl::at<return_empty>, return_empty>,
     pegtl::seq<pegtl::at<scope_begin>, scope_begin>, // scope as an instruction
     pegtl::seq<pegtl::at<scope_end>, scope_end>,
     pegtl::seq<pegtl::at<print>, print>,
     pegtl::seq<pegtl::at<call>, call>,
     pegtl::seq<pegtl::at<assign>, assign>
   > {};

  struct Instructions_rule:
    pegtl::star<
      pegtl::seq<
        seps,
        Instruction_rule,
        seps
      >
    > {};

 struct arg_var: 
     var {};

 struct arg_type:
     var_type {};

  struct Function_declare:
	pegtl::seq<
      pegtl::sor<
        var_type,
        void_type
      >,
	  seps,
      var 
	> {};
  
  struct Function_rule:
    pegtl::seq<
      seps,
      Function_declare,
      seps,
      pegtl::one<'('>,
	  seps,
      pegtl::star<
          arg_type,
          seps,
          arg_var,
          seps,
          pegtl::opt<
			pegtl::seq<
              pegtl::one<','>,
			  seps
			>
          >
      >,
      pegtl::one<')'>,
      seps,
      //pegtl::one< '{' >,
      scope_begin;
      seps,
      Instructions_rule,
      seps,
      //pegtl::one< '}' >
      scope_end;
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
      Functions_rule,
      seps
    > {};

  struct grammar : 
    pegtl::must< 
      entry_point_rule
    > {};

  Program parse_file (char *fileName);
}
