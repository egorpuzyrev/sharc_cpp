#include <string>
#include <map>
#include <unordered_set>
#include <algorithm>
#include <iostream>
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

    float max_weight;
    std::string s, key, key1, key2;

    size_t n, N, bi=2, L=text.length();
    size_t L1=L;
    std::vector<std::string> keys_stack;
    Counts<std::string> keys_intersections;
    fCounts<std::string> weights;


    /// calculation of keys_stack
    std::cout<<">>>get_keys_naive()"<<std::endl;
//    Counts<std::string> keys = get_keys_naive(text, 2, 1, 1);
    EASY_BLOCK("Get keys block");
    Counts<std::string> keys = get_keys_by_lcp(text, 2, 1, block_size, block_size);
    EASY_END_BLOCK

    Counts<std::string> keys_copy(keys.begin(), keys.end());

    EASY_BLOCK("Calculation of keys_stack block");
    std::cout<<">>>calculation of keys_stack"<<std::endl;
    while(keys.size()) {
        std::cout<<"keys left: "<<keys.size()<<std::endl;
        weights = get_weights(get_weight1, keys, L1, bi, B);
        max_weight = -1.0;
        for(auto& i: weights) {
            if(i.second>max_weight) {
                max_weight = i.second;
                key = i.first;
//                std::cout<<"max_weight: "<<max_weight<<std::endl;
//                std::cout<<"max key: "<<key<<std::endl;
            }
        }

        std::cout<<"new key = <"<<key<<"> "<<keys[key]<<std::endl;
        keys_stack.push_back(key);
        bi += 1;
        L1 -= key.size()*keys[key]+ceil(bi*keys[key]/B);

        N = keys[key];
        keys.erase(key);
        //keys[key] = 0;

        std::cout<<">>>>>>>>intersections: "<<key<<"\t"<<keys.count(key)<<std::endl;
        std::vector<std::string> new_intersections;
        std::unordered_set<std::string> to_count;
        std::map<std::string, std::unordered_set<std::string>> cur_intersections;

        n = 0;
        //bool t1, t2;
        //#pragma omp parallel
        EASY_BLOCK("Calculation of intersections block");
        for(auto& i: keys) {

            key2 = i.first;
            //std::cout<<"key2: "<<key2<<std::endl;
            new_intersections = get_keys_intersections(key, key2);
            n = 0;

            cur_intersections[key2] = std::unordered_set<std::string>(new_intersections.begin(), new_intersections.end());
            //std::cout<<"Sub"<<std::endl;
            //to_count.clear();
            for(auto& j: new_intersections) {
                if(!keys_intersections.count(j)) {
                    if(keys_copy.count(j)) {
                        keys_intersections[j] = keys_copy[j];
                    } else {
//                        keys_intersections[j] = countSubstring(text, j);
//                        to_count.emplace(j);
                        to_count.insert(j);
                    }
                }
            }
        }

        Counts<std::string> to_substract = multicount(text, to_count);
        keys_intersections.insert(to_substract.begin(), to_substract.end());
        //to_substract.clear();

        for(auto& i: keys) {

            key2 = i.first;
            n=0;
            for(auto& j: cur_intersections[key2]) {
                if(keys_intersections.count(j)) {
                    n += keys_intersections[j];
                }

                if(keys.count(j)) {
                    if(keys[j]-N<2 || keys[j]-N>keys[j]) {
                        keys[j] = 0;
                    } else {
                        keys[j] -= N;
                    }
                }
            }

//            if(n>0) {
//                std::cout<<"<"<<key<<">\t<"<<key2<<">\t"<<n<<std::endl;
//            }

//            if(keys.count(key2)) {
                if(keys[key2]-n<2 || keys[key2]-n>keys[key2]) {
                    keys[key2] = 0;
                } else {
                    keys[key2] -= n;
                }
//            }
        }
        EASY_END_BLOCK;
//        std::cout<<"5"<<std::endl;
        std::cout<<"keys before filter: "<<keys.size()<<std::endl;
        auto it = keys.begin();
        while(it != keys.end()) {
            if (it->second < 2) {
                it = keys.erase(it);
            } else {
                ++it;
            }
        }
        std::cout<<"keys after filter: "<<keys.size()<<std::endl;
//        splitted.erase(
//               std::remove_if(splitted.begin(),
//                              splitted.end(),
//                              [](std::string s){return s.empty();}),
//               splitted.end());
        //std::cin>>s;
    }
    EASY_END_BLOCK;

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

    /// fetching alpha keys
    Counts<std::string> alpha_keys;
    for(auto& i: splitted) {
        if(keys_set.find(i)==keys_set.end()) {
            //for(auto& j: i) {
            for(size_t j=0; j<i.length(); j++) {
                //auto s = (std::string)i[j];
                alpha_keys[i.substr(j, 1)] += 1;
            }
        }
    }

    /// sorting alpha keys
    std::vector<std::pair<std::string, size_t>> alpha_keys_stack(alpha_keys.begin(), alpha_keys.end());
    //std::sort(alpha_keys_stack.begin(), alpha_keys_stack.end(), [](std::pair<std::string, size_t> &left, auto &right) {
    std::sort(alpha_keys_stack.begin(), alpha_keys_stack.end(), [](auto &left, auto &right) {
        return left.second > right.second;
    });


    /// splitting by alpha keys
    for(auto& alpha: alpha_keys_stack) {
        key = alpha.first;
        keys_stack.push_back(key);
        for(auto& str: splitted) {
            if(!str.empty() && str.length()>1 && keys_set.find(str)==keys_set.end()) {
                //std::cout<<"div\t"<<str<<std::end;
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
    size_t sum=0;
    std::cout<<"\n\n\n>>>>>>>>keys_stack with alpha keys: "<<keys_stack.size()<<std::endl;
    for(auto& i: keys_stack) {
        std::cout<<"\t<"<<i<<">";//<<std::endl;
        sum += i.size();
    }
    std::cout<<"\n\n\n>>>>>>>>keys_stack with alpha keys: "<<keys_stack.size()<<std::endl;
    std::cout<<std::endl<<"total len of keys: "<<sum<<std::endl;

    /// building markov chain
    Markov<std::vector<std::string>, std::string> markov(&splitted);
    MarkovChain<std::string> chain = markov.build_chain();
    //auto chain = markov.build_chain();

    /// sorting keys in keys_tree by counts
    Counts<std::string> keys_tree_stat;
    std::map<std::string, std::vector<std::string>> sorted_keys_tree;
    for(auto& i: chain) {
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

    HuffmanEncoder<std::string> huff_string;
    huff_string.InitFrequencies(keys_tree_stat);
    huff_string.Encode();
    auto huff_tree_str = huff_string.outCodes;

    sum = 0;
    size_t sum1=0, sum2=0;
    for(auto& i: sorted_keys_tree) {
        sum += sizeof(i.first);// + sizeof(i.second);
        sum1 += sizeof(i.first);
        sum2 += i.second.size();
        for(auto& j: i.second) {
            sum += sizeof(j);
        }
    }
    std::cout<<"\nsizeof(sorted_keys_tree) = "<<sizeof(sorted_keys_tree)<<"\t"<<sum<<"\t"<<sum1<<"\t"<<sum2<<std::endl;
    ///sum1 - суммарный размер ключей

    /// encoding
    //std::string key1, key2;
    std::vector<size_t> encoded;
    size_t code, max_code=0;
    Counts<size_t> stat;
    for(auto it=splitted.begin()+1; it!=splitted.end()-1; it++) {
        key1 = *it;
        key2 = *(it+1);

        code = std::find(sorted_keys_tree[key1].begin(), sorted_keys_tree[key1].end(), key2) - sorted_keys_tree[key1].begin();
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

    std::cout<<"total:\t"<<sum1;
    total += sum1;
    sum=0;
    for(auto& i: encoded) {
        sum += huff_tree[i].size();
    }
    std::cout<<"\t"<<sum/8;
    total += sum/8;
    ///sum - размер закодированного текста

    sum=0;
    for(auto& i: sorted_keys_tree) {
        sum += huff_tree_str[i.first].size();
        for(auto& j: i.second) {
            sum += huff_tree_str[j].size();
        }
    }
    std::cout<<"\t"<<sum/8;
    total += sum/8;
    ///sum - суммарный размер закодированного дерева переходов

    std::cout<<"\t"<<total<<std::endl;


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

    std::cout<<"Size of keys_string: "<<keys_string.size()<<" "<<sizeof(keys_string)<<std::endl;
    std::cout<<"Size of compressed_keys_string: "<<compressed_keys_string.size()<<" "<<sizeof(compressed_keys_string)<<std::endl;

    //auto packed_trans_tree = huff_string.pack(ordered_keys);

    HuffCode keys_mask;
    HuffCode keys_huffman_codes;
    HuffCode keys_huffman_codes_mask;

    bool cur_mask = 1;
    for(const auto& key: ordered_keys) {
        keys_mask.insert(keys_mask.end(), key.size(), cur_mask);
        keys_huffman_codes.insert(keys_huffman_codes.end(), huff_string.outCodes[key].begin(), huff_string.outCodes[key].end());
        keys_huffman_codes_mask.insert(keys_huffman_codes_mask.end(), huff_string.outCodes[key].size(), cur_mask);

        cur_mask ^= 1;
    }

    std::vector<size_t> stat_vec(max_code);
    std::iota(stat_vec.begin(), stat_vec.end(), 0);


    auto mtfed = mtf(ordered_keys, splitted);
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


    return res;
}

std::string decompress(std::string text) {
    return "";
}
