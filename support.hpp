#ifndef SUPPORT_HPP_INCLUDED
#define SUPPORT_HPP_INCLUDED

#include <vector>
#include <string>
#include <set>
#include <map>
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
        auto elem = *alphabet_pos;
        size_t index = distance(alpha.begin(), alphabet_pos);
        res.push_back(index);

        alpha.push_front(elem);
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


template <typename KeyType, typename DataType, typename Predicate>
std::set<KeyType> filter_map_keys(const std::map<KeyType, DataType> &map1, Predicate &cond) {
    std::set<KeyType> res;

    for(auto& it=map1.begin(); it!=map1.end; it++) {
        if(cond(it)) {
            res.insert(it.first);
        }
    }
    return res;
}





template <typename ForwardIterator, typename T>
 ForwardIterator find_fge(ForwardIterator first, ForwardIterator last, const T& val) {
//    std::cout<<"started find_fge"<<std::endl;
    ForwardIterator it;
    auto left = first;
    auto right = last-1;
    auto mid = first + (last-first)/2;

    while(right-left>=0) {
        mid = left + (right-left)/2;
//        std::cout<<"<<<<<<<<<<<<<<<<<<<<<<<<<"<<std::endl;
//        std::cout<<"right-left: "<<right-left<<std::endl;
//        std::cout<<"right: "<<right-first<<std::endl;
//        std::cout<<"left: "<<left-first<<std::endl;
//        std::cout<<"mid: "<<mid-first<<std::endl;
//        std::cout<<"values: "<<*(mid-1)<<"\t"<<*mid<<"\t"<<*(mid+1)<<std::endl;
        if((*mid)<val) {
//            std::cout<<"(*mid)<val: "<<((*mid)<val)<<std::endl;
            left = mid + 1;
        } else
        if(val<=*(mid-1) && *(mid-1)<=*mid) {
//            std::cout<<"val<=*(mid-1) && *(mid-1)<=*mid: "<< (val<=*(mid-1) && *(mid-1)<=*mid) <<std::endl;
            right = mid;
        } else
        if( ( (mid-1)-first>=0 && *(mid-1)<val && val<=*mid) ||
            ( (mid-1)-first<0 && val<=(*mid) ) ) {
            return mid;
        }
//        std::cout<<"(mid-1)-first>=0 "<< ((mid-1)-first>=0) <<std::endl;
//        std::cout<<"*(mid-1)<val "<< (*(mid-1)<val) <<std::endl;
//        std::cout<<"val<=*mid "<< (val<=*mid) <<std::endl;
//        std::cout<<"(mid-1)-first<0 "<< ((mid-1)-first<0) <<std::endl;
//        std::cout<<"val<=(*mid) "<< (val<=(*mid)) <<std::endl;
//        std::cout<<" "<<  <<std::endl;
//        std::cout<<" "<<  <<std::endl;
    }
    return last;
}


template <typename ForwardIterator, typename T>
size_t find_longest_streak(ForwardIterator first, ForwardIterator last, const T& val) {
//    std::cout<<"started find_longest_streak"<<std::endl;
    auto dist = std::distance(first, last);
    if(dist<=1) {
        return dist;
    }
//    if(last-first<=1) {
//        return last-first;
//    }

    size_t cur_len = 0;
    ForwardIterator next_pos = first;

    while(next_pos!=last) {
        cur_len += 1;
        next_pos = find_fge(next_pos, last, (*next_pos)+val);
    }

    return cur_len;
}


template <typename ForwardIterator, typename T>
std::vector<ForwardIterator> find_longest_streak_vector(ForwardIterator first, ForwardIterator last, const T& val) {
//    std::cout<<"started find_longest_streak"<<std::endl;

//    auto dist = std::distance(first, last);
    std::vector<ForwardIterator> res;
    auto dist = last - first;
    if(dist<=1) {
        res.push_back(first);
        return res;
    }

    res.reserve(dist/2);
//    if(last-first<=1) {
//        return last-first;
//    }

    size_t cur_len = 0;
    ForwardIterator next_pos = first;

    while(next_pos!=last) {
//        cur_len += 1;
        res.push_back(next_pos);
        next_pos = find_fge(next_pos, last, (*next_pos)+val);
    }

    return res;
}

template <typename ForwardIterator, typename T>
std::vector<size_t> find_longest_streak_values(ForwardIterator first, ForwardIterator last, const T& val) {

    std::vector<size_t> res;
    auto dist = last - first;
    if(dist<=1) {
        res.push_back(dist);
        return res;
    }

    res.reserve(dist/2);

    size_t cur_len = 0;
    ForwardIterator next_pos = first;

    while(next_pos!=last) {
        res.push_back(*next_pos);
        next_pos = find_fge(next_pos, last, (*next_pos)+val);
    }

    return res;
}



//template <typename ForwardIterator, typename T>
template <typename ForwardIterator>
std::vector<ForwardIterator> find_minimals(ForwardIterator first, ForwardIterator last) {
//    std::cout<<"started find_minimals"<<std::endl;
    std::vector<ForwardIterator> res;
    if(last-first<=1) {
        if(last-first==1) {
            res.push_back(first);
        }
        return res;
    }

    if (*first<*(first+1)) {
        res.push_back(first);
    }

    for(auto it=first+1; it!=last-1; it++) {
        if((*it<*(it-1) && *it<=*(it+1)) || (*it<=*(it-1) && *it<*(it+1))) {
            res.push_back(it);
        }
    }

    if(*(last-1)<*(last-2)) {
        res.push_back((last-1));
    }

    return res;
}


template <typename ForwardIterator>
ForwardIterator find_peak(ForwardIterator first, ForwardIterator last) {
//    std::cout<<"started find_peak"<<std::endl;
    if(last-first==2) {
        if(*first<*(first+1)) {
            return first+1;
        } else {
            return first;
        }
    } else
    if(last-first==1) {
        return first;
    } else
    if(last-first==0) {
        return last;
    }

    ForwardIterator it;
    while(1) {
        it = first + (last-first)/2;
        if(*it<*(it-1)) {
            last = it;
        } else
        if(*it<*(it+1)) {
            first = it+1;
        } else {
            return it;
        }
    }
}


template <typename ForwardIterator>
std::vector<ForwardIterator> find_peaks(ForwardIterator first, ForwardIterator last) {
//    std::cout<<"started find_peaks"<<std::endl;
    auto minimals = find_minimals(first, last);

    std::vector<ForwardIterator> res;
    res.reserve(minimals.size());

    if(last-first==1 || !minimals.size()) {
        res.push_back(first);
        return res;
    }

//    std::cout<<">>>>minimals:"<<std::endl;
//    for(auto& i: minimals) {
//        std::cout<<i-first<<" ";
//    }
//    std::cout<<std::endl;


//    std::cout<<"find_peaks 1"<<std::endl;
    if(last-first==1 || (last-first>1 && *first>*(first+1))) {
//        std::cout<<"first added"<<std::endl;
        res.push_back(first);
    }

//    std::cout<<"find_peaks 2"<<std::endl;
    //for(auto& it=minimals.begin(); it!=(minimals.end()-1); it++) {

    for(auto i=0; i<minimals.size()-1; i++) {
        res.push_back(find_peak(minimals[i], minimals[i+1]));
    }

//    std::cout<<"find_peaks 3"<<std::endl;
    if(last-first>1 && *(last-1)>*(last-2)) {
//        std::cout<<"last added"<<std::endl;
        res.push_back(last-1);
    }
//    std::cout<<"find_peaks 4"<<std::endl;

    return res;
}


#endif // SUPPORT_HPP_INCLUDED
