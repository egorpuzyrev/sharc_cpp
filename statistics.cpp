#ifndef STATISTICS_CPP_INCLUDED
#define STATISTICS_CPP_INCLUDED

#include <cmath>
#include "global.hpp"
#include "statistics.hpp"

float get_weight1(std::string key, size_t n, size_t L, size_t bi, size_t b){
    auto l = key.length();
    return (float)(L*b)/(float)(b*(L-l*(n-1))+bi*(1+n));
}

float get_weight2(std::string key, size_t n, size_t L, size_t bi, size_t b){
    auto l = key.length();
    return (float)(n*l*b)/(float)(bi*(n+1)+l*b);
}

float get_weight3(std::string key, size_t n, size_t L, size_t bi, size_t b){
    auto l = key.length();
    return (float)(l)*sqrt(n);
}

float get_weight4(std::string key, size_t n, size_t L, size_t bi, size_t b){
    auto l = key.length();
    return (float)(l*n)/((L-l*n)*(n+1));
}

float get_weight5(std::string key, size_t n, size_t L, size_t bi, size_t b){
    auto l = key.length();
    return (float) (l)*log2(n);
}

float get_weight6(std::string key, size_t n, size_t L, size_t bi, size_t b){
    auto l = key.length();
    return (float) sqrt(l)*log2(n);
}

fCounts<std::string> get_weights(float (*wei_fun)(std::string key, size_t n, size_t L, size_t bi, size_t b), Counts<std::string> keys, size_t L, size_t bi, size_t b){

    fCounts<std::string> weights;
    for (auto& kv: keys){
        weights[kv.first] = (*wei_fun)(kv.first, kv.second, L, bi, b);
    };

    return weights;
}

#endif
