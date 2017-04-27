#include <string>
#include <map>
#include <unordered_set>
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

#include <easy/profiler.h>


std::string compress_block(std::string text, size_t block_size) {
    EASY_FUNCTION();
    std::string res;
    CompressedBlock compressed_block;

    float max_weight, second_max_weight;
    std::string s, key, second_key, key1, key2;

    size_t n, N, bi=2, L=text.length();
    size_t L1=L;
    std::vector<std::string> keys_stack;
    Counts<std::string> keys_intersections;
    fCounts<std::string> weights;


    /// calculation of keys_stack
    std::cout<<">>>get_keys_naive()"<<std::endl;
//    Counts<std::string> keys = get_keys_naive(text, 2, 1, 1);
    EASY_BLOCK("Get keys block");
    Counts<std::string> keys = get_keys_by_lcp(text, 2, 2, block_size, block_size);
    EASY_END_BLOCK

//    std::ofstream out;
//    out.open("keys.csv");
//    for(auto& i: keys) {
//        out<<keys.first;
//        out<<"\t";
//        out<<keys.second<<std::endl;
//    }
//    out.close();

    Counts<std::string> keys_copy(keys.begin(), keys.end());
    Counts<std::string> last_keys_counts;

//    EASY_BLOCK("Calculation of keys_stack block");
    std::cout<<">>>calculation of keys_stack"<<std::endl;
    bool flag, independent_key_found;
    std::vector<std::string> new_intersections, recent_keys;
    std::unordered_set<std::string> to_count;
    std::map<std::string, std::unordered_set<std::string>> cur_intersections;

    Counts<std::string> to_substract, last_checked_pos;
    std::map<std::string, std::map<std::string, std::vector<std::string>>> to_check;

//    std::vector<std::string> sorted_by_weights;
    std::vector<std::pair<std::string, float>> sorted_by_weights;
    while(keys.size()) {
//        std::cout<<"keys left: "<<keys.size()<<std::endl;
        weights = get_weights(get_weight6, keys, L1, bi, B);
        max_weight = -1.0;
        second_max_weight = -1.0;
//        recent_keys.clear();
        flag = false;
        independent_key_found = false;

//        EASY_BLOCK("Choosing key with max wei");
        std::cout<<"\n\nChoosing key with max weight..."<<std::endl;
        for(auto& i: weights) {
            if(i.second>max_weight) {
                max_weight = i.second;
                key2 = i.first;
            }
        }
//        EASY_END_BLOCK;

        //weights[key2] = -1;
        weights.erase(key2);

        std::cout<<"Checking next key: <"<<key2<<"> with size: "<<key2.size()<<std::endl;
        std::cout<<"Checking for intersections..."<<std::endl;
        for(auto it=keys_stack.begin()+last_checked_pos[key2]; it!=keys_stack.end(); it++) {
            auto key = (*it);
            last_checked_pos[key2] += 1;
            new_intersections = get_keys_intersections(key, key2);
//            std::cout<<"new_intersections.size(): "<<new_intersections.size()<<std::endl;

            if(!new_intersections.empty()) {
//                std::cout<<new_intersections[0]<<std::endl;
                flag = true;

                to_check[key][key2] = std::vector<std::string>(new_intersections.begin(), new_intersections.end());
            }
        }


        if(flag) {
            std::cout<<"Intersections found"<<std::endl;
            sorted_by_weights.clear();
//            sorted_by_weights.reserve(weights.size());
            std::cout<<"Sorting keys by max weight"<<std::endl;
//            for(const auto& i: weights) {
//                sorted_by_weights.push_back(i.first);
//            }
//            std::sort(sorted_by_weights.begin(), sorted_by_weights.end(), [&weights](const auto& a, const auto& b){return weights[a]<weights[b];});

            sorted_by_weights.insert(sorted_by_weights.end(), weights.begin(), weights.end());
            std::sort(sorted_by_weights.begin(), sorted_by_weights.end(), [](const auto& a, const auto& b){return a.second<b.second;});

            std::cout<<"Looking for independent key..."<<std::endl;
            independent_key_found = false;
            while(!independent_key_found) {
                key2 = sorted_by_weights.back().first;
                sorted_by_weights.pop_back();

//                weights.erase(key2);
                independent_key_found = true;
                for(auto it=keys_stack.begin()+last_checked_pos[key2]; it!=keys_stack.end(); it++) {
                    auto key = (*it);
                    last_checked_pos[key2] += 1;
                    new_intersections = get_keys_intersections(key, key2);

                    if(!new_intersections.empty()) {
                        independent_key_found = false;
                        to_check[key2][key] = std::vector<std::string>(new_intersections.begin(), new_intersections.end());
                    }
                }
            }
            std::cout<<"Independent key found: <"<<key2<<">"<<std::endl;


            std::cout<<"Looking for subs need to be counted..."<<std::endl;
            for(auto& it: to_check) {
                for(auto& it1: it.second) {
                    new_intersections = it1.second;

                    for(auto& j: new_intersections) {
                        if(!keys_intersections.count(j)) {
                            if(keys_copy.count(j)) {
                                keys_intersections[j] = keys_copy[j];
                            } else {
                                if(text.find(j) != std::string::npos) {
                                    if(j.size()>block_size) {
                                        to_count.insert(j);
                                    } else {
                                        keys_intersections[j] = 1;
                                    }
                                } else {
                                    keys_intersections[j] = 0;
                                }
                            }
                        }
                    }
                }
            }
            std::cout<<"Need to count: "<<to_count.size()<<std::endl;
//            EASY_BLOCK("Counting intersections...");
            std::cout<<"Counting intersections (multicount)"<<std::endl;
            to_substract = multicount(text, to_count);
            keys_intersections.insert(to_substract.begin(), to_substract.end());
//            EASY_END_BLOCK;

            to_count.clear();
            to_substract.clear();

            std::cout<<"Substracting..."<<std::endl;
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
    //                                keys[j] = 0;
                                keys.erase(j);
                            } else {
                                keys[j] -= N;
                            }
                        }
                    }
                }

                if(keys[key2]-n<2 || keys[key2]-n>keys[key2]) {
                    keys.erase(key2);
                } else {
                    keys[key2] -= n;
                }
            }

            to_check.clear();

        } else {
            std::cout<<"Intersections not found"<<std::endl;
            std::cout<<"new key = <"<<key2<<"> "<<keys[key2]<<std::endl;
            std::cout<<"keys left: "<<keys.size()<<std::endl;
            keys_stack.push_back(key2);
            bi += 1;
            L1 -= key.size()*keys[key2]+ceil(bi*keys[key2]/B);

            last_keys_counts[key] = keys[key];
            keys.erase(key2);
            weights.erase(key2);
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

    /// splitting text with keys in keys_stack
    std::unordered_set<std::string> keys_set(keys_stack.begin(), keys_stack.end());
    std::vector<std::string> splitted = {text, }, new_splitted;
    for(auto& key: keys_stack) {
        for(auto& str: splitted) {
            if(!str.empty() && keys_set.find(str)==keys_set.end()) {
                //std::cout<<"div\t"<<str<<std::endl;
                auto div = split_string_by_token(str, key);
                for(auto& i: div) {
                    new_splitted.push_back(i);
                }
            } else {
                new_splitted.push_back(str);
            }
        }
        splitted = new_splitted;
        new_splitted.clear();
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
    MarkovChain<std::string> text_markov_chain = markov.build_chain();
    //auto chain = markov.build_chain();

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
    for(auto it=splitted.begin()+1; it!=splitted.end()-1; it++) {
        key1 = *it;
        key2 = *(it+1);

        code = std::find(sorted_keys_tree[key1].begin(), sorted_keys_tree[key1].end(), key2) - sorted_keys_tree[key1].begin() + 1;
//        code = std::distance(std::find(sorted_keys_tree[key1].begin(), sorted_keys_tree[key1].end(), key2), sorted_keys_tree[key1].begin());
        encoded.push_back(code);
        stat[code] += 1;
        max_code = std::max(code, max_code);
    }

//    std::cout<<"\t\tencoded.size\thuff_tree\tkeys tree\t"<<std::endl;
    std::cout<<"\n\n\n>>>>>>>>encoded: "<<encoded.size()<<std::endl;

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
    Counts<size_t> mtf_counts;
    std::cout<<"MTF: "<<std::endl;
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

    return res;
}

std::string decompress(std::string text) {
    return "";
}
