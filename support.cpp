#include <string>
#include <vector>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/dynamic_bitset.hpp>
#include <chrono>
#include <iostream>
#include <algorithm>
#include <unordered_set>
#include <functional>
#include <cmath>

#include "support.hpp"

//#include <easy/profiler.h>
std::string repeat_string(const std::string &word, int times ) {
   std::string result;
   result.reserve(times*word.length()); // avoid repeated reallocation
   for ( int a = 0; a < times; a++ )
      result += word;
   return result;
}

size_t countSubstring(const std::string& str, const std::string& sub) {
//    EASY_FUNCTION(0xfff080aa);
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
    std::vector<std::string> res = {text, };
    std::vector<std::string> new_res;
    std::vector<std::string> strs;
    std::unordered_set<std::string> tokens_set(tokens.begin(), tokens.end());

//    res.push_back(text);
    for(auto& token: tokens) {
        new_res.reserve(res.size());
        for(std::string& str: res) {
//            boost::split(strs, str, boost::is_any_of(token))
//            if(std::find(tokens.begin(), tokens.end(), str)==tokens.end()) {
            if(!tokens_set.count(str)) {
                strs = split(str, token);
                for(auto& j: strs) {
                    new_res.push_back(j);
                    new_res.push_back(token);
                }
                new_res.pop_back();
            } else {
                new_res.push_back(str);
            }
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
	    if(!token.empty()) {
            res.push_back(token);
	    }
	    // std::cout << token << std::endl;
	    pos0 = pos+l;
	    //s.erase(0, pos + l);
	}
	// std::cout << s << std::endl;
	res.push_back(s.substr(pos0, s.length()));

	return res;
}


bool is_keys_intersect(const std::string& key11, const std::string& key22) {
    std::string key1, key2;
    if(key11.size()>key22.size()) {
//        auto key3 = key2;
//        key2 = key1;
//        key1 = key3;
        key2 = key11;
        key1 = key22;
    } else {
        key2 = key22;
        key1 = key11;
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


std::vector<std::string> get_keys_intersections(const std::string& key11, const std::string& key22) {
//    EASY_FUNCTION();
    std::string key1, key2;
    if(key11.size()>key22.size()) {
//        auto key3 = key2;
//        key2 = key1;
//        key1 = key3;
        key2 = key11;
        key1 = key22;
    } else {
        key2 = key22;
        key1 = key11;
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

//std::vector<std::string> get_keys_intersections_wh(const std::string& key11, const std::string& key22) {
std::vector<std::string> get_keys_intersections_wh(std::string key1, std::string key2) {
//    auto start = std::chrono::system_clock::now();
//    EASY_FUNCTION();
//    std::string key1, key2;
//    if(key11.size()>key22.size()) {
    if(key1.size()>key2.size()) {
        auto key3 = key2;
        key2 = key1;
        key1 = key3;
//        key2 = key11;
//        key1 = key22;
    }
//    else {
//        key2 = key22;
//        key1 = key11;
//    }


    std::vector<std::string> intersections;
    std::hash<std::string> str_hash;

    bool res = (key2.find(key1)!=std::string::npos);
    if(res) {
        intersections.push_back(key2);
    }
    size_t l1=key1.size(), l2=key2.size();

//    size_t l = key1.size();
    std::string sub, sub2;
//    sub2.reserve(key2);
//    #pragma omp parallel for
    for(size_t i=1; i<l1; i++) {
//        std::cout<<"1 ";
        //sub = key1.substr(0, i);
        sub += key1[i-1];
        sub2 = key2[l2-i] + sub2;
        //if(boost::algorithm::ends_with(key2, sub)) {
//        if(str_hash(key2.substr(l2-i, i))==str_hash(sub) && key2.substr(l2-i, i)==sub) {
//        if(str_hash(sub2)==str_hash(sub) && sub2==sub) {
        if(str_hash(sub2)==str_hash(sub)) {
            intersections.push_back(key2+key1.substr(i, l1));
        }
    }

    sub = "";
    sub2 = "";
//    #pragma omp parallel for
    for(size_t i=1; i<l1; i++) {
//        std::cout<<"2 ";
        //sub = key1.substr(l1-i, i);
        sub = key1[l1-i] + sub;
        sub2 += key2[i-1];
        //if(boost::algorithm::starts_with(key2, sub)) {
//        if(str_hash(key2.substr(0, i))==str_hash(sub) && key2.substr(0, i)==sub) {
//        if(str_hash(sub2)==str_hash(sub) && sub2==sub) {
        if(str_hash(sub2)==str_hash(sub)) {
            intersections.push_back(key1.substr(0, l1-i)+key2);
        }
    }

//    auto finish = std::chrono::system_clock::now();
//    auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(finish - start);
//
//    std::cout<<">>>get_keys_intersections_wh time: "<< elapsed.count() << std::endl;

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


//std::vector<size_t> onetwo_encoder(size_t num, size_t z1, size_t z2) {
//	size_t bin[2] = {z1, z2};
//	std::vector<size_t> res;
//	res.reserve(ceil(log2(num)));
//
//	boost::dynamic_bitset<> bset(ceil(log2(num)), num);
//	for(auto i=0; i<bset.size(); i++) {
//		res.push_back(bin[bset[i]]);
//	}
//
//	return res;
//}


std::vector<size_t> onetwo_encoder(size_t num, size_t z1, size_t z2) {
	std::vector<size_t> res;

	while(num>0) {
		if(num & 1) {
			res.push_back(z1);
			num -= 1;
		} else {
			res.push_back(z2);
			num -= 2;
		}
		num >>= 1;
	}

	return res;
}


std::vector<size_t> dc_encode(std::vector<size_t> text, std::vector<size_t> alpha) {

	const size_t al = alpha.size();
	const size_t tl = text.size();

	std::vector<size_t> res, vec(tl+1, 0);

	//vec.reserve(text);

	///alpha
	size_t dist, index, zero_count;
	for(auto i=0; i<al; i++) {
		auto it = std::find(text.begin(), text.end(), alpha[i]);
		index = it - text.begin();
		dist = index;

		//if(dist!=0) {
			zero_count = std::count_if(vec.begin(), vec.begin()+dist, [](auto a){return a==0;});
			res.push_back(zero_count+1);
//			std::cout<<alpha[i]<<"\t"<<dist<<"\t"<<zero_count<<std::endl;
		//}
		vec[index] = alpha[i];
	}

    ///kast
	for(auto i=0; i<tl; i++) {
		//auto start =
		auto it = std::find(std::min(text.begin()+i+1, text.end()), text.end(), vec[i]);
		if(it!=text.end()) {
		index = it - text.begin();
		dist = index - i;

		//std::cout<<"dist: "<<dist<<std::endl;

		if(dist!=1) {
			zero_count = std::count_if(vec.begin()+i, vec.begin()+index, [](auto a){return a==0;});
			res.push_back(zero_count+1);
		}

		vec[index] = vec[i];

		//if(dist==1) {
			//std::cout<<"dist==1"<<std::endl;
			//continue;
		//}
		} else {
            res.push_back(0);
		}

	}


	return res;
}
