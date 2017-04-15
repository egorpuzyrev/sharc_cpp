#ifndef RABINKARP_HPP_INCLUDED
#define RABINKARP_HPP_INCLUDED

#include <iostream>
#include <string>
#include <vector>
#include <iterator>
#include <map>
//#include <multimap>
#include <algorithm>
#include <iterator>
#include <unordered_set>
#include <set>

#include "global.hpp"
#include "support.hpp"

#include <easy/profiler.h>


Counts<std::string> multicount(std::string text, const std::unordered_set<std::string>& hashes_set);

template<class CustomIterator>
Counts<std::string> multicount(const std::string& text, CustomIterator first, CustomIterator last) {
    EASY_FUNCTION();
    size_t l=text.length();
    Counts<std::string> counts;
    std::unordered_set<std::string> hashes_set;
//    std::set<std::string> hashes_set;

//    std::unordered_set<size_t> patterns_lens_set;
    std::set<size_t> patterns_lens_set;

    CustomIterator it=first;
    //#pragma parallel
//    while(it!=last) {
    for(;it!=last; it++) {
//        hashes_set.emplace((*it));
        hashes_set.insert((*it));
//        patterns_lens_set.emplace((*it).length());
        patterns_lens_set.insert((*it).length());
        counts[(*it)] = 0;
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

    std::cout<<std::endl<<"rabinkarp keys:"<<std::endl;
    for(auto& i: counts) {
        std::cout<<"\t<"<<i.first<<">\t"<<i.second<<std::endl;
    }

    return counts;
}
#endif // RABINKARP_HPP_INCLUDED
