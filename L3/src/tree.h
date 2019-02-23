#pragma once


#include <L3.h>

namespace L3 {

    struct TreeNode {
        Item* equivalent_item;
    };
    
    /*
     * Each Tree constructor assumes that the Instruction passed to it has 
     * the correct members inside. This is unsafe but it make it safe, the 
     * Instruction structs have to become polymorphic.
     */
    struct Tree {
        Tree();
        Tree(Instruction* ip);
        virtual ~Tree(); // polymorphic magic initialized 
        std::vector<TreeNode> nodes;
        Instruction* equivalent_instruction;
        std::vector<Tree*> predecessors;
        std::vector<Tree*> successors;
    };
    
//    struct Tree_assign : Tree {
//        Tree_assign(Instruction* ip);
//        TreeNode source;
//        TreeNode destination;
//    };
//    
//    struct Tree_assign_compare : Tree {
//        Tree_assign_compare(Instruction* ip);
//        TreeNode arg1;
//        TreeNode arg2;
//        //TreeNode compare_op; // should this be a TreeNode or just a Compare_Operator?
//        Compare_Operator Comp_Oper = Compare_Operator::nop; 
//        TreeNode destination;
//    };
//    
//    struct Tree_assign_arithmetic : Tree {
//        Tree_assign_arithmetic(Instruction* ip);
//        TreeNode arg1;
//        TreeNode arg2;
//        //TreeNode arithmetic_op; // should this be a TreeNode or just a Compare_Operator? 
//        Arith_Operator Arith_Oper = Arith_Operator::nop;
//        TreeNode destination;
//    };
//    
//    struct Tree_assign_load : Tree {
//        Tree_assign_load(Instruction* ip);
//        TreeNode arg1;
//        TreeNode source;
//        TreeNode destination;
//    };
//    
//    struct Tree_assign_store : Tree {
//        Tree_assign_store(Instruction* ip);
//        TreeNode source;
//        TreeNode destination;
//    };
//    
//    struct Tree_return_empty : Tree {
//        Tree_return_empty(Instruction* ip);
//    };
//    
//    struct Tree_return_value : Tree {
//        Tree_return_value(Instruction* ip);
//        TreeNode return_value;
//    };
//    
//    struct Tree_label : Tree {
//        Tree_label(Instruction* ip);
//        TreeNode label;
//    };
//    
//    struct Tree_br_conditional : Tree {
//        Tree_br_conditional(Instruction* ip);
//        TreeNode source;
//        TreeNode jump_to;
//    };
//    
//    struct Tree_br_unconditional : Tree {
//        Tree_br_unconditional(Instruction* ip);
//        TreeNode jump_to;
//    };
//    
//    struct Tree_call : Tree {
//        Tree_call(Instruction* ip);
//        std::vector<TreeNode> arguments;
//        TreeNode item_to_call;
//    };
//    
//    struct Tree_call_assign : Tree {
//        Tree_call_assign(Instruction* ip);
//        std::vector<TreeNode> arguments;
//        TreeNode item_to_call;
//        TreeNode destination;
//    };

    Tree* Instruction_to_tree(Instruction* ip);
}
