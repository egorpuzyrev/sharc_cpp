#ifndef AHOCORASIK_HPP_INCLUDED
#define AHOCORASIK_HPP_INCLUDED


#include <cstdio>
#include <cstring>
#include <iostream>
#include <algorithm>
#include <vector>
#include <queue>
#include <string>
#include <unordered_set>

#include "global.hpp"

//int next_id = 0;

struct Node
{   Node *suff;
    Node *next[256];
    std::vector<int> pats;
    std::vector<unsigned char> out;
    int id; /* for debugging only */

    Node();
//    ~Node();
};


class AhoCorasick
{   Node *trie;

    Node *suffix(Node *x, char c);

    void insert(Node *x, std::string s, int id);


    public:

    AhoCorasick(std::vector<std::string> &p);
    void match(std::string s);
    Counts<size_t> multicount(std::string s);

};


//Counts<size_t> multicount_aho(std::string s);
Counts<std::string> multicount_aho(std::string text, const std::unordered_set<std::string>& hashes_set);

//std::pair<Counts<std::string>, std::map<std::string, std::set<size_t>>> multipos_aho(std::string text, const std::unordered_set<std::string>& hashes_set);
std::tuple< Counts<std::string>,
            std::map<std::string, std::set<size_t>>,
            std::vector<size_t>
            >
    multipos_aho(std::string text, const std::unordered_set<std::string>& hashes_set);

#endif // AHOCORASIK_HPP_INCLUDED
