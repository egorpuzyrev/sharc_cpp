#ifndef HUFFMAN_HPP_INCLUDED
#define HUFFMAN_HPP_INCLUDED

#include <queue>
#include <map>
#include "global.hpp"

class INode {
public:
    const int f;

    virtual ~INode() {};

protected:
    INode(int f) : f(f) {};
};


class InternalNode : public INode {
public:
    INode *const left;
    INode *const right;

    InternalNode(INode* c0, INode* c1) : INode(c0->f + c1->f), left(c0), right(c1) {}
    ~InternalNode()
    {
        delete left;
        delete right;
    }
};

template <typename DataType>
class LeafNode : public INode {
public:
    const DataType c;

    LeafNode(int f, DataType c) : INode(f), c(c) {}
};

struct NodeCmp
{
    bool operator()(const INode* lhs, const INode* rhs) const { return lhs->f > rhs->f; }
};


//template <typename DataType>
//using MarkovChain = std::map<DataType, Counts<DataType>>;
typedef std::vector<bool> HuffCode;

template <typename DataType>
using HuffCodeMap = std::map<DataType, HuffCode>;


template <typename DataType>
class HuffmanEncoder {

public:
    //typedef Counts<DataType> HuffFreqs;


    //HuffFreqs freqs;
    Counts<DataType> freqs;
    HuffCodeMap<DataType> outCodes;

    HuffmanEncoder() {};
    HuffmanEncoder(Counts<DataType> frequencies) {
        this->freqs = frequencies;
    };
    //HuffmanEncoder(Counts<DataType> frequencies): freqs(frequencies) {};
    ~HuffmanEncoder() {};

    //void InitFrequencies(HuffFreqs frequencies) {
    void InitFrequencies(Counts<DataType> frequencies) {
        this->freqs = frequencies;
        this->outCodes.clear();
    };

    void Encode() {
        INode* root = BuildTree(this->freqs);
        GenerateCodes(root, HuffCode());
        delete root;
    };

    //INode* BuildTree(const HuffFreqs &frequencies) {
    INode* BuildTree(const Counts<DataType> &frequencies) {

        std::priority_queue<INode*, std::vector<INode*>, NodeCmp> trees;

        for (auto it=frequencies.begin(); it!=frequencies.end(); ++it) {
            trees.push(new LeafNode<DataType>(it->second, it->first));
        }
        while (trees.size() > 1)
        {
            INode* childR = trees.top();
            trees.pop();

            INode* childL = trees.top();
            trees.pop();

            INode* parent = new InternalNode(childR, childL);
            trees.push(parent);
        }
        return trees.top();
    };

    void GenerateCodes(const INode* node, const HuffCode& prefix)
    {
        if (const LeafNode<DataType>* lf = dynamic_cast<const LeafNode<DataType>*>(node))
        {
            this->outCodes[lf->c] = prefix;
        }
        else if (const InternalNode* in = dynamic_cast<const InternalNode*>(node))
        {
            HuffCode leftPrefix = prefix;
            leftPrefix.push_back(false);
            GenerateCodes(in->left, leftPrefix);

            HuffCode rightPrefix = prefix;
            rightPrefix.push_back(true);
            GenerateCodes(in->right, rightPrefix);
        }
    };


//    std::array<std::vector<bool>, 3> pack(bool if_sort=1,
//                                          bool (*sort_fun)(std::string s1, std::string s2)=[](std::string s1, std::string s2){return s1>s2;}) {
    std::array<HuffCode, 3> pack(std::vector<DataType> keys) {
//        std::vector<std::string> keys;
//        for(auto& i: this->freqs) {
//            keys.push_back(i.first);
//        }
//        if(if_sort) {
//            std::sort(keys.begin(), keys.end(), sort_fun);
//        }

        HuffCode keys_mask;
        HuffCode keys_huffman_codes;
        HuffCode keys_huffman_codes_mask;

        bool cur_mask = 1;
        for(const auto& key: keys) {
//            for(auto j=0; j<key.length(); j++) {
//                keys_mask.push_back(cur_mask);
//            }
            //keys_mask.reserve(keys_mask.size() + key.length());
            keys_mask.insert(keys_mask.end(), key.size(), cur_mask);

            //keys_huffman_codes.reserve(keys_huffman_codes.size() + this->outCodes[key].size());
            keys_huffman_codes.insert(keys_huffman_codes.end(), this->outCodes[key].begin(), this->outCodes[key].end());
//            for(auto j=0; j<this->outCodes[key].size(); j++) {
//                keys_huffman_codes_mask.push_back();
//            }
            //keys_huffman_codes_mask.reserve(keys_huffman_codes_mask.size() + this->outCodes[key].size());
            keys_huffman_codes_mask.insert(keys_huffman_codes_mask.end(), this->outCodes[key].size(), cur_mask);

            cur_mask ^= 1;
        }

//        return std::array<std::vector<bool>>({keys_mask, key_huffman_codes, keys_huffman_codes_mask});
        return {keys_mask, keys_huffman_codes, keys_huffman_codes_mask};
    }
};


#endif // HUFFMAN_HPP_INCLUDED
