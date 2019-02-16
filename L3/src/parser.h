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
   * Data required to parse
   */ 
  std::vector<Item> parsed_items;

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
   * instructions
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

  struct assignment:
	pegtl::seq<
	  seps,
	  var,
	  seps,
	  assign_operator,
	  seps,
	  pegtl::sor<var, number, Label_rule>,
      seps
    > {};

  struct assign_with_oper:
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

 struct comparison_operator:
  pegtl::sor<
   pegtl::string<'<', '='>,
   pegtl::one<'<'>,
   pegtl::one<'='>,
   pegtl::one<'>'>,
   pegtl::string<'>','='>
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
    pegtl::seq<return_empty>, 
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

  struct argument : 
      pegtl::seq<var> {}; // to make sure var action is called

  struct Function_rule:
    pegtl::seq<
      pegtl::string<'d','e','f','i','n','e'>,
      seps,
      function_name,
      seps,
      pegtl::one<'('>,
      seps,
      pegtl::star<
        pegtl::seq<
            argument,
            seps
        >
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
