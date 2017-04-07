#ifndef HUFFMAN_CPP_INCLUDED
#define HUFFMAN_CPP_INCLUDED

#include <iostream>
#include <queue>
#include <map>
#include <iterator>
#include <algorithm>

#include "huffman.hpp"

template <typename DataType>
HuffmanEncoder<DataType>::HuffmanEncoder() {
}

template <typename DataType>
HuffmanEncoder<DataType>::~HuffmanEncoder() {
}

template <typename DataType>
void HuffmanEncoder<DataType>::InitFrequencies(HuffFreqs frequencies) {
    this->freqs = frequencies;
}

template <typename DataType>
void Encode() {
    INode* root = BuildTree(freqs);
    GenerateCodes(root, HuffCode());
}

template <typename DataType>
INode* BuildTree(const HuffFreqs &frequencies) {

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
}

template <typename DataType>
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
        GenerateCodes(in->left, leftPrefix, this->outCodes);

        HuffCode rightPrefix = prefix;
        rightPrefix.push_back(true);
        GenerateCodes(in->right, rightPrefix, this->outCodes);
    }
}

#endif
