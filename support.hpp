#ifndef SUPPORT_HPP_INCLUDED
#define SUPPORT_HPP_INCLUDED

#include <vector>
#include <string>

size_t countSubstring(const std::string& str, const std::string& sub);

std::vector<std::string> split(const std::string& s, const std::string& delim);

std::vector<std::string> split_string_by_token(const std::string& text, const std::string& token);
std::vector<std::string> split_string_by_tokens(const std::string& text, const std::vector<std::string>& tokens);

bool is_keys_intersect(std::string key1, std::string key2);

std::vector<std::string> get_keys_intersections(std::string key1, std::string key2);

std::vector<size_t> prefix_function(const std::string& s);
size_t KMP_count(const std::string& S, const std::string& pattern, size_t start=0);
size_t preKMP_count(const std::vector<size_t>& prefix, const std::string& S, const std::string& pattern, size_t start=0);

#endif // SUPPORT_HPP_INCLUDED
