#include <string>
#include <map>
#include <unordered_set>
#include <unordered_map>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <cmath>
#include <numeric>

#include <boost/algorithm/algorithm.hpp>
#include <boost/algorithm/string.hpp>

#include "global.hpp"
#include "statistics.hpp"
#include "keys.hpp"
#include "huffman.hpp"
#include "markov.hpp"
#include "support.hpp"
#include "compressor.hpp"
#include "rabinkarp.hpp"
#include "string_compressors.hpp"
#include "base64.hpp"

//#include <easy/profiler.h>

const size_t ALPHA_LIMIT=2;

std::string compress_block(std::string text, size_t block_size, size_t markov_order) {
//    EASY_FUNCTION();
    auto wei_fun = get_weight3;
    std::string res;
    CompressedBlock compressed_block;

    float max_weight, second_max_weight;
    std::string s, key, second_key, key1, key2;

    size_t n, N, bi=2, L=text.length();
    size_t L1=L;
    std::vector<std::string> keys_stack;
//    Counts<std::string> keys_intersections;
//    std::unordered_map<std::string, size_t> keys_intersections;
//    fCounts<std::string> weights;
    std::unordered_map<std::string, float> weights, new_weights;

    std::unordered_set<std::string> recalculate;

    /// calculation of keys_stack
    std::cout<<">>>get_keys_naive()"<<std::endl;
//    Counts<std::string> keys = get_keys_naive(text, 2, 1, 1);
//    EASY_BLOCK("Get keys block");
//    Counts<std::string> keys_copy = get_keys_by_lcp(text, 1, 2, 2, 1);
    Counts<std::string> keys_copy = get_keys_by_lcp(text, 1, 2, block_size, block_size);
//    Counts<std::string> keys_copy = get_keys_naive_mod(text, 1, 2, block_size);
//    Counts<std::string> keys_copy = get_keys_naive(text, 1, 2, block_size);
    std::unordered_map<std::string, size_t> keys_intersections(keys_copy.begin(), keys_copy.end());
    Counts<std::string> keys;
    //Counts<std::string> keys = get_keys_by_lcp(text, 1, 2, 2*block_size, block_size);
    float limweight = get_weight2("--", 2, L1, bi, B);
    //weights = get_weights(get_weight3, keys, L1, bi, B);
    for(const auto& i: keys_copy) {
//        if(i.second>1 && get_weight2(i.first, i.second, L1, bi, B)>limweight) {
        if(i.first.size()>ALPHA_LIMIT && i.second>=2 && get_weight2(i.first, i.second, L1, bi, B)>limweight) {
            keys[i.first] = i.second;
        }
    }
    keys_copy.clear();
//    EASY_END_BLOCK

//    std::ofstream out;
//    out.open("keys.csv");
//    for(auto& i: keys) {
//        out<<keys.first;
//        out<<"\t";
//        out<<keys.second<<std::endl;
//    }
//    out.close();

//    std::unordered_map<std::string, size_t> keys_copy(keys); //keys.begin(), keys.end());
//    Counts<std::string> keys_copy(keys.begin(), keys.end());
    Counts<std::string> last_keys_counts;

//    EASY_BLOCK("Calculation of keys_stack block");
    std::cout<<">>>calculation of keys_stack"<<std::endl;
    bool flag, independent_key_found;
    std::vector<std::string> new_intersections, recent_keys;
    std::unordered_set<std::string> to_count;
    std::map<std::string, std::unordered_set<std::string>> cur_intersections;

    Counts<std::string> to_substract, last_checked_pos;
//    std::map<std::string, std::map<std::string, std::vector<std::string>>> to_check;
    std::unordered_map<std::string, std::unordered_map<std::string, std::vector<std::string>>> to_check;

//    std::vector<std::string> sorted_by_weights;
    std::vector<std::pair<std::string, float>> sorted_by_weights;
//    weights = get_uweights(get_weight3, keys, L1, bi, B);
    weights = get_uweights(wei_fun, keys, L1, bi, B);
    sorted_by_weights.reserve(weights.size());
    sorted_by_weights.insert(sorted_by_weights.end(), weights.begin(), weights.end());
    std::sort(sorted_by_weights.begin(), sorted_by_weights.end(), [](const auto& a, const auto& b){return a.second<b.second;});
    weights.clear();
    std::cout<<"sorted_by_weights.size(): "<<sorted_by_weights.size()<<std::endl;
    std::cout<<"keys.size(): "<<keys.size()<<std::endl;
//    std::cin>>n;
    while(keys.size()) {
//        std::cout<<"\nsorted_by_weights.size() before: "<<sorted_by_weights.size()<<std::endl;
//        std::cout<<"keys.size() before: "<<keys.size()<<std::endl;
//        std::cout<<"keys left: "<<keys.size()<<std::endl;
//        weights = get_weights(get_weight3, keys, L1, bi, B);

        max_weight = -1.0;
//        second_max_weight = -1.0;
//        recent_keys.clear();
        flag = false;
        independent_key_found = false;

//        EASY_BLOCK("Choosing key with max wei");
//        std::cout<<"\n\nChoosing key with max weight..."<<std::endl;
///--------------------------------------------------------------------------------
//        for(auto& i: weights) {
//            if(i.second>max_weight) {
//                max_weight = i.second;
//                key2 = i.first;
//            }
//        }
//        std::sort(sorted_by_weights.begin(), sorted_by_weights.end(), [](const auto& a, const auto& b){return a.second<b.second;});
        key2 = sorted_by_weights.back().first;
        sorted_by_weights.pop_back();
//        std::cout<<">>>pop: "<<std::endl;
//        std::cout<<"\t<"<<key2<<">"<<std::endl;
///--------------------------------------------------------------------------------
//        EASY_END_BLOCK;

        //weights[key2] = -1;

//        std::cout<<"Checking next key: <"<<key2<<"> with size: "<<key2.size()<<std::endl;
//        std::cout<<"Checking for intersections..."<<std::endl;
        for(auto it=keys_stack.begin()+last_checked_pos[key2]; it!=keys_stack.end(); it++) {
            auto key = (*it);
            last_checked_pos[key2] += 1;
            new_intersections = get_keys_intersections_wh(key, key2);
//            new_intersections = get_keys_intersections(key, key2);
//            std::cout<<"new_intersections.size(): "<<new_intersections.size()<<std::endl;

            if(!new_intersections.empty()) {
//                std::cout<<new_intersections[0]<<std::endl;
                flag = true;
//                weights.erase(key2);
                to_check[key2][key] = std::vector<std::string>(new_intersections.begin(), new_intersections.end());
                for(auto& j: new_intersections) {
                    if(!keys_intersections.count(j)) {
                        to_count.insert(j);
                    }
                }
            }
        }


        if(flag) {
//            std::cout<<"Intersections found"<<std::endl;
//            sorted_by_weights.clear();
//            sorted_by_weights.reserve(weights.size());
//            std::cout<<"Sorting keys by max weight"<<std::endl;
//            for(const auto& i: weights) {
//                sorted_by_weights.push_back(i.first);
//            }
//            std::sort(sorted_by_weights.begin(), sorted_by_weights.end(), [&weights](const auto& a, const auto& b){return weights[a]<weights[b];});

//            sorted_by_weights.insert(sorted_by_weights.end(), weights.begin(), weights.end());
//            std::sort(sorted_by_weights.begin(), sorted_by_weights.end(), [](const auto& a, const auto& b){return a.second<b.second;});
//            sorted_by_weights.pop_back();
//            std::cout<<"Looking for independent key..."<<std::endl;
//            std::cout<<">>>pop: "<<std::endl;
            independent_key_found = false;
            while(!independent_key_found && !sorted_by_weights.empty()) {
//                std::cout<<"0 ";
                key2 = sorted_by_weights.back().first;
                sorted_by_weights.pop_back();
//                std::cout<<"\t<"<<key2<<">"<<std::endl;
//                std::cout<<"1 ";
//                weights.erase(key2);
                independent_key_found = true;
                for(auto it=keys_stack.begin()+last_checked_pos[key2]; it!=keys_stack.end(); it++) {
                    auto key = (*it);
                    last_checked_pos[key2] += 1;
                    new_intersections = get_keys_intersections_wh(key, key2);
//                    new_intersections = get_keys_intersections(key, key2);
//                    std::cout<<"2 ";
                    if(!new_intersections.empty()) {
                        independent_key_found = false;
//                        std::cout<<"3 ";
                        to_check[key2][key] = std::vector<std::string>(new_intersections.begin(), new_intersections.end());
//                        std::cout<<"4 ";
                        for(auto& j: new_intersections) {
                            if(!keys_intersections.count(j)) {
                                to_count.insert(j);
                            }
                        }
                    }
//                    else {
//                        independent_key_found = true;
//                    }
                }
            }

            if(independent_key_found) {
//                weights[key2] = get_weight3(key2, keys[key2], L1, bi, B);
                weights[key2] = wei_fun(key2, keys[key2], L1, bi, B);
            }
//            std::cout<<"Independent key found: <"<<key2<<">"<<std::endl;

///--------------------------------------------------------------------------------

////            std::cout<<"Looking for subs need to be counted..."<<std::endl;
//            for(auto& it: to_check) {
////                std::cout<<"6 ";
//                for(auto& it1: it.second) {
//                    new_intersections = it1.second;
////                    std::cout<<"7 ";
//                    for(auto& j: new_intersections) {
////                        std::cout<<"8 ";
//                        if(!keys_intersections.count(j)) {
////                            if(keys_copy.count(j)) {
////                                keys_intersections[j] = keys_copy[j];
////                            } else {
////                                std::cout<<"9 ";
////                                to_count.insert(j);
////                                if(text.find(j) != std::string::npos) {
////                                    if(j.size()>block_size) {
//                                        to_count.insert(j);
////                                    } else {
////                                        keys_intersections[j] = 1;
////                                    }
////                                } else {
////                                    keys_intersections[j] = 0;
////                                }
////                            }
//                        }
//                    }
//                }
//            }

///--------------------------------------------------------------------------------
//            std::cout<<"Need to count: "<<to_count.size()<<std::endl;
//            EASY_BLOCK("Counting intersections...");
//            std::cout<<"Counting intersections (multicount)"<<std::endl;
//            int more_than_1=0,more_than_0=0,maximal_len=0;
            to_substract = multicount(text, to_count);
//            for(auto& i: to_substract) {
//                if(i.second>1) {
//                    more_than_1 += 1;
//                }
//                if(i.second>0) {
//                    more_than_0 += 1;
//                }
//                if(i.first.size()>maximal_len) {
//                    maximal_len=i.first.size();
//                }
//            }
//            std::cout<<"\tmore_than_1: "<<more_than_1<<std::endl;
//            std::cout<<"\tmore_than_0: "<<more_than_0<<std::endl;
//            std::cout<<"\tmaximal_len: "<<maximal_len<<std::endl;
            keys_intersections.insert(to_substract.begin(), to_substract.end());
//            EASY_END_BLOCK;

            to_count.clear();
            to_substract.clear();

//            std::cout<<"Substracting..."<<std::endl;
            limweight = get_weight2("--", 2, L1, bi, B);
            for(auto& it: to_check) {
                key2 = it.first;
                n = 0;
                for(auto& it1: it.second) {
                    key = it1.first;
                    N = last_keys_counts[key];

                    for(auto& j: it1.second) {
                        n += keys_intersections[j];
                        if(keys.count(j)) {
                            if(keys[j]-N<2 || keys[j]-N>keys[j]) {
                                keys[j] = 0;
                                weights[j] = 0;
//                                weights.erase(j);
                            } else {
                                keys[j] -= N;
//                                weights[j] = get_weight3(j, keys[j], L1, bi, B);
                                weights[j] = wei_fun(j, keys[j], L1, bi, B);
                                if(get_weight2(j, keys[j], L1, bi, B)<=limweight) {
                                    keys[j] = 0;
                                    weights[j] = 0;
//                                    weights.erase(j);
                                }
//                                else {
//                                    weights[j] = get_weight3(j, keys[j], L1, bi, B);
////                                    sorted_by_weights.push_back(std::make_pair(j, get_weight3(j, keys[j], L1, bi, B)));
//                                }
                            }
                        }
                    }
                }

//                if(keys[key2]-n<2 || keys[key2]-n>keys[key2]) {
                if(keys[key2]-n<2 || keys[key2]-n>keys[key2]) {
                    keys[key2] = 0;
                    weights[key2] = 0;
//                    weights.erase(key2);
                } else {
                    keys[key2] -= n;
//                    weights[key2] = get_weight3(key2, keys[key2], L1, bi, B);
                    weights[key2] = wei_fun(key2, keys[key2], L1, bi, B);
                    if(get_weight2(key2, keys[key2], L1, bi, B)<=limweight) {
                        keys[key2] = 0;
                        weights[key2] = 0;
//                        weights.erase(key2);
                    }
//                    else {
//                        weights[key2] = get_weight3(key2, keys[key2], L1, bi, B);
////                        sorted_by_weights.push_back(std::make_pair(key2, get_weight3(key2, keys[key2], L1, bi, B)));
//                    }
                }
            }

            //auto it = keys.begin();
//            weights = get_uweights(get_weight2, keys, L1, bi, B);
//            limweight = get_weight2("--", 2, L1, bi, B);
            for(auto it = keys.begin(); it != keys.end(); ) {
                if (it->second < 2) {
//                if (it->second < 2 || weights[it->first]<=limweight) {
//                if (it->second < 2 || get_weight2(it->first, keys[it->first], L1, bi, B)<=limweight) {
                    weights[it->first] = 0;
                    it = keys.erase(it);
                } else {
                    ++it;
                }
            }


//            std::unordered_map<std::string, float> tmp_map(weights.begin(), weights.end());
            for(auto it=sorted_by_weights.begin(); it!=sorted_by_weights.end(); it++) {
                if(weights.count(it->first)) {
//                    it->second = tmp_map[it->first];
//                    tmp_map.erase(it->first);
                    it->second = weights[it->first];
                    weights[it->first] = 0;
//                    weights.erase(it->first);
                }
            }

//            for(auto it = weights.begin(); it != weights.end(); ) {
//                if (it->second == 0.0) {
//                    it = weights.erase(it);
//                } else {
//                    ++it;
//                }
//            }


//            sorted_by_weights.insert(sorted_by_weights.end(), tmp_map.begin(), tmp_map.end());
            sorted_by_weights.reserve(sorted_by_weights.size()+weights.size());
            sorted_by_weights.insert(sorted_by_weights.end(), weights.begin(), weights.end());

            sorted_by_weights.erase(
                   std::remove_if(sorted_by_weights.begin(),
                                  sorted_by_weights.end(),
                                  [](auto& x){return x.second==0.0;}),
                   sorted_by_weights.end());

            std::sort(sorted_by_weights.begin(), sorted_by_weights.end(), [](const auto& a, const auto& b){return a.second<b.second;});

//            std::unordered_map<std::string, float> tmp_map(sorted_by_weights.begin(), sorted_by_weights.end());
//            for(auto& i: keys) {
//                if(!tmp_map.count(i.first)) {
//                    std::cout<<"\tmissed <"<<i.first<<"> "<<i.second<<std::endl;
//                }
//            }

//            std::cout<<"sorted_by_weights.size() after: "<<sorted_by_weights.size()<<std::endl;
//            std::cout<<"keys.size() after: "<<keys.size()<<std::endl;

            to_check.clear();
            weights.clear();

        } else {
//            std::cout<<"Intersections not found"<<std::endl;
            std::cout<<"new key = <"<<key2<<"> "<<keys[key2]<<std::endl;
            std::cout<<"keys left: "<<keys.size()<<std::endl;
            keys_stack.push_back(key2);
            bi += 1;
            L1 -= key.size()*keys[key2]+ceil(bi*keys[key2]/B);

            last_keys_counts[key2] = keys[key2];
            keys.erase(key2);
//            weights.erase(key2);
//                recent_keys.push_back(key);
//            max_weight = 0;
        }
    }


//
////            std::cout<<"key2: <"<<key2<<">\t";
//            //for(auto& key: keys_stack) {
//            EASY_BLOCK("Check intersections");
//            for(auto it=keys_stack.begin()+last_checked_pos[key2]; it!=keys_stack.end(); it++) {
//                auto key = (*it);
////                last_checked_pos[key2] += 1;
//                new_intersections = get_keys_intersections(key, key2);
////                if(is_keys_intersect(key, key2)) {
//                if(!new_intersections.empty()) {
//                    flag = true;
////                    N = last_keys_counts[key];
////                    new_intersections = get_keys_intersections(key, key2);
//                    cur_intersections[key] = std::unordered_set<std::string>(new_intersections.begin(), new_intersections.end());
//                    for(auto& j: new_intersections) {
//                        if(!keys_intersections.count(j)) {
//                            if(keys_copy.count(j)) {
//                                keys_intersections[j] = keys_copy[j];
//                            } else {
//                                if(text.find(j) != std::string::npos) {
//                                    if(j.size()>block_size) {
//                                        to_count.insert(j);
//                                    } else {
//                                        keys_intersections[j] = 1;
//                                    }
//                                } else {
//                                    keys_intersections[j] = 0;
//                                }
//                            }
//                        }
//                    }
//                }
//            }
//
////                    new_intersections.clear();
//            if(flag) {
//            EASY_BLOCK("Counting intersections...");
//            to_substract = multicount(text, to_count);
//            keys_intersections.insert(to_substract.begin(), to_substract.end());
//            EASY_END_BLOCK;
//
//            to_count.clear();
//            to_substract.clear();
//
//
//            for(auto it=keys_stack.begin()+last_checked_pos[key2]; it!=keys_stack.end(); it++) {
//                auto key = (*it);
//                N = last_keys_counts[key];
//                last_checked_pos[key2] += 1;
//
//                n=0;
//                for(auto& j: cur_intersections[key]) {
////                for(auto& j: new_intersections) {
//                    n += keys_intersections[j];
//
//                    if(keys.count(j)) {
//                        if(keys[j]-N<2 || keys[j]-N>keys[j]) {
////                                keys[j] = 0;
//                            keys.erase(j);
//                        } else {
//                            keys[j] -= N;
//                        }
//                    }
//                }
//
//
//                if(keys[key2]-n<2 || keys[key2]-n>keys[key2]) {
//                    //keys[key2] = 0;
//                    keys.erase(key2);
//                } else {
//                    keys[key2] -= n;
//                }
//                    //break;
//            }
//            cur_intersections.clear();
//
//            EASY_END_BLOCK;
//
//
////            if(!flag) {
////                break;
//            } else {
//                std::cout<<"new key = <"<<key2<<"> "<<keys[key2]<<std::endl;
//                std::cout<<"keys left: "<<keys.size()<<std::endl;
//                keys_stack.push_back(key2);
//                bi += 1;
//                L1 -= key.size()*keys[key2]+ceil(bi*keys[key2]/B);
//
////                N = keys[key];
//                last_keys_counts[key] = keys[key];
//                keys.erase(key2);
//                weights.erase(key2);
////                recent_keys.push_back(key);
//                max_weight = 0;
//            }
//        }
//        std::cin>>key1;
//        std::cout<<"keys before filter: "<<keys.size()<<std::endl;
//        for(const auto& key: recent_keys) {
//            N = keys[key];
////            std::cout<<">>>>>>>>intersections: "<<key<<"\t"<<keys.count(key)<<std::endl;
//
//            n = 0;
//
//    //        new_intersections.clear();
//    //        cur_intersections.clear();
//
//            for(auto& i: keys) {
//
//                key2 = i.first;
//
//                new_intersections = get_keys_intersections(key, key2);
//                n = 0;
//
//                cur_intersections[key2] = std::unordered_set<std::string>(new_intersections.begin(), new_intersections.end());
//
//                for(auto& j: new_intersections) {
//                    if(!keys_intersections.count(j)) {
//                        if(keys_copy.count(j)) {
//                            keys_intersections[j] = keys_copy[j];
//                        } else {
//                            if(text.find(j) != std::string::npos) {
//                                if(j.size()>block_size) {
//                                    to_count.insert(j);
//                                } else {
//                                    keys_intersections[j] = 1;
//                                }
//                            } else {
//                                keys_intersections[j] = 0;
//                            }
//                        }
//                    }
//                }
//
//                new_intersections.clear();
//            }
////        }
//
//            to_substract = multicount(text, to_count);
//            keys_intersections.insert(to_substract.begin(), to_substract.end());
//
//            to_count.clear();
//            to_substract.clear();
//
////        for(const auto& key: recent_keys) {
//            N = keys[key];
//            for(auto& i: keys) {
//                key2 = i.first;
//                n=0;
//                for(auto& j: cur_intersections[key2]) {
//                    n += keys_intersections[j];
//
//                    if(keys.count(j)) {
//                        if(keys[j]-N<2 || keys[j]-N>keys[j]) {
//                            keys[j] = 0;
//                        } else {
//                            keys[j] -= N;
//                        }
//                    }
//                }
//
//                if(keys[key2]-n<2 || keys[key2]-n>keys[key2]) {
//                    keys[key2] = 0;
//                } else {
//                    keys[key2] -= n;
//                }
//
//            }
//            cur_intersections.clear();
//
////            std::cout<<"keys before filter: "<<keys.size()<<std::endl;
//            auto it = keys.begin();
//            while(it != keys.end()) {
//                if (it->second < 2) {
//                    it = keys.erase(it);
//                } else {
//                    ++it;
//                }
//            }
////            std::cout<<"keys after filter: "<<keys.size()<<std::endl;
//        }
//        std::cout<<"keys after filter: "<<keys.size()<<std::endl;
//    }



    keys_intersections.clear();
    std::cout<<"Splitting string..."<<std::endl;
    /// splitting text with keys in keys_stack
    std::unordered_set<std::string> keys_set(keys_stack.begin(), keys_stack.end());
//    std::vector<std::string> splitted=split_string_by_tokens(text, keys_stack);
//    std::vector<std::string> new_splitted;
    size_t last_pos=0;
    std::vector<std::string> splitted = {text, }, new_splitted;
    for(auto& key: keys_stack) {
//        for(auto& str: splitted) {
        for(auto it=splitted.begin()+last_pos; it!=splitted.end(); it++) {
            auto str = (*it);
            if(!str.empty() && !keys_set.count(str)) {
                //std::cout<<"div\t"<<str<<std::endl;
                auto div = split_string_by_token(str, key);
//                for(auto& i: div) {
//                    new_splitted.push_back(i);
//                }
                new_splitted.insert(new_splitted.end(), div.begin(), div.end());
            } else {
                new_splitted.push_back(str);
            }
        }
//        splitted = new_splitted;
        splitted.swap(new_splitted);
        new_splitted.clear();

        while(keys_set.count(splitted[last_pos])) {
            last_pos += 1;
        }

        new_splitted.insert(new_splitted.end(), splitted.begin(), splitted.begin()+last_pos);
    }
    /// clear empty vals
    splitted.erase(
                   std::remove_if(splitted.begin(),
                                  splitted.end(),
                                  [](std::string s){return s.empty();}),
                   splitted.end());

    std::cout<<"\n\n\n>>>>>>>>keys_stack:";
//    for(auto& i: keys_stack) {
//        std::cout<<"\t<"<<i<<">";
//    }
//    std::cout<<"\n\n\n>>>>>>>>splitted 1:";
//    for(auto& i: splitted) {
//        std::cout<<"\t<"<<i<<">"<<std::endl;
//    }

    std::vector<std::string> non_compressed_seqs;
    new_splitted.clear();
    size_t non_compressed_seqs_len=0, new_splitted_len=0;
    /// fetching alpha keys
    Counts<std::string> alpha_keys;
    for(auto& i: splitted) {
        if(!keys_set.count(i)) {
            //for(auto& j: i) {
            for(size_t j=0; j<i.length(); j++) {
                //auto s = (std::string)i[j];
                alpha_keys[i.substr(j, 1)] += 1;
            }
            non_compressed_seqs.push_back(i);
            new_splitted.push_back("");
            non_compressed_seqs_len += i.size();
//            alpha_keys[i] += 1;
        } else {
            new_splitted.push_back(i);
            new_splitted_len += i.size();
        }
    }

    std::cout<<"\nalpha_keys.size(): "<<alpha_keys.size();
    std::cout<<"\nnew_splitted_len: "<<new_splitted_len;
    std::cout<<"\nnon_compressed_seqs_len: "<<non_compressed_seqs_len;


    /// sorting alpha keys
    std::vector<std::pair<std::string, size_t>> alpha_keys_stack(alpha_keys.begin(), alpha_keys.end());
    //std::sort(alpha_keys_stack.begin(), alpha_keys_stack.end(), [](std::pair<std::string, size_t> &left, auto &right) {
    std::sort(alpha_keys_stack.begin(), alpha_keys_stack.end(), [](auto &left, auto &right) {
        return left.second > right.second;
    });


    /// splitting by alpha keys
//    new_splitted.clear();
//    for(auto& alpha: alpha_keys_stack) {
//        key = alpha.first;
//        keys_stack.push_back(key);
//        for(auto& str: splitted) {
//            if(!str.empty() && str.length()>1 && !keys_set.count(str)) {
//                //std::cout<<"div\t"<<str<<std::end;
//                auto div = split_string_by_token(str, key);
//                for(auto& i: div) {
//                    new_splitted.push_back(i);
//                }
//            } else {
//                new_splitted.push_back(str);
//            }
//        }
//        splitted = new_splitted;
//        new_splitted.clear();
//    }

    /// clear empty vals from splitted
    splitted.erase(
                   std::remove_if(splitted.begin(),
                                  splitted.end(),
                                  [](std::string s){return s.empty();}),
                   splitted.end());

//    std::cout<<"\n\n\n>>>>>>>>splitted 2:";
//    for(auto& i: splitted) {
//        std::cout<<"\t<"<<i<<">"<<std::endl;
//    }
    size_t summary_keys_size=0;
//    std::cout<<"\n\n\n>>>>>>>>keys_stack with alpha keys: "<<keys_stack.size()<<std::endl;
    for(auto& i: keys_stack) {
//        std::cout<<"\t<"<<i<<">";//<<std::endl;
        summary_keys_size += i.size();
    }
    std::cout<<"\n\n\n>>>>>>>>keys_stack with alpha keys: "<<keys_stack.size()<<std::endl;
    std::cout<<std::endl<<"total len of keys: "<<summary_keys_size<<std::endl;

    /// building markov chain
//    Markov<std::vector<std::string>, std::string> markov(&splitted);
    Markov<std::vector<std::string>, std::string> markov(&new_splitted);
//    MarkovChain<std::string> text_markov_chain = markov.build_chain(markov_order);
    //auto chain = markov.build_chain();

    std::vector<MarkovChain<std::string>> markov_chains;
    std::vector<size_t> markov_chains_sizes;
    size_t model_size=0, min_model;
//    size_t min_model_size=0;
    size_t min_model_size=text.size();
    markov_chains.reserve(markov_order);
    for(auto i=1; i<=markov_order; i++) {
        markov_chains.push_back(MarkovChain<std::string>(markov.build_chain(i)));
        model_size = 0;
        for(auto& j: markov_chains.back()) {
            model_size += j.second.size();
        }
        if(model_size<min_model_size) {
            min_model = markov_chains.size();
            min_model_size = model_size;
        }
        std::cout<<"\n\tOrder: "<<i<<std::endl;
        std::cout<<"\tSize: "<<model_size<<std::endl;
    }

    MarkovChain<std::string> text_markov_chain = markov_chains[min_model-1];
    markov_order = min_model;
    std::cout<<"\nMarkov model order: "<<markov_order<<std::endl;

    /// sorting keys in keys_tree by counts
    /// sorted_keys_tree - list of lists
    Counts<std::string> keys_tree_stat;
    std::map<std::string, std::vector<std::string>> sorted_keys_tree;
    for(auto& i: text_markov_chain) {
        key = i.first;
        keys_tree_stat[key] += 1;
        std::vector<std::pair<std::string, size_t>> vec((i.second).begin(), (i.second).end());
        std::sort(vec.begin(), vec.end(), [](auto &left, auto &right) {
            return left.second > right.second;
        });

        sorted_keys_tree[key] = std::vector<std::string>();
        sorted_keys_tree[key].reserve(vec.size());
        for(auto& j: vec) {
            sorted_keys_tree[key].push_back(j.first);
            keys_tree_stat[j.first] += 1;
        }

        vec.clear();
    }

    keys_tree_stat[""] = keys_tree_stat.size();

    std::vector<std::string> keys_tree_vec, keys_tree_keys(keys_stack.begin(), keys_stack.end());
    keys_tree_keys.push_back("");
    for(auto& i: keys_stack) {
        for(auto& j: sorted_keys_tree[i]) {
            keys_tree_vec.push_back(j);
        }
        keys_tree_vec.push_back("");
    }

    auto test_mtfed = mtf(keys_tree_keys, keys_tree_vec);
    Counts<size_t> test_mtf_counts;
    std::cout<<"MTF: "<<std::endl;
    for(auto& i: test_mtfed) {
        //std::cout<<"\t"<<i;
        test_mtf_counts[i] += 1;
    }

    HuffmanEncoder<size_t> test_mtf_huffenc;
    test_mtf_huffenc.InitFrequencies(test_mtf_counts);
    test_mtf_huffenc.Encode();
    auto test_mtf_huff_tree = test_mtf_huffenc.outCodes;

    size_t sum3=0, sum4=0;
    for(auto& i: test_mtf_huff_tree) {
        sum3 += i.second.size()*test_mtf_counts[i.first];
        sum4 += i.second.size();
    }

    std::cout<<"sum3: "<<sum3/8<<std::endl;
    std::cout<<"sum4: "<<sum4/8<<std::endl;
    std::cout<<"sum3+4: "<<(sum3+sum4)/8<<std::endl;

    /// huffman encoding of transitions tree
    HuffmanEncoder<std::string> huff_string;
    huff_string.InitFrequencies(keys_tree_stat);
    huff_string.Encode();
    auto huff_tree_str = huff_string.outCodes;

//    std::cout<<"\n\nhuff_tree_str: "<<std::endl;
//    for(auto& i: huff_tree_str) {
//        std::cout<<"\t<"<<i.first<<"> ";
//        std::copy(i.second.begin(), i.second.end(), std::ostream_iterator<bool>(std::cout));
//        std::cout<<std::endl;
//    }

    size_t encoded_transition_tree_size = 0, encoded_transition_tree_len = 0;
    size_t sum1=0, sum2=0;
    for(auto& i: sorted_keys_tree) {
//        sum += sizeof(i.first);// + sizeof(i.second);
//        sum1 += sizeof(i.first);
//        sum2 += i.second.size();
        encoded_transition_tree_size += huff_tree_str[i.first].size();
        encoded_transition_tree_len += i.first.size();
        for(auto& j: i.second) {
//            sum += sizeof(j);
                encoded_transition_tree_size += huff_tree_str[j].size();
                encoded_transition_tree_len += j.size();
        }
    }
    //std::cout<<"\nsizeof(sorted_keys_tree) = "<<sizeof(sorted_keys_tree)<<"\t"<<sum<<"\t"<<sum1<<"\t"<<sum2<<std::endl;
    std::cout<<"\nencoded_transition_tree_size = "<<encoded_transition_tree_size<<std::endl;
    ///sum1 - суммарный размер ключей

    /// encoding of text with transitions codes
    //std::string key1, key2;
    std::vector<size_t> encoded;
    size_t code, max_code=0;
    Counts<size_t> stat;
    std::cout<<"\n\nencoded: ";
//    for(auto it=splitted.begin()+1; it!=splitted.end()-1; it++) {
    for(auto it=splitted.begin(); it!=splitted.end()-markov_order; it++) {
        key1 = *it;
//        key2 = *(it+1);
        key2 = *(it+markov_order);

        code = std::find(sorted_keys_tree[key1].begin(), sorted_keys_tree[key1].end(), key2) - sorted_keys_tree[key1].begin() + 1;
//        code = std::distance(std::find(sorted_keys_tree[key1].begin(), sorted_keys_tree[key1].end(), key2), sorted_keys_tree[key1].begin());
        encoded.push_back(code);
        std::cout<<" "<<code;
//        stat[code] += 1;
        max_code = std::max(code, max_code);
    }

    ///RLE
    std::vector<size_t> rled;
    rled.reserve(encoded.size());
    size_t cur_code=encoded[0], run_len=0;
    for(auto it=encoded.begin(); it!=encoded.end(); it++) {
        if((*it)==cur_code) {
            run_len += 1;
        } else {
            rled.push_back(run_len);
            rled.push_back(cur_code);

            stat[run_len] += 1;
            stat[cur_code] += 1;

            run_len = 1;
            cur_code = (*it);
        }
    }

//    std::cout<<"\t\tencoded.size\thuff_tree\tkeys tree\t"<<std::endl;
    std::cout<<"\n\n\n>>>>>>>>encoded: "<<encoded.size()<<std::endl;
    std::cout<<"\n\n\n>>>>>>>>rled: "<<rled.size()<<std::endl;

    HuffmanEncoder<size_t> huff;
    huff.InitFrequencies(stat);
    huff.Encode();
    auto huff_tree = huff.outCodes;

    size_t total=0;
    std::cout<<"huff.outCodes: ";
    for(auto& i: huff.outCodes) {
        std::cout<<"\t"<<i.first<<"-"<<i.second.size(); //<<std::endl;
    }
//    std::cout<<"total:\t"<<sum1;
//    std::cout<<"total:\t";
//    total += sum1;
//    sum=0;

    size_t encoded_text_size = 0;
//    for(auto& i: rled) {
    for(auto& i: encoded) {
//        sum += huff_tree[i].size();
        encoded_text_size += huff_tree[i].size();
    }
//    std::cout<<"\t"<<sum/8;
//    total += sum/8;
    ///sum - размер закодированного текста

//    sum=0;
//    for(auto& i: sorted_keys_tree) {
//        sum += huff_tree_str[i.first].size();
//        for(auto& j: i.second) {
//            sum += huff_tree_str[j].size();
//        }
//    }
//    std::cout<<"\t"<<sum/8;
//    total += sum/8;
    ///sum - суммарный размер закодированного дерева переходов

    //std::cout<<"\t"<<total<<std::endl;


    compressed_block.keys_count = keys_stack.size();

    std::vector<std::string> ordered_keys;
    ordered_keys.reserve(keys.size());

    for(auto& i: keys_stack) {
        ordered_keys.push_back(i);
    }

    std::sort(ordered_keys.begin(), ordered_keys.end());
    //std::sort(ordered_keys.begin(), ordered_keys.end(), [](auto a, auto b){return a.size()>b.size();});

    std::string keys_string = boost::algorithm::join(ordered_keys, "");
//    for(auto& i: ordered_keys) {
//        keys_string += i;
//    }

    std::string compressed_keys_string = Gzip::compress(keys_string);

    std::string non_compressed_text = boost::algorithm::join(non_compressed_seqs, "");
    std::string compressed_non_compressed_text = Gzip::compress(non_compressed_text);

    std::cout<<"\nmax_code: "<<max_code;
    std::cout<<"\nkeys count: "<<keys_stack.size()<<std::endl;
    std::cout<<"\nkeys count**2/8: "<<keys_stack.size()*keys_stack.size()/8<<std::endl;

    std::cout<<"\nnon_compressed:"<<std::endl;
    std::cout<<"\tnon_compressed_text.size(): "<<non_compressed_text.size()<<std::endl;
    std::cout<<"\tkeys_string.size(): "<<keys_string.size()<<std::endl;

    std::cout<<"\ntotal:"<<std::endl;
    std::cout<<"\tcompressed_keys_string.size(): "<<compressed_keys_string.size()<<std::endl;
    std::cout<<"\tencoded_transition_tree_size: "<<encoded_transition_tree_size/8<<std::endl;
    std::cout<<"\tencoded_text_size: "<<encoded_text_size/8<<std::endl;
    std::cout<<"\tcompressed_non_compressed_text.size(): "<<compressed_non_compressed_text.size()<<std::endl;

    std::cout<<"\ttotal: "<<keys_string.size()+(encoded_transition_tree_size+encoded_text_size)/8+compressed_non_compressed_text.size()<<std::endl;

    std::cout<<"\nencoded_transition_tree_len: "<<encoded_transition_tree_len<<std::endl;
    std::cout<<"\nSize of keys_string: "<<keys_string.size()<<" "<<sizeof(keys_string)<<std::endl;
    std::cout<<"Size of compressed_keys_string: "<<compressed_keys_string.size()<<" "<<sizeof(compressed_keys_string)<<std::endl;

    //auto packed_trans_tree = huff_string.pack(ordered_keys);

//    HuffCode keys_mask;
//    HuffCode keys_huffman_codes;
//    HuffCode keys_huffman_codes_mask;
//
//    bool cur_mask = 1;
//    for(const auto& key: ordered_keys) {
//        keys_mask.insert(keys_mask.end(), key.size(), cur_mask);
//        keys_huffman_codes.insert(keys_huffman_codes.end(), huff_string.outCodes[key].begin(), huff_string.outCodes[key].end());
//        keys_huffman_codes_mask.insert(keys_huffman_codes_mask.end(), huff_string.outCodes[key].size(), cur_mask);
//
//        cur_mask ^= 1;
//    }
//
    std::vector<size_t> stat_vec(max_code+1);
//    stat_vec.reserve(max_code);
//    std::iota(stat_vec.begin(), stat_vec.end(), 0);
    size_t ii=0;
    for(auto it=stat_vec.begin(); it!=stat_vec.end(); it++) {
        (*it) = ii;
        ii++;
    }

    std::cout<<"Max code: "<<max_code<<std::endl;

//    auto mtfed = mtf(ordered_keys, splitted);
    auto mtfed = mtf(stat_vec, encoded);
//    auto mtfed = mtf(stat_vec, rled);
    Counts<size_t> mtf_counts;
    std::cout<<"\nMTF: "<<std::endl;
    for(auto& i: mtfed) {
        //std::cout<<"\t"<<i;
        mtf_counts[i] += 1;
    }

    HuffmanEncoder<size_t> mtf_huffenc;
    mtf_huffenc.InitFrequencies(mtf_counts);
    mtf_huffenc.Encode();
    auto mtf_huff_tree = mtf_huffenc.outCodes;

    size_t sum5=0, sum6=0;
    for(auto& i: mtf_huff_tree) {
        sum5 += i.second.size()*mtf_counts[i.first];
        sum6 += i.second.size();
    }

    std::cout<<"sum5: "<<sum5/8<<std::endl;
    std::cout<<"sum6: "<<sum6/8<<std::endl;
    std::cout<<"sum5+6: "<<(sum5+sum6)/8<<std::endl;

    std::cout<<"\nBzip2.compress(text).size(): "<<Bzip2::compress(text).size();
    std::cout<<"\nGzip.compress(text).size(): "<<Gzip::compress(text).size()<<std::endl;

    std::cout<<"\n\nMTF(encoded): ";
    for(auto& i: mtfed) {
        std::cout<<" "<<i;
    }

//    std::cout<<"\n\nPress any key to compress uncompressed"<<std::endl;
//    std::cin>>n;
//    compress_block(non_compressed_text, block_size, markov_order);

    return res;
}

std::string decompress(std::string text) {
    return "";
}
