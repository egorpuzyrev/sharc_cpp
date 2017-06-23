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

#include "bwt.hpp"

#include "ahocorasik.hpp"

//#include <easy/profiler.h>

const size_t ALPHA_LIMIT=0;

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

    std::unordered_map<std::string, float> weights, new_weights;

    std::unordered_set<std::string> recalculate;

    /// calculation of keys_stack
    std::cout<<">>>get_keys_naive()"<<std::endl;

    Counts<std::string> keys_copy = get_keys_by_lcp(text, 1, ALPHA_LIMIT, block_size, block_size);


    std::unordered_map<size_t, std::set<std::string>> keys_by_lengths;
    {
        std::unordered_set<size_t> keys_lengths;
        for(auto& i: keys_copy) {
            keys_lengths.insert(i.first.size());
        }
        for(auto& i: keys_lengths) {
            keys_by_lengths[i] = std::set<std::string>();
        }
    }

    for(auto& i: keys_copy) {
        keys_by_lengths[i.first.size()].insert(i.first);
    }

    std::unordered_map<std::string, size_t> keys_intersections(keys_copy.begin(), keys_copy.end());
    Counts<std::string> keys;

    float limweight = 0;//get_weight2("--", 2, L1, bi, B);

    for(const auto& i: keys_copy) {
        if(i.first.size()>=ALPHA_LIMIT && i.second>=2 && get_weight2(i.first, i.second, L1, bi, B)>limweight) {
            keys[i.first] = i.second;
        }
    }
    keys_copy.clear();

    Counts<std::string> last_keys_counts;


    std::cout<<">>>calculation of keys_stack"<<std::endl;
    bool flag, independent_key_found;
    std::vector<std::string> new_intersections, recent_keys;
    std::unordered_set<std::string> to_count;
    std::map<std::string, std::unordered_set<std::string>> cur_intersections;

    Counts<std::string> to_substract, last_checked_pos;

    std::unordered_map<std::string, std::unordered_map<std::string, std::vector<std::string>>> to_check;


    std::vector<std::pair<std::string, float>> sorted_by_weights;

    weights = get_uweights(wei_fun, keys, L1, bi, B);
    sorted_by_weights.reserve(weights.size());
    sorted_by_weights.insert(sorted_by_weights.end(), weights.begin(), weights.end());
    std::sort(sorted_by_weights.begin(), sorted_by_weights.end(), [](const auto& a, const auto& b){return a.second<b.second;});
    weights.clear();
    std::cout<<"sorted_by_weights.size(): "<<sorted_by_weights.size()<<std::endl;
    std::cout<<"keys.size(): "<<keys.size()<<std::endl;

    while(keys.size()) {

        max_weight = -1.0;

        flag = false;
        independent_key_found = false;

        key2 = sorted_by_weights.back().first;
        sorted_by_weights.pop_back();

        for(auto it=keys_stack.begin()+last_checked_pos[key2]; it!=keys_stack.end(); it++) {
            auto key = (*it);
            last_checked_pos[key2] += 1;
            new_intersections = get_keys_intersections_wh(key, key2);

            if(!new_intersections.empty()) {

                flag = true;

                to_check[key2][key] = std::vector<std::string>(new_intersections.begin(), new_intersections.end());
                for(auto& j: new_intersections) {
                    if(!keys_intersections.count(j)) {
                        to_count.insert(j);
                    }
                }
            }
        }


        if(flag) {

            independent_key_found = false;
            while(!independent_key_found && !sorted_by_weights.empty()) {

                key2 = sorted_by_weights.back().first;
                sorted_by_weights.pop_back();

                independent_key_found = true;
                for(auto it=keys_stack.begin()+last_checked_pos[key2]; it!=keys_stack.end(); it++) {
                    auto key = (*it);
                    last_checked_pos[key2] += 1;
                    new_intersections = get_keys_intersections_wh(key, key2);

                    if(!new_intersections.empty()) {
                        independent_key_found = false;

                        to_check[key2][key] = std::vector<std::string>(new_intersections.begin(), new_intersections.end());

                        for(auto& j: new_intersections) {
                            if(!keys_intersections.count(j)) {
                                to_count.insert(j);
                            }
                        }
                    }

                }
            }

            if(independent_key_found) {

                weights[key2] = wei_fun(key2, keys[key2], L1, bi, B);
            }



            to_substract = multicount_aho(text, to_count);

            keys_intersections.insert(to_substract.begin(), to_substract.end());

            to_count.clear();
            to_substract.clear();

            limweight = 0;//get_weight2("--", 2, L1, bi, B);
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
                            } else {
                                keys[j] -= N;
                                weights[j] = wei_fun(j, keys[j], L1, bi, B);
                                if(get_weight2(j, keys[j], L1, bi, B)<=limweight) {
                                    keys[j] = 0;
                                    weights[j] = 0;
                                }

                            }
                        }
                    }
                }


                if(keys[key2]-n<2 || keys[key2]-n>keys[key2]) {
                    keys[key2] = 0;
                    weights[key2] = 0;
                } else {
                    keys[key2] -= n;
                    weights[key2] = wei_fun(key2, keys[key2], L1, bi, B);
                    if(get_weight2(key2, keys[key2], L1, bi, B)<=limweight) {
                        keys[key2] = 0;
                        weights[key2] = 0;
                    }
                }
            }


            for(auto it = keys.begin(); it != keys.end(); ) {
                if (it->second < 2) {
                    weights[it->first] = 0;
                    it = keys.erase(it);
                } else {
                    ++it;
                }
            }

            for(auto it=sorted_by_weights.begin(); it!=sorted_by_weights.end(); it++) {
                if(weights.count(it->first)) {
                    it->second = weights[it->first];
                    weights[it->first] = 0;
                }
            }

            sorted_by_weights.reserve(sorted_by_weights.size()+weights.size());
            sorted_by_weights.insert(sorted_by_weights.end(), weights.begin(), weights.end());

            sorted_by_weights.erase(
                   std::remove_if(sorted_by_weights.begin(),
                                  sorted_by_weights.end(),
                                  [](auto& x){return x.second==0.0;}),
                   sorted_by_weights.end());

            std::sort(sorted_by_weights.begin(), sorted_by_weights.end(), [](const auto& a, const auto& b){return a.second<b.second;});

            to_check.clear();
            weights.clear();

        } else {
            std::cout<<"new key = <"<<key2<<"> "<<keys[key2]<<std::endl;
            std::cout<<"keys left: "<<keys.size()<<std::endl;
            keys_stack.push_back(key2);
            bi += 1;
            L1 -= key.size()*keys[key2]+ceil(bi*keys[key2]/B);

            last_keys_counts[key2] = keys[key2];
            keys.erase(key2);
        }
    }


    keys_intersections.clear();
    std::cout<<"Splitting string..."<<std::endl;
    /// splitting text with keys in keys_stack
    std::unordered_set<std::string> keys_set(keys_stack.begin(), keys_stack.end());

    size_t last_pos=0;
    std::vector<std::string> splitted = {text, }, new_splitted;
    for(auto& key: keys_stack) {

        for(auto it=splitted.begin()+last_pos; it!=splitted.end(); it++) {
            auto str = (*it);
            if(!str.empty() && !keys_set.count(str)) {
                auto div = split_string_by_token(str, key);
                new_splitted.insert(new_splitted.end(), div.begin(), div.end());
            } else {
                new_splitted.push_back(str);
            }
        }

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

    std::vector<std::string> non_compressed_seqs;
    new_splitted.clear();
    size_t non_compressed_seqs_len=0, new_splitted_len=0;
    /// fetching alpha keys
    Counts<std::string> alpha_keys;
    for(auto& i: splitted) {
        if(!keys_set.count(i)) {
            for(size_t j=0; j<i.length(); j++) {
                alpha_keys[i.substr(j, 1)] += 1;
            }
            non_compressed_seqs.push_back(i);
            new_splitted.push_back("");
            non_compressed_seqs_len += i.size();
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

    std::sort(alpha_keys_stack.begin(), alpha_keys_stack.end(), [](auto &left, auto &right) {
        return left.second > right.second;
    });


    /// clear empty vals from splitted
    splitted.erase(
                   std::remove_if(splitted.begin(),
                                  splitted.end(),
                                  [](std::string s){return s.empty();}),
                   splitted.end());


    size_t summary_keys_size=0;
    for(auto& i: keys_stack) {
        summary_keys_size += i.size();
    }
    std::cout<<"\n\n\n>>>>>>>>keys_stack with alpha keys: "<<keys_stack.size()<<std::endl;
    std::cout<<std::endl<<"total len of keys: "<<summary_keys_size<<std::endl;

    /// building markov chain
    Markov<std::vector<std::string>, std::string> markov(&new_splitted);

    std::vector<MarkovChain<std::string>> markov_chains;
    std::vector<size_t> markov_chains_sizes;
    size_t model_size=0, min_model;

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


    size_t encoded_transition_tree_size = 0, encoded_transition_tree_len = 0;
    size_t sum1=0, sum2=0;
    for(auto& i: sorted_keys_tree) {
        encoded_transition_tree_size += huff_tree_str[i.first].size();
        encoded_transition_tree_len += i.first.size();
        for(auto& j: i.second) {
                encoded_transition_tree_size += huff_tree_str[j].size();
                encoded_transition_tree_len += j.size();
        }
    }

    std::cout<<"\nencoded_transition_tree_size = "<<encoded_transition_tree_size<<std::endl;
    ///sum1 - суммарный размер ключей

    /// encoding of text with transitions codes
    std::vector<size_t> encoded;
    size_t code, max_code=0;
    Counts<size_t> stat;
    HuffmanEncoder<size_t> huff;

    size_t total=0;

    for(auto it=splitted.begin(); it!=splitted.end()-markov_order; it++) {
        key1 = *it;
        key2 = *(it+markov_order);

        code = std::find(sorted_keys_tree[key1].begin(), sorted_keys_tree[key1].end(), key2) - sorted_keys_tree[key1].begin() + 1;
        encoded.push_back(code);
        max_code = std::max(code, max_code);
    }


    std::cout<<"\n\n\n>>>>>>>>encoded.size(): "<<encoded.size()<<std::endl;
    std::cout<<"\n\nencoded: ";
    for(auto& i: encoded) {
        std::cout<<" "<<i;
        stat[i]+=1;
    }

    huff.InitFrequencies(stat);
    huff.Encode();
    total = 0;
    for(auto& i: huff.outCodes) {
        total += i.second.size()*stat[i.first];
    }
    std::cout<<"\nencoded huffman: "<<total/8<<std::endl;

    auto huff_tree = huff.outCodes;
    size_t encoded_text_size = 0;
    for(auto& i: encoded) {
        encoded_text_size += huff_tree[i].size();
    }

    stat.clear();



    std::vector<size_t> bwted(encoded.begin(), encoded.end());
    auto bwt_key = townsend::algorithm::bwtEncode(bwted.begin(), bwted.end());

    std::cout<<"\n\n\n>>>>>>>>bwted.size(): "<<bwted.size()<<std::endl;
    std::cout<<"\nBWT(encoded):";
    for(auto& i: bwted) {
        std::cout<<" "<<i;
        stat[i] += 1;
    }

    huff.InitFrequencies(stat);
    huff.Encode();
    total = 0;
    for(auto& i: huff.outCodes) {
        total += i.second.size()*stat[i.first];
    }
    std::cout<<"\nBWT(encoded) huffman: "<<total/8<<std::endl;
    stat.clear();


    size_t max_elem = *std::max_element(bwted.begin(), bwted.end());
    auto rled = rlencode_mod12(bwted, max_elem+1, max_elem+2);
    std::cout<<"\n\n\n>>>>>>>>rled.size(): "<<rled.size()<<std::endl;
    std::cout<<"\nRLE(bwted): ";
    for(auto& i: rled) {
        std::cout<<" "<<i;
        stat[i]+=1;
    }

    huff.InitFrequencies(stat);
    huff.Encode();
    total = 0;
    for(auto& i: huff.outCodes) {
        total += i.second.size()*stat[i.first];
    }
    std::cout<<"\nRLE(bwted) huffman: "<<total/8<<std::endl;
    stat.clear();

    compressed_block.keys_count = keys_stack.size();

    std::vector<std::string> ordered_keys;
    ordered_keys.reserve(keys.size());

    for(auto& i: keys_stack) {
        ordered_keys.push_back(i);
    }

    std::sort(ordered_keys.begin(), ordered_keys.end());

    std::string keys_string = boost::algorithm::join(ordered_keys, "");

    std::string compressed_keys_string = Gzip::compress(keys_string);

    std::string non_compressed_text = "";
    std::string compressed_non_compressed_text = "";
    if(non_compressed_seqs.size()) {
        non_compressed_text = boost::algorithm::join(non_compressed_seqs, "");
        compressed_non_compressed_text = Gzip::compress(non_compressed_text);
    }

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


    std::vector<size_t> stat_vec(max_code+1);
    size_t ii=0;
    for(auto it=stat_vec.begin(); it!=stat_vec.end(); it++) {
        (*it) = ii;
        ii++;
    }

    std::cout<<"Max code: "<<max_code<<std::endl;

    auto mtfed = mtf(stat_vec, encoded);

    Counts<size_t> mtf_counts;

    for(auto& i: mtfed) {
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

    std::cout<<"mtfed.size(): "<<mtfed.size()<<std::endl;
    std::cout<<"sum5: "<<sum5/8<<std::endl;
    std::cout<<"sum6: "<<sum6/8<<std::endl;
    std::cout<<"sum5+6: "<<(sum5+sum6)/8<<std::endl;

    std::cout<<"\nBzip2.compress(text).size(): "<<Bzip2::compress(text).size();
    std::cout<<"\nGzip.compress(text).size(): "<<Gzip::compress(text).size()<<std::endl;


    std::cout<<"\n\n\n>>>>>>>>mtfed.size(): "<<mtfed.size()<<std::endl;
    std::cout<<"\nMTF(encoded): ";
    for(auto& i: mtfed) {
        std::cout<<" "<<i;
        stat[i]+=1;
    }

    huff.InitFrequencies(stat);
    huff.Encode();
    total = 0;
    for(auto& i: huff.outCodes) {
        total += i.second.size()*stat[i.first];
    }
    std::cout<<"\nMTF(encoded) huffman: "<<total/8<<std::endl;
    stat.clear();

    max_elem = *std::max_element(mtfed.begin(), mtfed.end());
    std::vector<size_t> rled_mtfed = rlencode_mod12(mtfed, max_elem+1, max_elem+2);
    std::cout<<"\n\n\n>>>>>>>>rled_mtfed.size(): "<<rled_mtfed.size()<<std::endl;
    std::cout<<"\nRLE(mtfed): ";
    for(auto& i: rled_mtfed) {
        std::cout<<" "<<i;
        stat[i]+=1;
    }

    huff.InitFrequencies(stat);
    huff.Encode();
    total = 0;
    for(auto& i: huff.outCodes) {
        total += i.second.size()*stat[i.first];
    }
    std::cout<<"\nRLE(mtfed) huffman: "<<total/8<<std::endl;
    stat.clear();

    std::vector<size_t> mtfed_bwt = mtf(stat_vec, bwted);
    std::cout<<"\n\n\n>>>>>>>>mtfed_bwt.size(): "<<mtfed_bwt.size()<<std::endl;
    std::cout<<"\nMTF(bwted): ";
    for(auto& i: mtfed_bwt) {
        std::cout<<" "<<i;
        stat[i]+=1;
    }

    huff.InitFrequencies(stat);
    huff.Encode();
    total = 0;
    for(auto& i: huff.outCodes) {
        total += i.second.size()*stat[i.first];
    }
    std::cout<<"\nMTF(bwted) huffman: "<<total/8<<std::endl;
    stat.clear();

    max_elem = *std::max_element(mtfed_bwt.begin(), mtfed_bwt.end());
    std::vector<size_t> rled_mtfed_bwt = rlencode_mod12(mtfed_bwt, max_elem+1, max_elem+2);
    std::cout<<"\n\n\n>>>>>>>>rled_mtfed_bwt.size(): "<<rled_mtfed_bwt.size()<<std::endl;
    std::cout<<"\nRLE(mtfed_bwt): ";
    for(auto& i: rled_mtfed_bwt) {
        std::cout<<" "<<i;
        stat[i]+=1;
    }

    huff.InitFrequencies(stat);
    huff.Encode();
    total = 0;
    for(auto& i: huff.outCodes) {
        total += i.second.size()*stat[i.first];
    }
    std::cout<<"\nRLE(mtfed_bwt) huffman: "<<total/8<<std::endl;
    stat.clear();

    std::vector<size_t> bwted_mtfed_bwt(mtfed_bwt.begin(), mtfed_bwt.end());

    auto bwted_mtfed_bwt_key = townsend::algorithm::bwtEncode(bwted_mtfed_bwt.begin(), bwted_mtfed_bwt.end());

    std::cout<<"\n\n\n>>>>>>>>bwted_mtfed_bwt.size(): "<<bwted_mtfed_bwt.size()<<std::endl;
    std::cout<<"\nBWT(mtfed_bwt): ";
    for(auto& i: bwted_mtfed_bwt) {
        std::cout<<" "<<i;
        stat[i]+=1;
    }

    huff.InitFrequencies(stat);
    huff.Encode();
    total = 0;
    for(auto& i: huff.outCodes) {
        total += i.second.size()*stat[i.first];
    }
    std::cout<<"\nBWT(mtfed_bwt) huffman: "<<total/8<<std::endl;
    stat.clear();


    max_elem = *std::max_element(bwted_mtfed_bwt.begin(), bwted_mtfed_bwt.end());
    std::vector<size_t> rled_bwted_mtfed_bwt = rlencode_mod12(bwted_mtfed_bwt, max_elem+1, max_elem+2);
    std::cout<<"\n\n\n>>>>>>>>rled_bwted_mtfed_bwt.size(): "<<rled_bwted_mtfed_bwt.size()<<std::endl;
    std::cout<<"\nRLE(bwted_mtfed_bwt): ";
    for(auto& i: rled_bwted_mtfed_bwt) {
        std::cout<<" "<<i;
        stat[i]+=1;
    }

    huff.InitFrequencies(stat);
    huff.Encode();
    total = 0;
    for(auto& i: huff.outCodes) {
        total += i.second.size()*stat[i.first];
    }
    std::cout<<"\nRLE(bwted_mtfed_bwt) huffman: "<<total/8<<std::endl;
    stat.clear();

    std::set<size_t> set1(bwted.begin(), bwted.end());
    std::vector<size_t> alpha(set1.begin(), set1.end());
//    std::vector<size_t> dc_bwt = mtf(alpha, bwted);
    std::vector<size_t> dc_bwt = dc_encode(bwted, alpha);

    std::cout<<"\n\n\n>>>>>>>>dc_bwt.size(): "<<dc_bwt.size()<<std::endl;
    std::cout<<">>>>>>>>bwted.size(): "<<bwted.size()<<std::endl;
    std::cout<<"\nDC(bwted): ";
    for(auto& i: dc_bwt) {
        std::cout<<" "<<i;
        stat[i]+=1;
    }

    huff.InitFrequencies(stat);
    huff.Encode();
    total = 0;
    for(auto& i: huff.outCodes) {
        total += i.second.size()*stat[i.first];
    }
    std::cout<<"\nDC(bwted) huffman: "<<total/8<<std::endl;
    stat.clear();


    set1.clear();
    set1.insert(encoded.begin(), encoded.end());
    alpha.clear();
    alpha.insert(alpha.end(), set1.begin(), set1.end());
//    dc_bwt = mtf(alpha, encoded);
    dc_bwt = dc_encode(encoded, alpha);
    std::cout<<"\n\n\n>>>>>>>>dc_encoded.size(): "<<dc_bwt.size()<<std::endl;
    std::cout<<">>>>>>>>encoded.size(): "<<encoded.size()<<std::endl;
    std::cout<<"\nDC(encoded): ";
    for(auto& i: dc_bwt) {
        std::cout<<" "<<i;
        stat[i]+=1;
    }

    huff.InitFrequencies(stat);
    huff.Encode();
    total = 0;
    for(auto& i: huff.outCodes) {
        total += i.second.size()*stat[i.first];
    }
    std::cout<<"\nDC(encoded) huffman: "<<total/8<<std::endl;
    stat.clear();



    set1.clear();
    set1.insert(rled.begin(), rled.end());
    alpha.clear();
    alpha.insert(alpha.end(), set1.begin(), set1.end());
//    dc_encode = mtf(alpha, encoded);
    auto dc_encoded = dc_encode(rled, alpha);
    std::cout<<"\n\n\n>>>>>>>>dc_rled.size(): "<<dc_encoded.size()<<std::endl;
    std::cout<<">>>>>>>>rled.size(): "<<rled.size()<<std::endl;
    std::cout<<"\nDC(rled): ";
    for(auto& i: dc_encoded) {
        std::cout<<" "<<i;
        stat[i]+=1;
    }

    huff.InitFrequencies(stat);
    huff.Encode();
    total = 0;
    for(auto& i: huff.outCodes) {
        total += i.second.size()*stat[i.first];
    }
    std::cout<<"\nDC(rled) huffman: "<<total/8<<std::endl;
    stat.clear();



    set1.clear();
    set1.insert(mtfed_bwt.begin(), mtfed_bwt.end());
    alpha.clear();
    alpha.insert(alpha.end(), set1.begin(), set1.end());
//    dc_encode = mtf(alpha, encoded);
    auto dc_mtfed_bwt = dc_encode(mtfed_bwt, alpha);
    std::cout<<"\n\n\n>>>>>>>>dc_rled.size(): "<<dc_mtfed_bwt.size()<<std::endl;
    std::cout<<">>>>>>>>mtfed_bwt.size(): "<<mtfed_bwt.size()<<std::endl;
    std::cout<<"\nDC(mtfed_bwt): ";
    for(auto& i: dc_encoded) {
        std::cout<<" "<<i;
        stat[i]+=1;
    }

    huff.InitFrequencies(stat);
    huff.Encode();
    total = 0;
    for(auto& i: huff.outCodes) {
        total += i.second.size()*stat[i.first];
    }
    std::cout<<"\nDC(mtfed_bwt) huffman: "<<total/8<<std::endl;
    stat.clear();


    return res;
}

std::string decompress(std::string text) {
    return "";
}
