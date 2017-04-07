#ifndef STATISTICS_HPP_INCLUDED
#define STATISTICS_HPP_INCLUDED

#include <map>

#include "global.hpp"

const char B = 8;

float get_weight1(std::string key, size_t n, size_t L, size_t bi, size_t b=B);
float get_weight2(std::string key, size_t n, size_t L, size_t bi, size_t b=B);
float get_weight3(std::string key, size_t n, size_t L, size_t bi, size_t b=B);
float get_weight4(std::string key, size_t n, size_t L, size_t bi, size_t b=B);
float get_weight5(std::string key, size_t n, size_t L, size_t bi, size_t b=B);

fCounts<std::string> get_weights(float (*wei_fun)(std::string key, size_t n, size_t L, size_t bi, size_t b), Counts<std::string> keys, size_t L, size_t bi, size_t b=B);

#endif // STATISTICS_H_INCLUDED
