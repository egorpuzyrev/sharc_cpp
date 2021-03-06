#ifndef STATISTICS_HPP_INCLUDED
#define STATISTICS_HPP_INCLUDED

#include <vector>
#include <map>
#include <unordered_map>
#include <unordered_set>

#include "global.hpp"

const char B = 8;

float get_weight1(std::string key, size_t n, size_t L, size_t bi, size_t b=B);
float get_weight2(std::string key, size_t n, size_t L, size_t bi, size_t b=B);
float get_weight3(std::string key, size_t n, size_t L, size_t bi, size_t b=B);
float get_weight4(std::string key, size_t n, size_t L, size_t bi, size_t b=B);
float get_weight5(std::string key, size_t n, size_t L, size_t bi, size_t b=B);
float get_weight6(std::string key, size_t n, size_t L, size_t bi, size_t b=B);
float get_weight7(std::string key, size_t n, size_t L, size_t bi, size_t b=B);
float get_weight8(std::string key, size_t n, size_t L, size_t bi, size_t b=B);

fCounts<std::string> get_weights(float (*wei_fun)(std::string key, size_t n, size_t L, size_t bi, size_t b), Counts<std::string> keys, size_t L, size_t bi, size_t b=B);
std::unordered_map<std::string, float> get_uweights(float (*wei_fun)(std::string key, size_t n, size_t L, size_t bi, size_t b), Counts<std::string> keys, size_t L, size_t bi, size_t b=B);
std::unordered_map<std::string, float> get_uweights(float (*wei_fun)(std::string key, size_t n, size_t L, size_t bi, size_t b), std::map<std::string, std::vector<size_t>> keys, size_t L, size_t bi, size_t b=B);
std::unordered_map<std::string, float> get_uweights(float (*wei_fun)(std::string key, size_t n, size_t L, size_t bi, size_t b), std::map<std::string, std::unordered_set<size_t>> keys, size_t L, size_t bi, size_t b=B);

#endif // STATISTICS_H_INCLUDED
