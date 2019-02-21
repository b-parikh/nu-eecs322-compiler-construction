#include <tree.h>

namespace L3 {

    Tree::Tree() {}
    Tree::~Tree() {}

    Tree::Tree(Instruction* ip) {
        this->equivalent_instruction = ip;
    }

    Tree_assign::Tree_assign(Instruction* ip) :
        Tree(ip) 
    {
        TreeNode dest;
        dest.equivalent_item = ip->Items[0];
        this->destination = dest;
        
        TreeNode src;
        src.equivalent_item = ip->Items[1];
        this->source = src;
    }
    
    Tree_assign_compare::Tree_assign_compare(Instruction* ip) :
        Tree(ip) 
    {
        TreeNode dest;
        dest.equivalent_item = ip->Items[0];
        this->destination = dest;
        
        TreeNode a1;
        a1.equivalent_item = ip->Items[1];
        this->arg1 = a1;
        
        TreeNode a2;
        a2.equivalent_item = ip->Items[2];
        this->arg2= a2;

        this->Comp_Oper = ip->Comp_Oper;
    }
    
    Tree_assign_arithmetic::Tree_assign_arithmetic(Instruction* ip) :
        Tree(ip) 
    {
        TreeNode dest;
        dest.equivalent_item = ip->Items[0];
        this->destination = dest;
        
        TreeNode a1;
        a1.equivalent_item = ip->Items[1];
        this->arg1 = a1;
        
        TreeNode a2;
        a2.equivalent_item = ip->Items[2];
        this->arg2= a2;

        this->Arith_Oper = ip->Arith_Oper;
    }

    Tree_assign_load::Tree_assign_load(Instruction* ip) :
        Tree(ip) 
    {
        TreeNode dest;
        dest.equivalent_item = ip->Items[0];
        this->destination = dest;

        TreeNode src;
        src.equivalent_item = ip->Items[1];
        this->source = src;
    }

    Tree_assign_store::Tree_assign_store(Instruction* ip) :
        Tree(ip)
    {
        TreeNode dest;
        dest.equivalent_item = ip->Items[0];
        this->destination = dest;

        TreeNode src;
        src.equivalent_item = ip->Items[1];
        this->source = src;
    }

    Tree_return_empty::Tree_return_empty(Instruction* ip) :
        Tree(ip)
    {}

    Tree_return_value::Tree_return_value(Instruction* ip) :
        Tree(ip)
    {
        TreeNode ret_val;
        ret_val.equivalent_item  = ip->Items[0];
        this->return_value = ret_val;
    }

    Tree_label::Tree_label(Instruction* ip) :
        Tree(ip)
    {
        TreeNode lab;
        lab.equivalent_item = ip->Items[0];
        this->label = lab;
    }

    Tree_br_conditional::Tree_br_conditional(Instruction* ip) :
        Tree(ip)
    {
        TreeNode src_var;
        src_var.equivalent_item = ip->Items[0];
        this->source = src_var;

        TreeNode lab;
        lab.equivalent_item = ip->Items[1];
        this->jump_to = lab;
    }
    
    Tree_br_unconditional::Tree_br_unconditional(Instruction* ip) :
        Tree(ip)
    {
        TreeNode lab;
        lab.equivalent_item = ip->Items[0];
        this->jump_to = lab;
    }

    Tree_call::Tree_call(Instruction* ip) :
        Tree(ip)
    {
        // insert arguments
        for(auto &arg : ip->arguments) {
            TreeNode node_arg;
            node_arg.equivalent_item = arg;
            this->arguments.push_back(node_arg);
        }

        // insert runtime, var, or label
        TreeNode to_call;
        if(ip->calleeType == CalleeType::print) {
            Item* callee = new Item();
            callee->labelName = "print";
            callee->Type = Atomic_Type::runtime; 
            to_call.equivalent_item= callee;
        }
        else if(ip->calleeType == CalleeType::allocate) {
            Item* callee = new Item();
            callee->labelName = "allocate";
            callee->Type = Atomic_Type::runtime; 
            to_call.equivalent_item= callee;
        }
        else if(ip->calleeType == CalleeType::array_error) {
            Item* callee = new Item();
            callee->labelName = "array_error";
            callee->Type = Atomic_Type::runtime; 
            to_call.equivalent_item= callee;
        }
        else{ // var or label
            to_call.equivalent_item = ip->Items[0];
        }
       
        this->item_to_call = to_call;
    }

    Tree_call_assign::Tree_call_assign(Instruction* ip) :
        Tree(ip)
    {
        TreeNode dest; 
        dest.equivalent_item = ip->Items[0];
        this->destination = dest;
        
        // insert arguments
        for(auto &arg : ip->arguments) {
            TreeNode arg_node;
            arg_node.equivalent_item = arg;
            this->arguments.push_back(arg_node);
        }

        // insert runtime, var, or label
        TreeNode to_call;
        if(ip->calleeType == CalleeType::print) {
            Item* callee = new Item();
            callee->labelName = "print";
            callee->Type = Atomic_Type::runtime; 
            to_call.equivalent_item = callee;
        }
        else if(ip->calleeType == CalleeType::allocate) {
            Item* callee = new Item();
            callee->labelName = "allocate";
            callee->Type = Atomic_Type::runtime; 
            to_call.equivalent_item = callee;
        }
        else if(ip->calleeType == CalleeType::array_error) {
            Item* callee = new Item();
            callee->labelName = "array_error";
            callee->Type = Atomic_Type::runtime; 
            to_call.equivalent_item = callee;
        }
        else{ // var or label
            to_call.equivalent_item = ip->Items[0];
        }
       
        this->item_to_call = to_call;
    }

    Tree* Instruction_to_Tree(Instruction* ip) {
        Tree* new_tree; 
        if(ip->Type == InstructionType::assign) {
            new_tree = new Tree_assign(ip);
        }
        else if(ip->Type == InstructionType::assign_compare) {
            new_tree = new Tree_assign_compare(ip);
        }
        else if(ip->Type == InstructionType::assign_arithmetic) {
            new_tree = new Tree_assign_arithmetic(ip);
        }
        else if(ip->Type == InstructionType::assign_load) {
            new_tree = new Tree_assign_arithmetic(ip);
        }
        else if(ip->Type == InstructionType::assign_store)
            new_tree = new Tree_assign_store(ip);
        else if(ip->Type == InstructionType::return_empty)
            new_tree = new Tree_return_empty(ip);
        else if(ip->Type == InstructionType::return_value)
            new_tree = new Tree_return_value(ip);
        else if(ip->Type == InstructionType::label)
            new_tree = new Tree_label(ip);
        else if(ip->Type == InstructionType::br_conditional)
            new_tree = new Tree_br_conditional(ip);
        else if(ip->Type == InstructionType::br_unconditional)
            new_tree = new Tree_br_unconditional(ip);
        else if(ip->Type == InstructionType::call)
            new_tree = new Tree_call(ip);
        else 
            new_tree = new Tree_call_assign(ip);
        
        return new_tree;
    }

}

