#pragma once

#include <tao/pegtl.hpp>
#include <tao/pegtl/analyze.hpp>
#include <tao/pegtl/contrib/raw_string.hpp>

#include <L3.h>

namespace pegtl = tao::TAO_PEGTL_NAMESPACE;

using namespace pegtl;
using namespace std;

namespace L3{
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

  struct return_empty:
      pegtl::seq<
        str_return
      >{};

  struct var:
    pegtl::seq<
      pegtl::one<'%'>,
      name
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

  /* 
   * Instructions
   */
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
	>
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
    pegtl::sor<var, number>
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
        >
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
     Label_rule,
     seps
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
        seps,
        argument,
        seps,
        pegtl::opt<
            pegtl::one<','>
        >,
    seps    
    >,
    pegtl::one<')'>,
    seps
   > {};

  struct call_assign:
      seps,
      var,
      seps,
      assign_operator,
      seps,
      pegtl::string<'c','a','l','l'>,
      pegtl::sor<
          Label_rule,
          runtime_func,
          var 
      >,
      seps,
      pegtl::one<'('>,
      seps,
      pegtl::plus<
          argument,
          seps
      >,
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

  struct argument : var {};

  struct comma:
      pegtl::one<','> {};
        
  struct Function_rule:
    pegtl::seq<
      pegtl::string<'d','e','f','i','n','e'>,
      seps,
      function_name,
      seps,
      pegtl::one<'('>,
      seps,
      pegtl::star<
          seps,
          argument,
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
      Instructions_rule,
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
