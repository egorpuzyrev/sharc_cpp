#ifndef RABINKARP_HPP_INCLUDED
#define RABINKARP_HPP_INCLUDED

#include <string>
#include <vector>
#include <iterator>
#include <map>
//#include <multimap>
#include <algorithm>
#include <iterator>
#include <unordered_set>

#include "global.hpp"
#include "support.hpp"


//Counts<std::string> multicount(std::string text, std::vector<std::string> patterns);
template<class CustomIterator>
Counts<std::string> multicount(std::string text, CustomIterator first, CustomIterator last) {
    //std::vector<size_t> hashes;
    //hashes.reserve(patterns.size());
    size_t l=text.length();
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
        counts[i] = 0;
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
    size_t cur_hash;
    for(size_t i=0; i<l; i++) {
        for(auto& j: patterns_lens) {
//        for(size_t j=1; j<=patterns_lens[patterns_lens.size()-1]; j++) {
            if(l-i>=j) {
                sub = text.substr(i, j);
//                if(j==sub.length()) {
                //cur_hash = fasthash(sub, BASE);
                //j.second = cur_hash;
                //if(rhashes.count(cur_hash)) {
                if(hashes.count(sub)) {
                    //if(sub==rhashes[cur_hash]) {
                        counts[sub] += 1;
                    //}
                }
            }
        }
    }


    return counts;
}
#endif // RABINKARP_HPP_INCLUDED
