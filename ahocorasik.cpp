#include <cstdio>
#include <cstring>
#include <iostream>
#include <algorithm>
#include <vector>
#include <queue>
#include <string>
#include <chrono>
#include <unordered_set>
#include <set>

#include "ahocorasik.hpp"
#include "global.hpp"


size_t next_id = 0;

Node::Node()
{   id = next_id++;
    suff = 0;
    for(int i = 0; i < 256; i++)
    {   next[i] = 0;
    }
}

//Node::~Node()
//{
//    for(int i = 0; i < 256; i++)
//    {
//        if(next[i]) {
//            delete next[i];
//        }
//    }
//}



Node* AhoCorasick::suffix(Node *x, char c)
{   while(x->next[c] == 0)
    {   x = x->suff;
    }
    return x->next[c];
}

void AhoCorasick::insert(Node *x, std::string s, int id)
{   for(int i = 0; i < s.size(); i++)
    {   if(x->next[s[i]] == 0)
        {   x->next[s[i]] = new Node;
            x->out.push_back(s[i]);
        }
        x = x->next[s[i]];
    }
    x->pats.push_back(id);
}


AhoCorasick::AhoCorasick(std::vector<std::string> &p)
{   trie = new Node;

    for(int i = 0; i < p.size(); i++)
    {   insert(trie, p[i], i);
    }

    std::queue<Node *> q;
    for(int i = 0; i < 256; i++)
    {   if(trie->next[i])
        {   trie->next[i]->suff = trie;
            q.push(trie->next[i]);
        }
        else
        {   trie->next[i] = trie;
        }
    }

    while(q.empty() == 0)
    {   Node *x = q.front(); q.pop();
        for(int i = 0; i < x->out.size(); i++)
        {   Node *y = x->next[x->out[i]];
            y->suff = suffix(x->suff, x->out[i]);
            y->pats.insert(y->pats.end(), y->suff->pats.begin(), y->suff->pats.end());
            q.push(y);
        }
    }
}

void AhoCorasick::match(std::string s)
{   Node *x = trie;
    int cnt = 0;
    for(int i = 0; i < s.size(); i++)
    {   x = suffix(x, s[i]);
        //x->pats contains id's of matched patterns
//        cout<<endl;
//        for(auto& i: x->pats) {
//            cout<<" "<<i;
//        }
    }
}


Counts<size_t> AhoCorasick::multicount(std::string s) {

    Counts<size_t> res;

    Node *x = trie;
    int cnt = 0;
    for(int i = 0; i < s.size(); i++) {
        x = suffix(x, s[i]);
        for(auto& j: x->pats) {
            res[j] += 1;
        }
    }

    return res;
}


//Counts<std::string> multicount_aho(std::string text, const std::unordered_set<std::string>& hashes_set) {
//
//
//    auto start = std::chrono::system_clock::now();
//
//    std::vector<std::string> vec(hashes_set.begin(), hashes_set.end());
//
//    size_t l=text.length();
//    Counts<std::string> counts;
//    std::map<std::string, int> prev_pos;
//
//
//    for(auto it=vec.begin(); it!=vec.end(); it++) {
//        counts[(*it)] = 0;
//        prev_pos[(*it)] = -(*it).length()-1;
//    }
//
//    AhoCorasick aho(vec);
//    auto res = aho.multicount(text);
//
//    for(auto& i: res) {
//        counts[vec[i.first]] = i.second;
//    }
//
//
//    auto finish = std::chrono::system_clock::now();
//    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(finish - start);
//
//    std::cout<<">>>AhoCorasik multiount time: "<< elapsed.count() << std::endl;
//
//}


void freenode(Node *x, Node* root) {

    for(int i=0; i<256; i++) {
        if(x->next[i] != 0 && x->next[i] != root) {
            freenode(x->next[i], root);
            delete x->next[i];
        }
    }
}


void insert(Node *x, std::string s, int id)
{   for(int i = 0; i < s.size(); i++){
        if(x->next[(unsigned char)s[i]] == 0) {
            x->next[(unsigned char)s[i]] = new Node;
            x->out.push_back((unsigned char)s[i]);
        }
        x = x->next[(unsigned char)s[i]];
    }
    x->pats.push_back(id);
}

Node* suffix(Node *x, unsigned char c)
{   while(x->next[c] == 0)
    {   x = x->suff;
    }
    return x->next[c];
}


Counts<std::string> multicount_aho(std::string text, const std::unordered_set<std::string>& hashes_set) {


    auto start = std::chrono::system_clock::now();

    std::vector<std::string> p(hashes_set.begin(), hashes_set.end());

    size_t l=text.length();
    Counts<std::string> counts;
    std::map<std::string, int> prev_pos;


    for(auto it=p.begin(); it!=p.end(); it++) {
        counts[(*it)] = 0;
        prev_pos[(*it)] = -(*it).length()-1;
    }


    Node *trie;

    trie = new Node;

    for(int i = 0; i < p.size(); i++)
    {   insert(trie, p[i], i);
    }

    std::queue<Node *> q;
    for(int i = 0; i < 256; i++)
    {   if(trie->next[i])
        {   trie->next[i]->suff = trie;
            q.push(trie->next[i]);
        }
        else
        {   trie->next[i] = trie;
        }
    }

    while(q.empty() == 0)
    {   Node *x = q.front(); q.pop();
        for(int i = 0; i < x->out.size(); i++)
        {   Node *y = x->next[x->out[i]];
            y->suff = suffix(x->suff, x->out[i]);
            y->pats.insert(y->pats.end(), y->suff->pats.begin(), y->suff->pats.end());
            q.push(y);
        }
    }


    std::string sub;
    Node *x = trie;
    int cnt = 0;
    for(int i = 0; i < text.size(); i++) {
        x = suffix(x, text[i]);
        for(auto& j: x->pats) {
            //res[j] += 1;
            //counts[p[j]] += 1;
            sub = p[j];
            if(i-prev_pos[sub]>=sub.size()) {
                prev_pos[sub] = i;
                counts[sub] += 1;
//                    std::cout<<"\t<"<<sub<<">\t"<<counts[sub]<<std::endl;
            }
        }
    }

//    auto clean_start = std::chrono::system_clock::now();
    freenode(trie, trie);
    delete trie;


    auto finish = std::chrono::system_clock::now();
//    auto clean_elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(finish - clean_start);
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(finish - start);

//    std::cout<<">>>clean time: "<< clean_elapsed.count() << std::endl;
    std::cout<<">>>AhoCorasik multiount time: "<< elapsed.count() << std::endl;

    return counts;
}
