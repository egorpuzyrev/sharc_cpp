#include <iostream>
#include <vector>
#include <unordered_set>
#include <string>
#include <algorithm>
#include <chrono>

#include "keys.hpp"
#include "global.hpp"
#include "support.hpp"
#include "lcp.hpp"
#include "rabinkarp.hpp"

Counts<std::string> get_keys_naive(std::string text, size_t factor, size_t min_len, size_t max_len) {

    size_t pos = 0;
    size_t c = min_len;
    size_t l = text.length();
    Counts<std::string> keys;
    std::unordered_set<std::string> keys_set;

    std::string sub;
    size_t cnt;

    while(pos<l) {
        //std::cout<<"\t"<<pos<<" of "<<l;
        while(pos+c<=l && c<=max_len) {
            sub = text.substr(pos, c);
//            std::cout<<"sub="<<sub;
            if(!keys_set.count(sub)) {
//                cnt = std::count(text.begin(), text.end(), sub);
                keys_set.emplace(sub);
                cnt = countSubstring(text, sub);
                if(cnt>factor) {
                    keys[sub] = cnt;
                } else {
                    break;
                }
            }
            c += 1;
        }
        pos += 1;
        c = min_len;
    }
    return keys;
}


Counts<std::string> get_keys_by_lcp(std::string text, size_t factor, size_t min_len, size_t max_len, size_t block_size) {

    auto start = std::chrono::system_clock::now();
    auto finish = std::chrono::system_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(finish - start);

    size_t L = text.length(), l, cnt;
    //size_t step = (size_t)block_size-min_len;
    size_t step = (size_t)block_size/2-min_len;
    //size_t step = (size_t)1;
    std::vector<std::string> blocks;

    start = std::chrono::system_clock::now();
    for(size_t i=0; i<L; i+=step) {
        blocks.push_back(text.substr(i, block_size));
    }
    finish = std::chrono::system_clock::now();
    elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(finish - start);
    std::cout<<"Split by blocks: "<< elapsed.count() << std::endl;

    std::vector<std::pair<std::string, size_t>> suffixes, suffs;

    start = std::chrono::system_clock::now();
    for(size_t i=0; i<blocks.size(); i++) {
        suffs = get_suffixes(blocks[i]);
        for(auto& j: suffs) {
            suffixes.push_back(std::make_pair(j.first, block_size*i+j.second));
        }
    }
    finish = std::chrono::system_clock::now();
    elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(finish - start);
    std::cout<<"Getting suffixes: "<< elapsed.count() << std::endl;

    start = std::chrono::system_clock::now();
    std::sort(suffixes.begin(), suffixes.end(), [](auto a, auto b){return a.first>b.first;});
    finish = std::chrono::system_clock::now();
    elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(finish - start);
    std::cout<<"Sorting suffixes: "<< elapsed.count() << std::endl;

    start = std::chrono::system_clock::now();
    std::vector<size_t> lcps = get_lcp_naive(suffixes);
    finish = std::chrono::system_clock::now();
    elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(finish - start);
    std::cout<<"get_lcp: "<< elapsed.count() << std::endl;


    std::vector<std::string> common_prefixes;

    start = std::chrono::system_clock::now();
    for(size_t i=0; i<lcps.size(); i++) {
        if(lcps[i]>0) {
            common_prefixes.push_back(suffixes[i].first.substr(0, lcps[i]));
        }
    }
    finish = std::chrono::system_clock::now();
    elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(finish - start);
    std::cout<<"getting common prefixes: "<< elapsed.count() << std::endl;


    std::unordered_set<std::string> keys_set;
    Counts<std::string> keys;
    std::string sub;

    //std::vector<size_t> prekmp = prefix_function(text);

    start = std::chrono::system_clock::now();
    for(auto& prefix: common_prefixes) {
        l = prefix.length();
        size_t lim = std::min(l, max_len);
        //#pragma omp parallel for
        for(size_t i=min_len; i<=lim; i++) {
            sub = prefix.substr(0, i);

            //keys[sub] = 0;
            keys_set.emplace(sub);
//            if(!keys_set.count(sub)) {
//                keys_set.emplace(sub);
//                cnt = countSubstring(text, sub);
//                //std::cout<<"Sub: "<<sub<<"\t"<<cnt<<std::endl;
//                //cnt = KMP_count(text, sub);
//                //cnt = preKMP_count(prekmp, text, sub);
//                if(cnt>factor) {
//                    keys[sub] = cnt;
//
//                } else {
//                    break;
//                }
//            }
        }
    }
    finish = std::chrono::system_clock::now();
    elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(finish - start);
    std::cout<<"zeroing keys: "<< elapsed.count() << std::endl;

    start = std::chrono::system_clock::now();
//    for(auto& key: keys_set) {
//        sub = key;
////        if(key.second==0) {
//            cnt = countSubstring(text, sub);
////            if(cnt>factor) {
//                keys[sub] = cnt;
////            } else {
////                break;
////            }
////        }
//    }
    keys = multicount(text, keys_set.begin(), keys_set.end());
    finish = std::chrono::system_clock::now();
    elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(finish - start);
    std::cout<<"counting keys: "<< elapsed.count() << std::endl;

    std::cout<<"ypsies in the count: "<<keys["ypsies in the"]<<std::endl;

    std::cout<<"Size before filtering: "<<keys.size()<<std::endl;
    start = std::chrono::system_clock::now();
    //for(auto it=keys.begin(); it!=keys.end(); it++) {
    auto it=keys.begin();
    while(it!=keys.end()) {
        if((*it).second<=factor || (*it).first.length()<min_len || (*it).first.length()>max_len) {
            keys.erase(it++);
        } else {
            ++it;
        }
    }
    finish = std::chrono::system_clock::now();
    elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(finish - start);
    std::cout<<"Size after filtering: "<<keys.size()<<std::endl;
    std::cout<<"filtering keys: "<< elapsed.count() << std::endl;

    return keys;
}
