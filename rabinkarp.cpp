#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <deque>
#include <algorithm>
#include <iterator>
#include <unordered_set>
#include <chrono>

#include "rabinkarp.hpp"
#include "global.hpp"


Counts<std::string> multicount(std::string text, const std::unordered_set<std::string>& hashes_set) {
//    EASY_FUNCTION();
    auto start = std::chrono::system_clock::now();

    size_t l=text.length();
    Counts<std::string> counts;
    std::map<std::string, int> prev_pos;

    std::set<size_t> patterns_lens_set;

//    std::cout<<"To find: ";
    for(auto it=hashes_set.begin(); it!=hashes_set.end(); it++) {
//        patterns_lens_set.emplace((*it).length());
        patterns_lens_set.insert((*it).length());
        counts[(*it)] = 0;
        prev_pos[(*it)] = -(*it).length()-1;
//        std::cout<<"\t<"<<(*it)<<">";
//        ++it;
    }

    const size_t min_len = *(patterns_lens_set.begin());


//    std::vector<size_t> patterns_lens;
//    for(auto& i: patterns_lens_set) {
//        patterns_lens.push_back(i);
//    }

//    std::sort(patterns_lens.begin(), patterns_lens.end());

    std::string sub;
    //sub.reserve(*(patterns_lens_set.rbegin()));
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

    for(auto& cur_len: patterns_lens_set) {
        size_t i=0;
        //sub = "";
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


////    std::deque<char> deq();
//    size_t prev_len, cur_len;
//    //auto it = text.begin();
//    for(size_t i=0; i<=l-min_len; i+=1) {
//        //size_t i=0;
//        //sub = "";
////        sub.clear();
//        prev_len = 0;
//        //sub = text.substr(i, min_len);
//        //for(auto& cur_len: patterns_lens_set) {
//        for(auto it=patterns_lens_set.begin(); it!=patterns_lens_set.end() && i<=l-*(it); it++) {
//            cur_len = *(it);
////        std::cout<<"Pos: "<<i<<std::endl;
//            sub = text.substr(i, cur_len);
//            //sub += text.substr(i+prev_len, cur_len-prev_len);
//            //sub.append(text.substr(i+prev_len, cur_len-prev_len));
//            //sub.append(it+prev_len, it+cur_len);
//            if(hashes_set.count(sub)) {
//                if(i-prev_pos[sub]>=cur_len) {
//                    prev_pos[sub] = i;
//                    counts[sub] += 1;
////                    std::cout<<"\t<"<<sub<<">\t"<<counts[sub]<<std::endl;
//                }
//            }
//
//            prev_len = cur_len;
//        }
//        //it += 1;
//    }


//    std::cout<<std::endl<<"rabinkarp keys:"<<std::endl;
//    for(auto& i: counts) {
//        std::cout<<"\t<"<<i.first<<">\t"<<i.second<<std::endl;
//    }

    auto finish = std::chrono::system_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(finish - start);

    std::cout<<">>>Rabinkarp multicount time: "<< elapsed.count() << std::endl;

    return counts;
}


