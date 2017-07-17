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
#include "ahocorasik.hpp"
//#include <easy/profiler.h>

Counts<std::string> get_keys_naive(const std::string& text, size_t factor, size_t min_len, size_t max_len) {

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
//                keys_set.emplace(sub);
                keys_set.insert(sub);
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


Counts<std::string> get_keys_naive_mod(const std::string& text, size_t factor, size_t min_len, size_t max_len) {
    auto fun_start = std::chrono::system_clock::now();


    size_t pos = 0;
    size_t c = min_len;
    const size_t l = text.length();
    Counts<std::string> keys;
    std::unordered_set<std::string> keys_set, suffixes_set;

    std::string sub;
    size_t cnt;

//    while(pos<l) {
    for(pos=0; pos<l; pos++) {
        c = min_len;
        sub = text.substr(pos, c);
        while(pos+c<=l && c<=max_len && keys_set.count(sub)) {
            c += 1;
//            sub += text[pos+c-1];
            sub = text.substr(pos, c);
        }

        keys_set.insert(sub);

//        while(pos+c<=l && c<=max_len) {
//            c += 1;
//            sub = text.substr(pos, c);
////            sub += text[pos+c-1];
//            keys_set.insert(sub);
//        }
    }

//    keys = multicount(text, keys_set);
    keys = multicount_aho(text, keys_set);

    std::cout<<"keys before filter: "<<keys.size()<<std::endl;
    for(auto it=keys.begin(); it!=keys.end(); ) {
        if((*it).second<=factor || (*it).first.length()<min_len) {
            it = keys.erase(it);
        } else {
            ++it;
        }
    }
    std::cout<<"keys after filter: "<<keys.size()<<std::endl;

    auto fun_finish = std::chrono::system_clock::now();
    auto fun_elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(fun_finish - fun_start);
    std::cout<<"get_keys_naive_mod: "<< fun_elapsed.count() << std::endl;

    return keys;
}


//Counts<std::string> get_keys_by_lcp(const std::string& text, size_t factor, size_t min_len, size_t max_len, size_t block_size) {
////    EASY_FUNCTION();
//    auto start = std::chrono::system_clock::now();
//    auto fun_start = std::chrono::system_clock::now();
//    auto finish = std::chrono::system_clock::now();
//    auto fun_finish = std::chrono::system_clock::now();
//    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(finish - start);
//    auto fun_elapsed = elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(finish - start);
//
//    size_t L = text.length(), l, cnt;
////    size_t step = (size_t)block_size;
//    //size_t step = (size_t)block_size-min_len;
////    size_t step = (size_t)block_size/2-min_len;
//    size_t step = (size_t)1;
//    std::vector<std::string> blocks;
//    std::set<std::string> blocks_set;
//
//    start = std::chrono::system_clock::now();
//    for(size_t i=0; i<L; i+=step) {
//        blocks.push_back(text.substr(i, block_size));
//    }
//    finish = std::chrono::system_clock::now();
//    elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(finish - start);
//    std::cout<<"Split by blocks: "<< elapsed.count() << std::endl;
//
//    std::vector<std::pair<std::string, size_t>> suffixes, suffs;
//
//    start = std::chrono::system_clock::now();
//    for(size_t i=0; i<blocks.size(); i++) {
//        suffs = get_suffixes(blocks[i]);
//        for(auto& j: suffs) {
//            suffixes.push_back(std::make_pair(j.first, block_size*i+j.second));
//        }
//    }
//    finish = std::chrono::system_clock::now();
//    elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(finish - start);
//    std::cout<<"Getting suffixes: "<< elapsed.count() << std::endl;
//
//    start = std::chrono::system_clock::now();
//    std::sort(suffixes.begin(), suffixes.end(), [](auto a, auto b){return a.first>b.first;});
//    finish = std::chrono::system_clock::now();
//    elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(finish - start);
//    std::cout<<"Sorting suffixes: "<< elapsed.count() << std::endl;
//
//    start = std::chrono::system_clock::now();
//    std::vector<size_t> lcps = get_lcp_naive(suffixes);
//    finish = std::chrono::system_clock::now();
//    elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(finish - start);
//    std::cout<<"get_lcp: "<< elapsed.count() << std::endl;
//
//
//    std::vector<std::string> common_prefixes;
//
//    start = std::chrono::system_clock::now();
//    for(size_t i=0; i<lcps.size(); i++) {
//        if(lcps[i]>0) {
//            common_prefixes.push_back(suffixes[i].first.substr(0, lcps[i]));
//        }
//    }
//    finish = std::chrono::system_clock::now();
//    elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(finish - start);
//    std::cout<<"getting common prefixes: "<< elapsed.count() << std::endl;
//
//
//    std::unordered_set<std::string> keys_set;
//    Counts<std::string> keys;
//    std::string sub;
//
//    //std::vector<size_t> prekmp = prefix_function(text);
//
//    start = std::chrono::system_clock::now();
//    for(auto& prefix: common_prefixes) {
//        l = prefix.length();
//        size_t lim = std::min(l, max_len);
//        //#pragma omp parallel for
//        for(size_t i=min_len; i<=lim; i++) {
//            sub = prefix.substr(0, i);
//
//            //keys[sub] = 0;
////            keys_set.emplace(sub);
//            keys_set.insert(sub);
////            if(!keys_set.count(sub)) {
////                keys_set.emplace(sub);
////                cnt = countSubstring(text, sub);
////                //std::cout<<"Sub: "<<sub<<"\t"<<cnt<<std::endl;
////                //cnt = KMP_count(text, sub);
////                //cnt = preKMP_count(prekmp, text, sub);
////                if(cnt>factor) {
////                    keys[sub] = cnt;
////
////                } else {
////                    break;
////                }
////            }
//        }
//    }
//    finish = std::chrono::system_clock::now();
//    elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(finish - start);
//    std::cout<<"zeroing keys: "<< elapsed.count() << std::endl;
//
//    start = std::chrono::system_clock::now();
////    for(auto& key: keys_set) {
////        sub = key;
//////        if(key.second==0) {
////            cnt = countSubstring(text, sub);
//////            if(cnt>factor) {
////                keys[sub] = cnt;
//////            } else {
//////                break;
//////            }
//////        }
////    }
//    //keys = multicount(text, keys_set.begin(), keys_set.end());
//    keys = multicount(text, keys_set);
//    finish = std::chrono::system_clock::now();
//    elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(finish - start);
//    std::cout<<"counting keys: "<< elapsed.count() << std::endl;
//
//    std::cout<<"Size before filtering: "<<keys.size()<<std::endl;
//    start = std::chrono::system_clock::now();
//    //for(auto it=keys.begin(); it!=keys.end(); it++) {
//    auto it=keys.begin();
//    while(it!=keys.end()) {
//        if((*it).second<=factor || (*it).first.length()<min_len || (*it).first.length()>max_len) {
//            //keys.erase(it++);
//            it = keys.erase(it);
//        } else {
//            ++it;
//        }
//    }
//    finish = std::chrono::system_clock::now();
//    elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(finish - start);
//    std::cout<<"Size after filtering: "<<keys.size()<<std::endl;
//    std::cout<<"filtering keys: "<< elapsed.count() << std::endl;
//
//    fun_finish = std::chrono::system_clock::now();
//    fun_elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(fun_finish - fun_start);
//    std::cout<<"get_keys_by_lcp: "<< fun_elapsed.count() << std::endl;
//
//    return keys;
//}












Counts<std::string> get_keys_by_lcp_old(const std::string& text, size_t factor, size_t min_len, size_t max_len, size_t block_size) {
//    EASY_FUNCTION();
    auto start = std::chrono::system_clock::now();
    auto fun_start = std::chrono::system_clock::now();
    auto finish = std::chrono::system_clock::now();
    auto fun_finish = std::chrono::system_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(finish - start);
    auto fun_elapsed = elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(finish - start);

    size_t L = text.length(), l, cnt;
//    size_t step = (size_t)block_size;
    //size_t step = (size_t)block_size-min_len;
//    size_t step = (size_t)block_size/2-min_len;
    size_t step = (size_t)1;
    std::vector<std::string> blocks;
    std::set<std::string> blocks_set;

    start = std::chrono::system_clock::now();
    for(size_t i=0; i<L; i+=step) {
        blocks_set.insert(text.substr(i, block_size));
    }
    finish = std::chrono::system_clock::now();
    elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(finish - start);
    std::cout<<"Split by blocks: "<< elapsed.count() << std::endl;

//    std::vector<std::pair<std::string, size_t>> suffixes, suffs;
    std::vector<std::string> suffs;

    start = std::chrono::system_clock::now();
    for(size_t i=0; i<blocks.size(); i++) {
        suffs = get_prefixes_str(blocks[i]);
        blocks_set.insert(suffs.begin(), suffs.end());

//        suffs = get_suffixes_str(blocks[i]);
//        blocks_set.insert(suffs.begin(), suffs.end());
    }

    finish = std::chrono::system_clock::now();
    elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(finish - start);
    std::cout<<"Getting suffixes: "<< elapsed.count() << std::endl;

    std::vector<std::string> suffixes(blocks_set.begin(), blocks_set.end());


    start = std::chrono::system_clock::now();

    std::vector<size_t> lcps = get_lcp_naive(suffixes);

    finish = std::chrono::system_clock::now();
    elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(finish - start);
    std::cout<<"get_lcp: "<< elapsed.count() << std::endl;


    std::vector<std::string> common_prefixes;
    std::unordered_set<std::string> keys_set;

    start = std::chrono::system_clock::now();
    for(size_t i=0; i<lcps.size(); i++) {
//        keys_set.insert(suffixes[i]);
        for(size_t j=1; j<=lcps[i]; j++) {
            keys_set.insert(suffixes[i].substr(0, j));
        }

    }
    finish = std::chrono::system_clock::now();
    elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(finish - start);
    std::cout<<"getting common prefixes: "<< elapsed.count() << std::endl;


    start = std::chrono::system_clock::now();

    std::cout<<"keys_set.size(): "<< keys_set.size() << std::endl;
//    Counts<std::string> keys = multicount(text, keys_set);
    Counts<std::string> keys = multicount_aho(text, keys_set);

    finish = std::chrono::system_clock::now();
    elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(finish - start);
    std::cout<<"counting keys: "<< elapsed.count() << std::endl;


    std::cout<<"Size before filtering: "<<keys.size()<<std::endl;
    start = std::chrono::system_clock::now();

    for(auto it=keys.begin(); it!=keys.end(); ) {
//        if((*it).second<=factor || (*it).first.length()<min_len || (*it).first.length()>max_len) {
        if((*it).second<=factor || (*it).first.length()<min_len) {
            //keys.erase(it++);
            it = keys.erase(it);
        } else {
            ++it;
        }
    }

    finish = std::chrono::system_clock::now();
    elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(finish - start);
    std::cout<<"Size after filtering: "<<keys.size()<<std::endl;
    std::cout<<"filtering keys: "<< elapsed.count() << std::endl;

    fun_finish = std::chrono::system_clock::now();
    fun_elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(fun_finish - fun_start);
    std::cout<<"get_keys_by_lcp: "<< fun_elapsed.count() << std::endl;

    return keys;
}




Counts<std::string> get_keys_by_lcp(const std::string& text, size_t factor, size_t min_len, size_t max_len, size_t block_size) {
    std::cout<<"get_keys_by_lcp started"<<std::endl;

    auto start = std::chrono::system_clock::now();
    auto finish = std::chrono::system_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(finish - start);

//    auto start = std::chrono::system_clock::now();
    std::array<std::vector<size_t>, 256> start_positions;
    start_positions.fill(std::vector<size_t>());
//    std::array<std::vector<std::pair<std::string, size_t>>, 256> bucks;
    std::string s, key;
    Counts<std::string> keys;

//    std::cout<<"1"<<std::endl;
//    std::cout<<"Started filling start_positions"<<std::endl;
    start = std::chrono::system_clock::now();
    for(auto i=0; i<text.size(); i++) {
        //std::cout<<(int)text[i]<<std::endl;
        start_positions[(unsigned char)text[i]].push_back(i);
    }
    finish = std::chrono::system_clock::now();
    elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(finish - start);
//    std::cout<<"filling start_positions time: "<< elapsed.count() << std::endl;


//    std::cout<<"2"<<std::endl;
    for(const auto& positions: start_positions) {
        if(positions.size()<=1) {
            continue;
        }
//        std::cout<<"3"<<std::endl;
        std::vector<std::pair<std::string, size_t>> buck;
        buck.reserve(positions.size());
//        std::cout<<"3.1"<<std::endl;
        for(const auto& pos: positions) {
            buck.push_back(std::make_pair(text.substr(pos, block_size), pos));
        }

//        std::cout<<"Started sorting buck"<<std::endl;
        start = std::chrono::system_clock::now();
//        std::cout<<"3.5"<<std::endl;
        std::sort(buck.begin(), buck.end(), [](const auto& a, const auto& b){return a.first<b.first;});
        finish = std::chrono::system_clock::now();
        elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(finish - start);
//        std::cout<<"sorting buck time: "<< elapsed.count() << std::endl;
//        std::cout<<"3.6"<<std::endl;
        std::vector<size_t> cps = get_lcp_naive(buck);
//        std::cout<<"3.7"<<std::endl;
        std::vector<size_t> used(buck.size(), 1);

//        std::cout<<"\n>>>>buck:"<<std::endl;
//        for(auto& i: buck) {
//            std::cout<<i.first<<std::endl;
//        }
//        std::cout<<">>>>cps:"<<std::endl;
//        for(auto& i: cps) {
//            std::cout<<i<<" ";
//        }
//        std::cout<<std::endl;


//        std::cout<<"4"<<std::endl;
        auto peaks = find_peaks(cps.begin(), cps.end());
//        std::cout<<"peaks.size(): "<<peaks.size()<<std::endl;
//        std::cout<<">>>>peaks:"<<std::endl;
//        for(auto& i: peaks) {
//            std::cout<<i-cps.begin()<<" ";
//        }
//        std::cout<<std::endl;
        std::vector<size_t> p; //, p1, new_p;
        std::set<size_t> new_p;
        p.reserve(buck.size());
//        p1.reserve(buck.size());
//        new_p.reserve(buck.size());
//        for(auto j=0; j<buck.size(); j++) {
//            p.push_back(buck[j].second);
//        }
//        std::sort(p.begin(), p.end());

        for(const auto& peak: peaks) {
//            std::cout<<"5"<<std::endl;
//            std::cout<<"peak-cps.begin(): "<<peak-cps.begin()<<std::endl;
//            std::cout<<used.size()<<"\t"<<cps.size()<<std::endl;
//            std::cout<<used[peak-cps.begin()]<<std::endl;
//            std::cout<<cps[peak-cps.begin()]<<std::endl;
//            std::cout<<"Started peak processing"<<std::endl;
            start = std::chrono::system_clock::now();
            auto left_border = peak;
            auto right_border = peak;
            for(auto i=used[peak-cps.begin()]; i<=cps[peak-cps.begin()]; i++) {
//                std::cout<<"6"<<std::endl;
                left_border = peak;
                right_border = peak;

//                std::cout<<"Started borders adjusting"<<std::endl;
                auto start1 = std::chrono::system_clock::now();

                while(left_border!=cps.begin() && *(left_border-1)>=i) {
                    left_border--;
                }

                while(right_border!=cps.end()-1 && *(right_border+1)>=i) {
                    right_border++;
                }

                auto finish1 = std::chrono::system_clock::now();
                auto elapsed1 = std::chrono::duration_cast<std::chrono::milliseconds>(finish1 - start1);
//                std::cout<<"borders adjusting time: "<< elapsed1.count() << std::endl;
//                std::vector<size_t> p;
//                p.reserve(buck.size());

//                std::cout<<"Started filling and sorting p"<<std::endl;
                start1 = std::chrono::system_clock::now();
//                std::cout<<"7"<<std::endl;
                for(auto j=left_border-cps.begin(); j<(right_border-cps.begin())+2; j++) {
                    p.push_back(buck[j].second);
//                    new_p.push_back(buck[j].second);
//                    new_p.insert(buck[j].second);
                }
                std::sort(p.begin(), p.end());
//                std::copy(new_p.begin(), new_p.end(), p.begin());
//                std::cout<<"p.size(): "<<p.size()<<std::endl;
//                std::copy_if(p.begin(), p.end(), p1.begin(), [new_p](const auto& a){return ;})


//                std::vector<size_t>(new_p.begin(), new_p.end()).swap(p);
//                p.erase(std::remove_if(p.begin(), p.end(), [new_p](const auto& a){return !new_p.count(a);}), p.end());
                finish1 = std::chrono::system_clock::now();
                elapsed1 = std::chrono::duration_cast<std::chrono::milliseconds>(finish1 - start1);
//                std::cout<<"filling and sorting p time: "<< elapsed1.count() << std::endl;

//                std::cout<<"8"<<std::endl;
                auto streak = find_longest_streak(p.begin(), p.end(), i);
                key = buck[peak-cps.begin()].first.substr(0, i);
                keys[key] += streak;

//                std::cout<<"9"<<std::endl;
                for(auto j=left_border-cps.begin(); j<=(right_border-cps.begin()); j++) {
                    used[j] += 1;
                }

                p.clear();
//                p1.clear();
                new_p.clear();
            }
            finish = std::chrono::system_clock::now();
            elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(finish - start);
//            std::cout<<"peak processing time: "<< elapsed.count() << std::endl;
        }
//        used.clear();
//        peaks.clear();
//        cps.clear();
//        buck.clear();
    }

//    std::cout<<"10"<<std::endl;
    for(auto it=keys.begin(); it!=keys.end(); ) {
//        std::cout<<"11"<<std::endl;
        if((*it).second<=factor || (*it).first.length()<min_len) {
//            std::cout<<"12"<<std::endl;
            it = keys.erase(it);
        } else {
//            std::cout<<"13"<<std::endl;
            ++it;
        }
//        std::cout<<"14"<<std::endl;
    }
//    std::cout<<"15"<<std::endl;

    return keys;
//    std::cout<<"16"<<std::endl;
}



std::map<std::string, std::vector<size_t>> get_keys_positions_by_lcp(const std::string& text, size_t factor, size_t min_len, size_t max_len, size_t block_size) {
    std::cout<<"get_keys_positions_by_lcp started"<<std::endl;

    std::array<std::vector<size_t>, 256> start_positions;
    start_positions.fill(std::vector<size_t>());

    std::string s, key;

    std::map<std::string, std::vector<size_t>> keys;
    keys[""] = std::vector<size_t>(text.size(), 1);

    for(auto i=0; i<text.size(); i++) {

        start_positions[(unsigned char)text[i]].push_back(i);
    }

    for(const auto& positions: start_positions) {
        if(positions.size()<=1) {
            continue;
        }

        std::vector<std::pair<std::string, size_t>> buck;
        buck.reserve(positions.size());

        for(const auto& pos: positions) {
            buck.push_back(std::make_pair(text.substr(pos, block_size), pos));
        }


        std::sort(buck.begin(), buck.end(), [](const auto& a, const auto& b){return a.first<b.first;});

        std::vector<size_t> cps = get_lcp_naive(buck);

        std::vector<size_t> used(buck.size(), 1);


        auto peaks = find_peaks(cps.begin(), cps.end());

        std::vector<size_t> p; //, p1, new_p;
        std::set<size_t> new_p;
        p.reserve(buck.size());


        for(const auto& peak: peaks) {

            auto left_border = peak;
            auto right_border = peak;
            for(auto i=used[peak-cps.begin()]; i<=cps[peak-cps.begin()]; i++) {

                left_border = peak;
                right_border = peak;

                while(left_border!=cps.begin() && *(left_border-1)>=i) {
                    left_border--;
                }

                while(right_border!=cps.end()-1 && *(right_border+1)>=i) {
                    right_border++;
                }

                for(auto j=left_border-cps.begin(); j<(right_border-cps.begin())+2; j++) {
                    p.push_back(buck[j].second);
                }
                std::sort(p.begin(), p.end());

                key = buck[peak-cps.begin()].first.substr(0, i);
                keys[key] = find_longest_streak_values(p.begin(), p.end(), i);
//                auto tmp = find_longest_streak_vector(p.begin(), p.end(), i);
//                keys[key].reserve(tmp.size());
//                for(auto j: tmp) {
//                    keys[key].push_back(*j);
//                }
//                tmp.clear();

                for(const auto& j: keys[key]) {
                    keys[""][j] = i;
                }

                for(auto j=left_border-cps.begin(); j<=(right_border-cps.begin()); j++) {
                    used[j] += 1;
                }

                p.clear();
            }
        }
    }

    for(auto it=keys.begin(); it!=keys.end(); ) {
        if((*it).second.size()<=factor || (*it).first.length()<min_len) {
            it = keys.erase(it);
        } else {
            ++it;
        }
    }

    return keys;
}

