#ifndef SUPPORT_HPP_INCLUDED
#define SUPPORT_HPP_INCLUDED

#include <vector>
#include <string>
#include <algorithm>
#include <list>
#include <cmath>

#include <boost/dynamic_bitset.hpp>

std::string repeat_string(const std::string &word, int times);

size_t countSubstring(const std::string& str, const std::string& sub);

std::vector<std::string> split(const std::string& s, const std::string& delim);

std::vector<std::string> split_string_by_token(const std::string& text, const std::string& token);
std::vector<std::string> split_string_by_tokens(const std::string& text, const std::vector<std::string>& tokens);

bool is_keys_intersect(const std::string& key11, const std::string& key22);

std::vector<std::string> get_keys_intersections(const std::string& key11, const std::string& key22);
//std::vector<std::string> get_keys_intersections_wh(const std::string& key11, const std::string& key22);
std::vector<std::string> get_keys_intersections_wh(std::string key11, std::string key22);

std::vector<size_t> prefix_function(const std::string& s);
size_t KMP_count(const std::string& S, const std::string& pattern, size_t start=0);
size_t preKMP_count(const std::vector<size_t>& prefix, const std::string& S, const std::string& pattern, size_t start=0);

const auto BASE=256;
size_t fasthash(std::string s, size_t base=BASE);

size_t divide_rounding_up(size_t dividend, size_t divisor);
std::string dump_bits(const std::vector<bool>& bitvector);


std::vector<size_t> onetwo_encoder(size_t num, size_t z1=0, size_t z2=1);

std::vector<size_t> dc_encode(std::vector<size_t> text, std::vector<size_t> alpha);

template <typename DataType>
std::vector<size_t> mtf(std::vector<DataType>& alphabet, std::vector<DataType>& text) {

    std::list<DataType> alpha(alphabet.begin(), alphabet.end());

    std::vector<size_t> res;
    res.reserve(text.size());

    for(auto& i: text) {
        auto alphabet_pos = std::find(alpha.begin(), alpha.end(), i);
        size_t index = distance(alpha.begin(), alphabet_pos);
        res.push_back(index);

        alpha.push_front(*alphabet_pos);
        alpha.erase(alphabet_pos);
    }

    return res;
}

template <typename DataType>
std::vector<size_t> mtf_deferred(std::vector<DataType>& alphabet, std::vector<DataType>& text) {

    std::list<DataType> alpha(alphabet.begin(), alphabet.end());
    DataType elem;

    std::vector<size_t> res;
    res.reserve(text.size());

    for(auto& i: text) {
        auto alphabet_pos = std::find(alpha.begin(), alpha.end(), i);
        size_t index = distance(alpha.begin(), alphabet_pos);

        res.push_back(index);

        //elem = *alphabet_pos;
        if(index!=0) {
            elem = *alphabet_pos;
            alpha.erase(alphabet_pos);
            if(index==1) {
                alpha.push_front(elem);

            } else
            if(index>1) {
                auto pos = alpha.begin();
                std::advance(pos, 1);

                alpha.insert(pos, elem);
            }
        }
    }

    return res;
}


template <typename DataType>
std::vector<DataType> unmtf(std::vector<DataType>& alphabet, std::vector<DataType>& text) {

    std::list<DataType> alpha(alphabet.begin(), alphabet.end());

    std::vector<DataType> res;
    res.reserve(text.size());

    for(auto& i: text) {
        auto alphabet_pos = alpha.begin();
        std::advance(alphabet_pos, i);

        res.push_back(*alphabet_pos);

        alpha.push_front(*alphabet_pos);
        alpha.erase(alphabet_pos);
    }

    return res;
}


template <typename DataType>
std::vector<DataType> rlencode(const std::vector<DataType>& text) {
//    std::cout<<" 1";
    std::vector<DataType> rled;
    rled.reserve(text.size());
    DataType cur_code=text[0], run_len=0;

    for(auto it=text.begin(); it!=text.end(); it++) {
//        std::cout<<" 2";
        if((*it)==cur_code) {
            run_len += 1;
        } else {
            rled.push_back(run_len);
            rled.push_back(cur_code);

            run_len = 1;
            cur_code = (*it);
        }
    }

    rled.push_back(run_len);
    rled.push_back(cur_code);
//    std::cout<<" 3\n";
    return rled;
}


template <typename DataType>
std::vector<DataType> rlencode_mod(const std::vector<DataType>& text) {

    std::vector<DataType> encoded, buf;
    DataType cur_code=text[0];
    size_t run_len=0, l, c;

    for(auto it=text.begin(); it!=text.end(); ) {
        cur_code = *it;
        run_len = 0;
        while(it!=text.end() && *it==cur_code) {
            run_len += 1;
            it++;
        }

        if(run_len<3) {
            buf.insert(buf.end(), run_len, cur_code);
        } else {

            if(!buf.empty()) {
                l = buf.size();
                c = 0;

                while(l>127) {
                    encoded.push_back(l<<1+0);
//                    encoded.push_back(l);
                    encoded.insert(encoded.end(), buf.begin()+c*127, buf.begin()+(c+1)*127);
                    l -= 127;
                }
                encoded.push_back(l<<1+0);
//                encoded.push_back(l);
                encoded.insert(encoded.end(), buf.begin()+c*127, std::min(buf.begin()+(c+1)*127, buf.end()));
                buf.clear();
            }

            while(run_len>127) {
                encoded.push_back(127<<1+1);
//                encoded.push_back(127);
                encoded.push_back(cur_code);
                run_len -= 127;
            }
            encoded.push_back(run_len<<1+1);
//            encoded.push_back(run_len);
            encoded.push_back(cur_code);
        }
    }

    if(!buf.empty()) {
        l = buf.size();
        c = 0;

        while(l>127) {
            encoded.push_back(127<<1+0);
            //encoded.push_back(l<<1+0);
//            encoded.push_back(l);
            encoded.insert(encoded.end(), buf.begin()+c*127, buf.begin()+(c+1)*127);
            l -= 127;
        }
        encoded.push_back(l<<1+0);
//        encoded.push_back(l);
        encoded.insert(encoded.end(), buf.begin()+c*127, std::min(buf.begin()+(c+1)*127, buf.end()));
        buf.clear();
    }

	return encoded;
}




//template <typename DataType>
//std::vector<DataType> onetwo_encoder(size_t num, DataType z1, DataType z2) {
//std::vector<size_t> onetwo_encoder(size_t num, size_t z1, size_t z2) {
////	DataType bin[2] = {z1, z2};
//	size_t bin[2] = {z1, z2};
////	std::vector<DataType> res;
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


template <typename DataType>
std::vector<DataType> rlencode_mod12(const std::vector<DataType>& text, DataType z1, DataType z2) {

    std::vector<DataType> encoded, buf;
    DataType cur_code=text[0];
    size_t run_len=0, l, c;

    for(auto it=text.begin(); it!=text.end(); ) {
        cur_code = *it;
        run_len = 0;
        while(it!=text.end() && *it==cur_code) {
            run_len += 1;
            it++;
        }

        if(run_len<3) {
            buf.insert(buf.end(), run_len, cur_code);
        } else {

            if(!buf.empty()) {
                l = buf.size();
                c = 0;

                while(l>127) {
                    auto num = onetwo_encoder(127<<1+0, z1, z2);
                    encoded.insert(encoded.end(), num.begin(), num.end());
                    //encoded.push_back(127<<1+0);
                    //encoded.push_back(l<<1+0);
//                    encoded.push_back(l);
                    encoded.insert(encoded.end(), buf.begin()+c*127, buf.begin()+(c+1)*127);
                    l -= 127;
                }
                //encoded.push_back(l<<1+0);
                auto num = onetwo_encoder(l<<1+0, z1, z2);
                encoded.insert(encoded.end(), num.begin(), num.end());

//                encoded.push_back(l);
                encoded.insert(encoded.end(), buf.begin()+c*127, std::min(buf.begin()+(c+1)*127, buf.end()));
                buf.clear();
            }

            while(run_len>127) {
                auto num = onetwo_encoder(127<<1+0, z1, z2);
                encoded.insert(encoded.end(), num.begin(), num.end());
//                encoded.push_back(127<<1+1);
//                encoded.push_back(127);
                encoded.push_back(cur_code);
                run_len -= 127;
            }

            auto num = onetwo_encoder(run_len<<1+1, z1, z2);
            encoded.insert(encoded.end(), num.begin(), num.end());
//            encoded.push_back(run_len<<1+1);
//            encoded.push_back(run_len);
            encoded.push_back(cur_code);
        }
    }

    if(!buf.empty()) {
        l = buf.size();
        c = 0;

        while(l>127) {
            encoded.push_back(l<<1+0);
//            encoded.push_back(l);
            encoded.insert(encoded.end(), buf.begin()+c*127, buf.begin()+(c+1)*127);
            l -= 127;
        }
        encoded.push_back(l<<1+0);
//        encoded.push_back(l);
        encoded.insert(encoded.end(), buf.begin()+c*127, std::min(buf.begin()+(c+1)*127, buf.end()));
        buf.clear();
    }

	return encoded;
}


template <typename DataType>
std::vector<int64_t> delta_encode(const std::vector<DataType>& text) {
//    std::cout<<" 1";
    std::vector<DataType> encoded={text[0], };
    encoded.reserve(text.size());
    DataType cur_code, prev_code;

    for(auto it=text.begin(); it!=text.end(); it++) {
        cur_code = *(it+1);
        prev_code = *(it);
        encoded.push_back(cur_code-prev_code);
    }

    return encoded;
}


#endif // SUPPORT_HPP_INCLUDED
