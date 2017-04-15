#include <string>
#include <vector>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <chrono>
#include <iostream>

#include "support.hpp"

#include <easy/profiler.h>

size_t countSubstring(const std::string& str, const std::string& sub) {
    EASY_FUNCTION(0xfff080aa);
    if (sub.length() == 0) return 0;
    size_t count = 0, l = sub.length();
    for (size_t offset = str.find(sub); offset != std::string::npos; offset = str.find(sub, offset + l))
    {
        ++count;
    }

//    size_t pos = 0;
//    while ((pos = str.find(sub, pos)) != std::string::npos) {
//          ++count;
//          pos += l;
//    }

    return count;
}

std::vector<std::string> split_string_by_token(const std::string& text, const std::string& token) {
    std::vector<std::string> res;
    std::vector<std::string> strs;

    strs = split(text, token);
    for(auto& j: strs) {
        res.push_back(j);
        res.push_back(token);
    }
    res.pop_back();

    return res;
}


std::vector<std::string> split_string_by_tokens(const std::string& text, const std::vector<std::string>& tokens) {
    std::vector<std::string> res;
    std::vector<std::string> new_res;
    std::vector<std::string> strs;

    res.push_back(text);
    for(auto& token: tokens) {
        for(std::string& str: res) {
//            boost::split(strs, str, boost::is_any_of(token))
            strs = split(str, token);
            for(auto& j: strs) {
                new_res.push_back(j);
                new_res.push_back(token);
            }
            new_res.pop_back();
        }
        res = new_res;
        new_res.clear();
    }
    return res;
}


std::vector<std::string> split(const std::string& s, const std::string& delimiter) {
	std::vector<std::string> res;
	size_t pos = 0, pos0 = 0;
	std::string token;
	size_t l = delimiter.length();
	while ((pos = s.find(delimiter, pos0)) != std::string::npos) {
	    token = s.substr(pos0, pos-pos0);
	    res.push_back(token);
	    // std::cout << token << std::endl;
	    pos0 = pos+l;
	    //s.erase(0, pos + l);
	}
	// std::cout << s << std::endl;
	res.push_back(s.substr(pos0, s.length()));

	return res;
}


bool is_keys_intersect(std::string key1, std::string key2) {
    if(key1.size()>key2.size()) {
        auto key3 = key2;
        key2 = key1;
        key1 = key3;
    }

    bool res = (key2.find(key1)!=std::string::npos);
    if(res) {
        return 1;
    }

    size_t l = key1.size();
    std::string sub;
    for(size_t i=1; i<l; i++) {
        sub = key1.substr(0, i);
        if(boost::algorithm::ends_with(key2, sub)) {
            return 1;
        }
    }

    for(size_t i=1; i<l; i++) {
        sub = key1.substr(l-i, i);
        if(boost::algorithm::starts_with(key2, sub)) {
            return 1;
        }
    }

    return 0;
}


std::vector<std::string> get_keys_intersections(std::string key1, std::string key2) {
    EASY_FUNCTION();
    if(key1.size()>key2.size()) {
        auto key3 = key2;
        key2 = key1;
        key1 = key3;
    }

    std::vector<std::string> intersections;

    bool res = (key2.find(key1)!=std::string::npos);
    if(res) {
        intersections.push_back(key2);
    }

    size_t l = key1.size();
    std::string sub;
//    #pragma omp parallel for
    for(size_t i=1; i<l; i++) {
        sub = key1.substr(0, i);
        if(boost::algorithm::ends_with(key2, sub)) {
            intersections.push_back(key2+key1.substr(i, l));
        }
    }
//    #pragma omp parallel for
    for(size_t i=1; i<l; i++) {
        sub = key1.substr(l-i, i);
        if(boost::algorithm::starts_with(key2, sub)) {
            intersections.push_back(key1.substr(0, l-i)+key2);
        }
    }

    return intersections;
}


std::vector<size_t> prefix_function(const std::string& s) {
	size_t n = (size_t)s.length();
	std::vector<size_t> pi(n);
	pi[0] = 0;

    size_t j;
	for(size_t i=1; i<n; ++i) {
		j = pi[i-1];
		while(j>0 && s[i]!=s[j])
			j = pi[j-1];
		if(s[i]==s[j]) ++j;
		pi[i] = j;
	}

//
//	for(size_t j=0, i=1; i<n; ++i) {
//		while(j>0 && s[i]!=s[j])
//			j = pi[j-1];
//		if(s[i]==s[j]) j++;
//		pi[i] = j;
//	}

	return pi;
}



size_t KMP_count(const std::string& S, const std::string& pattern, size_t start) {

    std::vector<size_t> prefix = prefix_function(pattern);

    size_t counter=0, l=pattern.length(), k=0;
    for(size_t i=start; i<S.length(); ++i) {
        while((k>0) && (pattern[k]!=S[i])) {
            k = prefix[k-1];
        }
        if(pattern[k]==S[i]) {
            k++;
        }
        if(k==l) {
            //return (i-l+1);
            counter++;
            k=0;
            //i += k-1;
        }
    }

    return counter;
}

size_t preKMP_count(const std::vector<size_t>& prefix, const std::string& S, const std::string& pattern, size_t start) {

    size_t counter=0, l=pattern.length(), k=0;
    for(size_t i=start; i<S.length(); ++i) {
        while((k>0) && (pattern[k]!=S[i])) {
            k = prefix[k-1];
        }
        if(pattern[k]==S[i]) {
            k++;
        }
        if(k==l) {
            //return (i-l+1);
            counter++;
            k=0;
            //i += k-1;
        }
    }

    return counter;
}

//Counts<std::string>


size_t fasthash(std::string s, size_t base) {
    size_t c=1, res=0;
    for(auto& i: s) {
        res += i*c;
//    for(auto i=s.length()-1; i!=0; i--) {
//        res += s[i]*c;
//        c *= base;
    }

    return res;
}



size_t divide_rounding_up(size_t dividend, size_t divisor) {
    return (dividend + divisor - 1) / divisor;
}

std::string dump_bits(const std::vector<bool>& bitvector) {
    std::string ret(divide_rounding_up(bitvector.size(), 8), 0);
    auto out = ret.begin();
    int shift = 0;

    for(bool bit: bitvector) {
        *out |= bit << shift;

        if (++shift == 8) {
            ++out;
            shift = 0;
        }
    }

    return ret;
}
