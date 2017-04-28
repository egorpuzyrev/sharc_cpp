#ifndef SUPPORT_HPP_INCLUDED
#define SUPPORT_HPP_INCLUDED

#include <vector>
#include <string>
#include <algorithm>
#include <list>

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
std::vector<DataType> unmtf(std::vector<DataType>& alphabet, std::vector<size_t>& text) {

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


#endif // SUPPORT_HPP_INCLUDED
