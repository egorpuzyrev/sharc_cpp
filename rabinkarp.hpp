#ifndef RABINKARP_HPP_INCLUDED
#define RABINKARP_HPP_INCLUDED

#include <string>
#include <vector>
#include <iterator>
#include <map>
#include <algorithm>
#include <iterator>
#include <unordered_set>

#include "rabinkarp.hpp"
#include "global.hpp"
#include "global.hpp"

const auto BASE=256;


//size_t fasthash(std::string s, size_t base=BASE);

size_t fasthash(std::string s, size_t base=BASE) {
    size_t c=1, res=0;
    for(auto& i: s) {
        res += i*c;
        c *= base;
    }

    return res;
}


//Counts<std::string> multicount(std::string text, std::vector<std::string> patterns);
template<class CustomIterator>
Counts<std::string> multicount(std::string text, CustomIterator first, CustomIterator last) {
    //std::vector<size_t> hashes;
    //hashes.reserve(patterns.size());
    Counts<std::string> counts;
    std::map<std::string, size_t> hashes;
    std::map<size_t, std::string> rhashes;
    size_t h;

    std::map<size_t, size_t> cur_hashes;

    //for(auto& i: patterns) {
    //for(auto& i=patterns_begin; i!=patterns_end; i++) {
    CustomIterator it=first;
    while(it!=last) {
        auto i = (*it);
        h = fasthash(i, BASE);
        hashes[i] = h;
        rhashes[h] = i;
        cur_hashes[i.length()] = 0;
        ++it;
    }

    std::vector<size_t> patterns_lens;
    for(auto& i: cur_hashes) {
        patterns_lens.push_back(i.first);
    }

    std::sort(patterns_lens.begin(), patterns_lens.end());

    std::string sub;
    for(size_t i=0; i<text.length()-patterns_lens[0]; i++) {
        for(auto& j: cur_hashes) {
            sub = text.substr(i, j.first);
            j.second = fasthash(sub, BASE);
            if(rhashes.count(j.second)) {
                if(sub==rhashes[j.second]) {
                    counts[sub] += 1;
                }
            }
        }
    }


    return counts;
}
#endif // RABINKARP_HPP_INCLUDED
