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

    int pos = 0;
    int c = min_len;
    int l = text.length();
    Counts<std::string> keys;
    std::unordered_set<std::string> keys_set;

    std::string sub;
    int cnt;

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
    size_t half_block = (size_t)block_size/2;
    std::vector<std::string> blocks;

    start = std::chrono::system_clock::now();
    for(size_t i=0; i<L; i+=half_block) {
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
    std::vector<size_t> lcps = get_lcp(suffixes);
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
        for(size_t i=min_len; i<=l&&i<=max_len; i++) {
            sub = prefix.substr(0, i);

            //keys[sub] = 0;
            keys_set.emplace(sub);
//            if(!keys_set.count(sub)) {
//                keys_set.emplace(sub);
//                //cnt = countSubstring(text, sub);
//                cnt = KMP_count(text, sub);
//                //cnt = preKMP_count(prekmp, text, sub);
//                if(cnt>factor) {
//                    keys[sub] = cnt;
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
    multicount(text, keys_set.begin(), keys_set.end());
    finish = std::chrono::system_clock::now();
    elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(finish - start);
    std::cout<<"counting keys: "<< elapsed.count() << std::endl;


    start = std::chrono::system_clock::now();
    for(auto it=keys.begin(); it!=keys.end(); it++) {
        if((*it).second<factor) {
            keys.erase(it);
        }
    }
    finish = std::chrono::system_clock::now();
    elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(finish - start);
    std::cout<<"filtering keys: "<< elapsed.count() << std::endl;

    return keys;
}
