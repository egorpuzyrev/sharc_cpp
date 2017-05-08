#ifndef LCP_HPP_INCLUDED
#define LCP_HPP_INCLUDED
#include <vector>

std::vector<std::pair<std::string, size_t>> get_suffixes(const std::string& s);
std::vector<std::pair<std::string, size_t>> get_prefixes(const std::string& s);

std::vector<std::string> get_suffixes_str(const std::string& s);
std::vector<std::string> get_prefixes_str(const std::string& s);

size_t get_common_prefix_naive(const std::string& s1, const std::string& s2);
std::vector<size_t> get_lcp_naive(const std::vector<std::pair<std::string, size_t>>& sorted_suffixes);
std::vector<size_t> get_lcp_naive(const std::vector<std::string>& sorted_suffixes);

std::vector<size_t> get_lcp(const std::vector<std::pair<std::string, size_t>>& sorted_suffixes);

std::vector<std::string> get_common_prefixes(std::string text);
#endif // LCP_HPP_INCLUDED
