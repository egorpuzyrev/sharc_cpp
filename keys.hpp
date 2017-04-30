#ifndef KEYS_HPP_INCLUDED
#define KEYS_HPP_INCLUDED

#include "global.hpp"

Counts<std::string> get_keys_naive(const std::string& text, size_t factor, size_t min_len, size_t max_len);
Counts<std::string> get_keys_naive_mod(const std::string& text, size_t factor, size_t min_len, size_t max_len);
Counts<std::string> get_keys_by_lcp(const std::string& text, size_t factor, size_t min_len, size_t max_len, size_t block_size);

#endif // KEYS_HPP_INCLUDED
