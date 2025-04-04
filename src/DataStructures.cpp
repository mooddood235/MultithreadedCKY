#include "DataStructures.h"
#include <format>

TableTree::TableTree(std::string label, std::vector<TableTree> children)
    : label(std::move(label)), children(std::move(children)) {}

TableTree::TableTree(std::string label) 
    : label(std::move(label)), children(){}

std::string TableTree::ToString() const{
    std::string childrenRes = "";

    for (const TableTree& child : children) {
        childrenRes += child.ToString();
    }
    return std::format("({}{})", label, childrenRes);
}
