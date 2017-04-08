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

    size_t l=text.length();
    Counts<std::string> counts;
    std::unordered_set<std::string> hashes_set;

    std::unordered_set<size_t> patterns_lens_set;

    CustomIterator it=first;
    //#pragma parallel
//    while(it!=last) {
    for(;it!=last; it++) {
        auto i = (*it);
        hashes_set.emplace((*it));
        patterns_lens_set.emplace((*it).length());
        counts[i] = 0;
//        ++it;
    }

    std::vector<size_t> patterns_lens;
    for(auto& i: patterns_lens_set) {
        patterns_lens.push_back(i);
    }

    std::sort(patterns_lens.begin(), patterns_lens.end());

    std::string sub;
    //#pragma parallel for
    for(size_t i=0; i<l; i++) {
        for(const auto& j: patterns_lens) {
            sub = text.substr(i, j);
            if(hashes_set.count(sub)) {
                    counts[sub] += 1;
            }
        }
    }


    return counts;
}
#endif // RABINKARP_HPP_INCLUDED
