#include <string>
#include <vector>
#include <algorithm>
#include "global.hpp"
#include "lcp.hpp"


std::vector<std::pair<std::string, size_t>> get_suffixes(const std::string &s) {
    std::vector<std::pair<std::string, size_t>> res;
    const size_t l = s.length();
    for(size_t i=0; i<l; i++) {
        res.push_back(std::make_pair(s.substr(i, l), i));
    }
    return res;
}

std::vector<std::pair<std::string, size_t>> get_prefixes(const std::string &s) {
    std::vector<std::pair<std::string, size_t>> res;
    const size_t l = s.length();
    for(size_t i=0; i<l; i++) {
        res.push_back(std::make_pair(s.substr(0, i), i));
    }
    return res;
}

std::vector<size_t> get_lcp(const std::vector<std::pair<std::string, size_t>> &sorted_suffixes) {
    std::map<size_t, std::pair<std::string, size_t>> suffs;
    size_t c = 0;
    for(auto& i: sorted_suffixes) {
        suffs[i.second] = std::make_pair(i.first, c++);
    }

    size_t l = suffs.size();
    std::vector<size_t> lcp(l, 0);
    std::string s1, s2;
    size_t d = 0, d1, j, minl;
    for(size_t i=0; i<l; i++) {
        s1 = suffs[i].first;
        j = suffs[i].second + 1;
        if(j>=l) {
            d = 0;
            continue;
        }

        s2 = sorted_suffixes[j].first;
        minl = std::min(s1.length(), s2.length());
        d = std::min(d, minl);
        d1 = std::max(d, (size_t)0);

        if(s1[d1]!=s2[d1]) {
            if(d>=0) {
                d -= 1;
            }
        } else {
            while(d+1<=minl && s1[d+1]==s2[d+1]) {
                d += 1;
            }
        }
        lcp[j-1] = d + 1;
    }
    return lcp;
}


std::vector<std::string> get_common_prefixes(std::string text) {

    std::vector<std::pair<std::string, size_t>> sorted_suffixes = get_suffixes(text);
    std::sort(sorted_suffixes.begin(), sorted_suffixes.end(), [](auto a, auto b){return a.first>b.first;});

    std::vector<size_t> lcps = get_lcp(sorted_suffixes);
    std::vector<std::string> res;
    for(size_t i=0; i<lcps.size(); i++) {
        if(lcps[i]>0) {
            res.push_back(sorted_suffixes[i].first.substr(0, lcps[i]));
        }
    }

    return res;
}
