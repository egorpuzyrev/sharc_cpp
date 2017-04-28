#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <iterator>
#include <unordered_set>

#include "rabinkarp.hpp"
#include "global.hpp"


Counts<std::string> multicount(std::string text, const std::unordered_set<std::string>& hashes_set) {
//    EASY_FUNCTION();

    size_t l=text.length();
    Counts<std::string> counts;
    std::map<std::string, int> prev_pos;

    std::unordered_set<size_t> patterns_lens_set;

//    std::cout<<"To find: ";
    for(auto it=hashes_set.begin();it!=hashes_set.end(); it++) {
//        patterns_lens_set.emplace((*it).length());
        patterns_lens_set.insert((*it).length());
        counts[(*it)] = 0;
        prev_pos[(*it)] = -(*it).length()-1;
//        std::cout<<"\t<"<<(*it)<<">";
//        ++it;
    }

    std::vector<size_t> patterns_lens;
    for(auto& i: patterns_lens_set) {
        patterns_lens.push_back(i);
    }

    std::sort(patterns_lens.begin(), patterns_lens.end());

    std::string sub;
//    for(size_t i=0; i<l; i++) {
//        std::cout<<"Pos: "<<i<<std::endl;
//        for(const auto& j: patterns_lens) {
//            sub = text.substr(i, j);
//            if(hashes_set.count(sub)) {
//                counts[sub] += 1;
//                std::cout<<"\t<"<<sub<<">\t"<<counts[sub]<<std::endl;
//            }
//        }
//    }

    for(size_t& cur_len: patterns_lens) {
        size_t i=0;
        for(size_t i=0; i<=l-cur_len; i+=1) {
//        std::cout<<"Pos: "<<i<<std::endl;
            sub = text.substr(i, cur_len);
            if(hashes_set.count(sub)) {
                if(i-prev_pos[sub]>=cur_len) {
                    prev_pos[sub] = i;
                    counts[sub] += 1;
//                    std::cout<<"\t<"<<sub<<">\t"<<counts[sub]<<std::endl;
                }
            }
        }
    }

//    std::cout<<std::endl<<"rabinkarp keys:"<<std::endl;
//    for(auto& i: counts) {
//        std::cout<<"\t<"<<i.first<<">\t"<<i.second<<std::endl;
//    }

    return counts;
}


