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


template <typename DataType>
using MarkovChain = std::map<DataType, Counts<DataType>>;

template <typename DataType>
class HuffmanEncoder {

public:
    //typedef Counts<DataType> HuffFreqs;
    typedef std::vector<bool> HuffCode;
    typedef std::map<DataType, HuffCode> HuffCodeMap;

    //HuffFreqs freqs;
    Counts<DataType> freqs;
    HuffCodeMap outCodes;

    HuffmanEncoder() {};
    //HuffmanEncoder(Counts<DataType> frequencies): freqs(frequencies) {};
    ~HuffmanEncoder() {};

    //void InitFrequencies(HuffFreqs frequencies) {
    void InitFrequencies(Counts<DataType> frequencies) {
        this->freqs = frequencies;
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
};


#endif // HUFFMAN_HPP_INCLUDED
