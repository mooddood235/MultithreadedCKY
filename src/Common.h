#pragma once
#include <unordered_map>
#include <string>
#include <vector>
#include <algorithm>
#include "DataStructures.h"

std::unordered_map<std::string, std::string> rev_prods{
	{"S S", "S"},
	{"a", "S"},
	{"LEFT-S RIGHT", "S"},
	{"LEFT S", "LEFT-S"},
	{"(", "LEFT"},
	{")", "RIGHT"}
};

int indexFlat(int y, int x, int width) {
	return y * width + x;
};
void PrintTable(const std::vector<std::vector<TableTree>>& table, int n) {
    std::cout << "CYK Parse Table:\n";

    for (int row = 0; row < n; row++) {
        for (int col = 0; col < n; col++) {
            const std::vector<TableTree>& cell = table[indexFlat(row, col, n)];

            if (!cell.empty()) {
                std::cout << "[ ";
                for (const TableTree& t : cell) {
                    std::cout << t.ToString() << " ";
                }
                std::cout << "] ";
            }
            else {
                std::cout << "[ EMPTY ] ";
            }
        }
        std::cout << "\n";
    }
}
bool IsEnclosed(const std::string& expr) {
    if (expr.size() < 2 || expr[0] != '(' || expr[expr.size() - 1] != ')') {
        return false;
    }
    int stack = 0;

    for (int i = 0; i < expr.size(); i++) {
        if (expr[i] == '(') stack++;
        else if (expr[i] == ')') stack--;
        if (stack == 0 && i < expr.size() - 1) {
            return false;
        }
    }
    return stack == 0;
}

static std::vector<std::string> GenerateBracketingsHelper(const std::string& expr, bool all);


std::vector<std::string> GenerateBracketings(const std::string& expr, bool all) {
    std::vector<std::string> bracketings = GenerateBracketingsHelper(expr, all);
    
    std::vector<std::string> res = std::vector<std::string>();
    res.reserve(bracketings.size());
    
    for (std::string& b : bracketings) {
        if (IsEnclosed(b)) res.emplace_back(std::move(b));
    }
    std::sort(res.begin(), res.end());
    auto last = std::unique(res.begin(), res.end());
    res.erase(last, res.end());
    return std::move(res);
}
std::vector<std::string> GenerateFullBracketings(const std::string& expr) {
    if (expr.size() == 1)
        return { expr };

    std::vector<std::string> results;
    for (size_t i = 1; i < expr.size(); i++) {
        std::vector<std::string> leftParts = GenerateFullBracketings(expr.substr(0, i));
        std::vector<std::string> rightParts = GenerateFullBracketings(expr.substr(i));

        for (const auto& left : leftParts) {
            for (const auto& right : rightParts) {
                results.push_back("(" + left + right + ")");
            }
        }
    }
    return results;
}

static std::vector<std::vector<std::string>> tokenize(const std::string& s, bool all) {
    std::vector<std::vector<std::string>> tokens;
    std::size_t i = 0;
    while (i < s.size()) {
        if (s[i] == '(') {
            std::size_t start = i;
            int count = 1;
            ++i;
            while (i < s.size() && count > 0) {
                if (s[i] == '(') {
                    ++count;
                }
                else if (s[i] == ')') {
                    --count;
                }
                ++i;
            }
            std::string inner = s.substr(start + 1, i - start - 2);
            std::vector<std::string> possibilities = GenerateBracketingsHelper(inner, all);
            tokens.push_back(possibilities);
        }
        else if (s[i] == 'a') {
            tokens.push_back({ "a" });
            ++i;
        }
        else {
            ++i;
        }
    }
    return tokens;
}

static std::vector<std::string> combine(const std::vector<std::vector<std::string>>& tokenLists, bool all) {
    if (tokenLists.size() == 1) {
        return tokenLists[0];
    }
    std::vector<std::string> results;
    for (std::size_t split = 1; split < tokenLists.size(); ++split) {
        std::vector<std::vector<std::string>> leftTokens(tokenLists.begin(), tokenLists.begin() + split);
        std::vector<std::vector<std::string>> rightTokens(tokenLists.begin() + split, tokenLists.end());
        std::vector<std::string> leftExprs = combine(leftTokens, all);
        std::vector<std::string> rightExprs = combine(rightTokens, all);
        // For each pairing, form a new expression.
        for (const auto& l : leftExprs) {
            for (const auto& r : rightExprs) {
                results.push_back("(" + l + r + ")");
                if (all) {
                    results.push_back(l + r);
                }
            }
        }
    }
    return results;
}

static std::vector<std::string> GenerateBracketingsHelper(const std::string& expr, bool all) {
    std::vector<std::vector<std::string>> tokens = tokenize(expr, all);
    if (tokens.empty()) {
        return {};
    }
    else if (tokens.size() == 1) {
        return tokens[0];
    }
    else {
        return combine(tokens, all);
    }
}
std::vector<std::string> nAmbs(const std::string& expr, int n, int max) {
    std::vector<std::string> res = std::vector<std::string>();

    std::vector<std::string> interBs = GenerateBracketings(expr, true);
    for (const std::string& b : interBs) {
        std::vector<std::string> fullBs = GenerateBracketings(b, false);
        if (fullBs.size() == n) res.emplace_back(std::move(b));
        if (res.size() >= max) return std::move(res);
    }
    return std::move(res);
}
std::string oneAmb(int len) {
    if (len <= 1) return std::string(len, 'a');
    return std::format("(a{})", oneAmb(len - 1));
}

long long CatalanNumber(int n) {
    long long res = 1;
    for (int i = 0; i < n; i++) {
        res = res * (4 * i + 2) / (i + 2);
    }
    return res;
}
