#ifndef LCP_HPP_INCLUDED
#define LCP_HPP_INCLUDED
#include <vector>

std::vector<std::pair<std::string, size_t>> get_suffixes(const std::string &s);
std::vector<std::pair<std::string, size_t>> get_prefixes(const std::string &s);
std::vector<size_t> get_lcp(const std::vector<std::pair<std::string, size_t>>& sorted_suffixes);
std::vector<std::string> get_common_prefixes(std::string text);
#endif // LCP_HPP_INCLUDED
