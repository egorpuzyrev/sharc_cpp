#include <string>
#include <map>
#include <unordered_set>
#include <algorithm>
#include <iostream>
#include <cmath>
#include "global.hpp"
#include "statistics.hpp"
#include "keys.hpp"
#include "huffman.hpp"
#include "markov.hpp"
#include "support.hpp"
#include "compressor.hpp"

const char ID[] = "sharc";
const char VERSION[] = "02";

typedef struct CompressedFile_s {
    /// section 1
    /// format description
    char id[5];
    char version[2];

    /// section 2
    /// blocks description
    uint64_t compressed_block_length;
    char preprocessor;
    char keys_compression_method;

    uint32_t keys_count;
    uint32_t uncompressed_keys_list_length;
    uint32_t compressed_keys_list_length;
    uint32_t terminator_code_length;

    char* keys;
    char* keys_mask;

    char* terminator_code;
    char* keys_codes;
    char* transitions_codes;

    uint32_t encoded_message_length;
    uint32_t transitions_keys_count;
    uint32_t transitions_keys_length;
    char* transitions_leys_mask;

} CompressedFile;


std::string compress(std::string text, size_t block_size) {
    std::string res;

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
    Counts<std::string> keys = get_keys_by_lcp(text, 2, 1, block_size, block_size);
    Counts<std::string> keys_copy(keys.begin(), keys.end());
    std::cout<<">>>calculation of keys_stack"<<std::endl;
    while(keys.size()) {
        std::cout<<"keys left: "<<keys.size()<<std::endl;
        weights = get_weights(get_weight5, keys, L1, bi, B);
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
        keys[key] = 0;

        std::cout<<">>>>>>>>intersections: "<<key<<"\t"<<keys.count(key)<<std::endl;
        std::vector<std::string> new_intersections;
        n = 0;
        //bool t1, t2;
        //#pragma omp parallel
        for(auto& i: keys) {
            key2 = i.first;
            new_intersections = get_keys_intersections(key, key2);
            n = 0;
            for(auto& j: new_intersections) {
                if(!keys_intersections.count(j)) {
                    if(keys_copy.count(j)) {
                        keys_intersections[j] = keys_copy[j];
                    } else {
                        keys_intersections[j] = countSubstring(text, j);
                    }
                }
                n += keys_intersections[j];
                //n += keys_intersections[j];

                if(keys.count(j)) {
                    if(keys[j]-N<2 || keys[j]-N>keys[j]) {
                        keys[j] = 0;
                    } else {
                        keys[j] -= N;
                    }
                }
            }
            if(keys.count(key2)) {
                if(keys[key2]-n<2 || keys[key2]-n>keys[key2]) {
                    keys[key2] = 0;
                } else {
                    keys[key2] -= n;
                }
            }
        }
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
    for(auto& i: keys_stack) {
        std::cout<<"\t<"<<i<<">"<<std::endl;
    }
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
        for(auto& j: vec) {
            sorted_keys_tree[key].push_back(j.first);
            keys_tree_stat[j.first] += 1;
        }

        vec.clear();
    }

    HuffmanEncoder<std::string>huff_string;
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
    size_t code;
    Counts<size_t> stat;
    for(auto it=splitted.begin()+1; it!=splitted.end()-1; it++) {
        key1 = *it;
        key2 = *(it+1);

        code = std::find(sorted_keys_tree[key1].begin(), sorted_keys_tree[key1].end(), key2) - sorted_keys_tree[key1].begin();
        encoded.push_back(code);
        stat[code] += 1;
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

    return res;
}

std::string decompress(std::string text) {
    return "";
}
