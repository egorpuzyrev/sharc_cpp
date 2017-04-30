#ifndef MARKOV_HPP_INCLUDED
#define MARKOV_HPP_INCLUDED

#include <iostream>
#include "global.hpp"

template <typename DataType>
using MarkovChain = std::map<DataType, Counts<DataType>>;

template <typename TextDataType, typename ElemsDataType>
class Markov {
public:
    TextDataType* text;

    //Markov(TextDataType* text) : text(text) {};
    Markov(TextDataType* text) {
        this->text = text;
    };

    ~Markov() {
    }

    void update_text(TextDataType text) {
        this->text = text;
    }

//    MarkovChain<ElemsDataType> build_chain() {
    MarkovChain<ElemsDataType> build_chain(size_t order) {
        ElemsDataType key1;
        ElemsDataType key2;
        MarkovChain<ElemsDataType> res;
        size_t l = this->text->size();
//        for(size_t i=0; i<l-1; i++) {
        for(size_t i=0; i<l-order; i++) {
            key1 = (*text)[i];
            key2 = (*text)[i+order];
//            key2 = (*text)[i+1];
            //std::cout<<"keys: <"<<key1<<"><"<<key2<<">"<<std::endl;
            res[key1][key2] += 1;
            //std::cout<<"res["<<key1<<"]["<<key2<<"] = "<<res[key1][key2]<<std::endl;
        }
        return res;
    }


};

#endif // MARKOV_HPP_INCLUDED
