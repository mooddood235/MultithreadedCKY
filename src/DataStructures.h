#pragma once
#include <vector>
#include <string>

struct TableTree {
	TableTree(std::string label, std::vector<TableTree>);
	TableTree(std::string label);
	std::string ToString() const;
	std::string label;
	std::vector<TableTree> children;
};

