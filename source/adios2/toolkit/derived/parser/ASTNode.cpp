#ifndef ADIOS2_DERIVED_PARSER_ASTNODE_CPP_
#define ADIOS2_DERIVED_PARSER_ASTNODE_CPP_

#include "ASTNode.h"

namespace adios2
{
namespace detail
{

/*****************************************/
// alias maps to pair of path and indices (indices may be empty string)
std::map<std::string, std::pair<std::string, std::string>> ASTNode::var_lookup;
int ASTNode::num_expr = 0;

ASTNode::ASTNode()
{
    // DEBUGGING
    ++num_expr;
    // std::cout << "\tNum expressions: " << num_expr << std::endl;
}

ASTNode::ASTNode(ExpressionOperator op) : operation(op)
{
    // DEBUGGING
    ++num_expr;
    // std::cout << "creating ASTNode object with op: " << get_op_name(operation) << ", operation: "
    // << operation<< std::endl; std::cout << "\tNum expressions: " << num_expr << std::endl;
}

ASTNode::ASTNode(ExpressionOperator op, const char *str) : operation(op)
{
    // std::cout << "creating ASTNode object with op: " << op << " translated to operation (string):
    // " << get_op_name(op) << ", operation: " << operation << " and string: " << str << std::endl;
    switch (operation)
    {
    case ExpressionOperator::OP_ALIAS:
        alias = str;
        break;
    case ExpressionOperator::OP_PATH:
        alias = str;
        break;
    case ExpressionOperator::OP_INDEX:
        indices = str;
        break;
    default:
        // TODO: Make some error
        // std::cout << "***That's a problem... ASTNode constructed with string should be alias
        // type, path type,  or index type\n";
        break;
    }
    // DEBUGGING
    ++num_expr;
    // std::cout << "\tNum expressions: " << num_expr << std::endl;
}

ASTNode::ASTNode(ExpressionOperator op, double val) : operation(op), value(val)
{
    // DEBUGGING
    ++num_expr;
    // std::cout << "creating ASTNode object with op: " << op << " translated to operation (string):
    // " << get_op_name(operation) << ", operation: " << operation << " and value: " << value <<
    // std::endl; std::cout << "\tNum expressions: " << num_expr << std::endl;
}

ASTNode::ASTNode(ExpressionOperator op, ASTNode *e) : operation(op)
{
    sub_expr.push_back(e);
    // DEBUGGING
    ++num_expr;
    // std::cout << "creating ASTNode object with op: " << op << " translated to operation (string):
    // " << get_op_name(operation) << ", operation: " << operation << " and subexpression with op: "
    // << op << " translated to operation (string): " << get_op_name(sub_expr[0]->operation) <<
    // std::endl; std::cout << "\tNum expressions: " << num_expr << std::endl;
}

// for index
ASTNode::ASTNode(ExpressionOperator op, ASTNode *e, const char *str) : operation(op), indices(str)
{
    sub_expr.push_back(e);
    // DEBUGGING
    ++num_expr;
    // std::cout << "creating ASTNode object with op: " << op << " translated to operation (string):
    // " << get_op_name(operation) << ", operation: " << operation << ", subexpression with op: " <<
    // op << " translated to operation (string): " << get_op_name(sub_expr[0]->operation) << ", and
    // indices " << indices << std::endl; std::cout << "\tNum expressions: " << num_expr <<
    // std::endl;
}

ASTNode::ASTNode(ExpressionOperator op, ASTNode *e1, ASTNode *e2) : operation(op)
{
    sub_expr.push_back(e1);
    sub_expr.push_back(e2);
    // DEBUGGING
    ++num_expr;
    // std::cout << "creating ASTNode object with op: " << op << " translated to operation (string):
    // " << get_op_name(operation) << ", operation: " << operation << " and subexpression1 with op:
    // "
    // << op << " translated to operation (string): " << get_op_name(sub_expr[0]->operation) << "
    // and subexpression2 with op: " << op << " translated to operation (string): " <<
    // get_op_name(sub_expr[1]->operation) << std::endl; std::cout << "\tNum expressions: " <<
    // num_expr << std::endl;
}

// Copy constructor
ASTNode::ASTNode(const ASTNode &e)
: operation(e.operation), alias(e.alias), value(e.value), sub_expr(e.sub_expr)
{
    // DEBUGGING
    ++num_expr;
    // std::cout << "ASTNode Copy Constructor for op: " << get_op_name(operation) << ", operation: "
    // << operation << std::endl; std::cout << "\tNum expressions: " << num_expr << std::endl;
}

ASTNode::~ASTNode()
{
    for (ASTNode *e : sub_expr)
    {
        delete e;
    }
    // DEBUGGING
    --num_expr;
    // std::cout << "Destructor called for expression with op: " << get_op_name(operation) << ",
    // operation: " << operation << std::endl; std::cout << "\tNum expressions: " << num_expr <<
    // std::endl;
}

std::pair<std::string, std::string> ASTNode::lookup_var(const std::string var_alias)
{
    return var_lookup[var_alias];
}

std::string ASTNode::lookup_var_path(const std::string var_alias)
{
    return var_lookup[var_alias].first;
}

std::string ASTNode::lookup_var_indices(const std::string var_alias)
{
    return var_lookup[var_alias].second;
}

void ASTNode::add_lookup_entry(const std::string alias, const std::string var_name,
                               const std::string indices)
{
    // std::cout << "Adding alias to lookup table:\n\talias: " << alias << "\n\tvar_name: " <<
    // var_name << "\n\tindices: " << indices <<  std::endl;
    var_lookup[alias] = std::make_pair(var_name, indices);
}

void ASTNode::add_subexpr(ASTNode *e) { sub_expr.push_back(e); }

void ASTNode::add_back_subexpr(ASTNode *e, size_t n)
{
    size_t index = sub_expr.size() - n;
    // std::cout << "ASTNode add_back_subexpr index: " << index << std::endl;
    // if (index > 0 && sub_expr[index] == nullptr)
    sub_expr[index] = e;
}

void ASTNode::extend_subexprs(size_t n)
{
    // std::cout << "ASTNode extending subexprs from size " << sub_expr.size() << " to " <<
    // (sub_expr.size() + n) << std::endl;
    sub_expr.resize(sub_expr.size() + n);
}

void ASTNode::printpretty(std::string indent)
{
    std::cout << indent << get_op_name(operation) << ":";
    if (operation == ExpressionOperator::OP_ALIAS)
    {
        std::cout << " (alias " << alias << " maps to Variable '";
        std::cout << lookup_var_path(alias) << "'";
        if (lookup_var_indices(alias) != "")
        {
            std::cout << " [" << lookup_var_indices(alias) << "]";
        }
        std::cout << ")";
    }
    else if (operation == ExpressionOperator::OP_PATH)
    {
        std::cout << " (" << alias << ")";
    }
    else if (operation == ExpressionOperator::OP_INDEX)
    {
        std::cout << " [" << indices << "]";
    }
    std::cout << std::endl;
    for (ASTNode *e : sub_expr)
    {
        if (e != nullptr)
            e->printpretty(indent + "    ");
        else
            std::cout << "sub_expr is nullptr" << std::endl;
    }
}

}
}
#endif
