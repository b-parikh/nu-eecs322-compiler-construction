#pragma once

#include <tao/pegtl.hpp>
#include <tao/pegtl/analyze.hpp>
#include <tao/pegtl/contrib/raw_string.hpp>
#include <iostream>
#include <IR.h>

namespace pegtl = tao::TAO_PEGTL_NAMESPACE;

using namespace pegtl;
using namespace std;

namespace IR{
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

  struct var:
    pegtl::seq<
      pegtl::one<'%'>,
      name
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
        pegtl::string<'t','u','p'.'l','e'>
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

  struct assign_operator:
	pegtl::string<'<','-'> {};

  struct arithmetic_operator:
	pegtl::sor<
           pegtl::one<'+'>,
	       pegtl::one<'-'>,
		   pegtl::one<'*'>,
		   pegtl::one<'&'>,
           pegtl::string<'<','<'>,
           pegtl::string<'>','>'>
	> {};
  
 struct comparison_operator:
  pegtl::sor<
   pegtl::string<'<', '='>,
   pegtl::one<'<'>,
   pegtl::one<'='>,
   pegtl::one<'>'>,
   pegtl::string<'>','='>
   > {};

  struct assign_arithmetic:
    pegtl::seq<
      seps,
      var,
      seps,
      assign_operator,
      seps,
      pegtl::sor<var, number>,
      seps,
      arithmetic_operator,
      seps,
      pegtl::sor<var, number>
    > {};

 struct assign_comparison:
    pegtl::seq<
    seps,
    var,
    seps,
    assign_operator,
    seps,
    pegtl::sor<var, number>,
    seps,
    comparison_operator,
    seps,
    pegtl::sor<var, number>,
    seps
  > {};

 struct assign_load:
     pegtl::seq<
        seps,
        var,
        seps,
        assign_operator,
        seps,
        pegtl::string<'l','o','a','d'>,
        seps,
        var
     > {};
 
 struct assign_store:
     pegtl::seq<
        seps,
        pegtl::string<'s','t','o','r','e'>,
        seps,
        var,
        seps,
        assign_operator,
        seps,
        pegtl::sor<
            Label_rule,
            var,
            number
        >,
        seps
     > {};

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

 struct label_instruction:
   pegtl::seq<
     Label_rule
   > {};

 struct br_unconditional:
   pegtl::seq<
	     seps,
	     pegtl::string<'b','r'>,
	     seps,
 	     Label_rule,
	     seps
  > {};

struct br_conditional:
   pegtl::seq<
     seps,
     pegtl::string<'b','r'>,
     seps,
     var,
     seps,
     pegtl::sor<var, number>,
     seps,
     Label_rule,
     seps,
     Label_rule,
     seps
   > {};

struct runtime_func:
   pegtl::sor<
     pegtl::string<'p', 'r', 'i', 'n', 't'>,
     pegtl::string<'a', 'l', 'l', 'o', 'c', 'a', 't', 'e'>,
     pegtl::string<'a', 'r', 'r', 'a', 'y', '-', 'e', 'r', 'r', 'o' ,'r'>
   > {};

 // type var structure
 struct init_var: 
     pegtl::seq<
       var_type,
       seps,
       var
     > {};

 struct arg_var: 
     var {};

 struct arg_type:
     var_type {};

 struct argument_call : 
     pegtl::sor<
        var,
        number
     > {};
 
 struct call:
   pegtl::seq<
    seps,
    pegtl::string<'c','a','l','l'>,
    seps,

    pegtl::sor<
        Label_rule,
         runtime_func,
        var 
    >,
    seps,
    pegtl::one<'('>,
    pegtl::star<
        pegtl::seq<
            seps,
            argument_call,
            seps,
            pegtl::opt<
                pegtl::one<','>
            >
        >
    >,
    seps,
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
      pegtl::string<'c','a','l','l'>,
      seps,
      pegtl::sor<
          Label_rule,
          runtime_func,
          var 
      >,
      seps,
      pegtl::one<'('>,
      pegtl::star<
        pegtl::seq<
          seps,
          argument_call,
          seps,
          pegtl::opt<
            pegtl::one<','>
          >
        >
      >,
      seps,
      pegtl::one<')'>,
      seps
   > {};

  struct Instruction_rule:
    pegtl::sor<
      pegtl::seq<pegtl::at<return_value>, return_value>, 
      pegtl::seq<pegtl::at<return_empty>, return_empty>, 
      pegtl::seq<pegtl::at<assign_arithmetic>, assign_arithmetic>,
      pegtl::seq<pegtl::at<assign_comparison>, assign_comparison>,
      pegtl::seq<pegtl::at<assign_load>, assign_load>,
      pegtl::seq<pegtl::at<assign_store>, assign_store>,
      pegtl::seq<pegtl::at<call_assign>, call_assign>,
      pegtl::seq<pegtl::at<call>, call>,
      pegtl::seq<pegtl::at<label_instruction>, label_instruction>,
      pegtl::seq<pegtl::at<br_conditional>, br_conditional>,
      pegtl::seq<pegtl::at<br_unconditional>, br_unconditional>,
      pegtl::seq<pegtl::at<assign>, assign>
    > { };

  struct Instructions_rule:
    pegtl::star<
      pegtl::seq<
        seps,
        Instruction_rule,
        seps
      >
    > {};


  struct comma:
      pegtl::one<','> {};
        
  /*
   * Basic block construction
   */

  struct start_block : label_rule {};
  struct end_block:
      pegtl::sor<
        pegtl::seq<pegtl::at<br_conditional>, br_conditional>,
        pegtl::seq<pegtl::at<br_unconditional>, br_unconditional>,
        pegtl::seq<pegtl::at<return_value>, return_value>,
        pegtl::seq<pegtl::at<return_empty>, return_empty>
      > {};
        
  struct basic_block:
      pegtl::seq<
        seps,
        start_block,
        seps,
        Instructions_rule,
        seps,
        end_block,
        seps
      > {}; 

  struct blocks:
      pegtl::star<
         pegtl::seq<
            seps,
            basic_block,
            seps
        >
      > {};

  struct Function_return_type:
      pegtl::sor<
        var_type,
        void_type
      >{};

  
  struct Function_rule:
    pegtl::seq<
      pegtl::string<'d','e','f','i','n','e'>,
      seps,
      Function_return_type,
      seps,
      function_name,
      seps,
      pegtl::one<'('>,
      pegtl::star<
          seps,
          arg_type,
          seps,
          arg_var,
          seps,
          pegtl::opt<
              pegtl::one<','>
          >,
          seps    
      >,
      pegtl::one<')'>,
      seps,
      pegtl::one< '{' >,
      seps,
      blocks,
      seps,
      pegtl::one< '}' >
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
