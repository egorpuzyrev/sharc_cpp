#include <iostream>
#include <fstream>
#include <iterator>
#include <chrono>
#include "huffman.hpp"
#include "statistics.hpp"
#include "keys.hpp"
#include "markov.hpp"
#include "support.hpp"
#include "compressor.hpp"
#include "rabinkarp.hpp"
#include "base64.hpp"
#include "argparse.hpp"
#include "string_compressors.hpp"

//#include <easy/profiler.h>
//#include <easy/reader.h>
//EASY_PROFILER_ENABLE;


std::string repeat( const std::string &word, int times ) {
   std::string result ;
   result.reserve(times*word.length()); // avoid repeated reallocation
   for ( int a = 0 ; a < times ; a++ )
      result += word ;
   return result ;
}

int main() {
//    EASY_PROFILER_ENABLE;
    //EASY_MAIN_THREAD;
//    profiler::startListen();
    //std::string SampleString = repeat("karl_u_klari_ukral_koralli_a_klara_u_karla_ukrala_klarnet", 1000);
    std::ifstream f("/home/egor/Build/argparse.tar");
    //std::ifstream f("/home/egor/Загрузки/condoyle.txt");
//    std::ifstream f("/home/egor/Загрузки/my_audios.html");
    //std::ifstream f("/home/egor/Programs/py/vm_book1.txt");

    std::string SampleString((std::istreambuf_iterator<char>(f)),
                 std::istreambuf_iterator<char>());

//    std::string text = SampleString;
//    std::string text = base64_encode(SampleString.substr(0, 128*1024));
    std::string text = SampleString.substr(0, 128*1024);
//    std::string text = "karl_u_klari_ukral_koralli_a_klara_u_karla_ukrala_klarnet";
    //std::string text = "aaaasaaaaasaaaaasaaaaas";
    //std::string text = "aaaasaaaaasa";
    //std::string text = base64_encode(SampleString.substr(0, 64*1024));
    std::cout<<"Text size: "<<text.size()<<std::endl;

    auto start = std::chrono::system_clock::now();
//    auto keys_naive = get_keys_naive(text, 2, 2, 32);
    auto finish = std::chrono::system_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(finish - start);
    std::cout << "Get keys naive: " << elapsed.count() << std::endl;
//    std::cout << "keys_naive size: " << keys_naive.size() << std::endl << std::endl;

//    start = std::chrono::system_clock::now();
//    auto keys_lcp = get_keys_by_lcp(text, 2, 2, 32, 32);
//    finish = std::chrono::system_clock::now();
//    elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(finish - start);
//    std::cout << "Get keys lcp: " << elapsed.count() << std::endl;
//    std::cout << "keys_lcp size: " << keys_lcp.size() << std::endl << std::endl;

    size_t c=0, a;
//    std::cin>>a;
//    for(auto& i: keys_naive) {
//        //if(i.second!=keys_naive[i.first]) {
//        if(i.second!=keys_lcp[i.first]) {
//        //if(i.second<2) {
//            c++;
//            std::cout<<i.first<<"\t"<<i.second<<"\t"<<keys_lcp[i.first]<<std::endl;
//        }
//        //if(i.second>=2 && 2<=i.first.length()<=32) c++;
////        if(!keys_lcp.count(i.first)) {
////            std::cout<<i.first<<"\t"<<i.second<<"\t"<<countSubstring(text, i.first)<<std::endl;
////        }
//    }
    std::cout<<"Counter: "<<c<<std::endl;

//    EASY_BLOCK("Compress block");
    compress_block(text, 32);
    std::cout<<"\nBzip2.compress(text).size(): "<<Bzip2::compress(text).size();
    std::cout<<"\nGzip.compress(text).size(): "<<Gzip::compress(text).size()<<std::endl;
    //EASY_END_BLOCK;
    //compress(text, 32);
    //7200 20157 8895 36252
    //11016 17120 13072 41208
    //7056 21480 8523 37059
    //3024 22533 5756 31313

    //32
    //7200 20157 8895 36252
    //
    //3016 22488 5746 31250

    //128
    //
    //
    //3016 22488 5746 31250


//    split(SampleString, "a");
//    split_string_by_tokens(SampleString, {"ar", "_u"});
//
//    Markov<std::string, std::string> markov(&SampleString);
//    auto chain = markov.build_chain();
//    for(auto& i: chain) {
//        std::cout<<i.first<<":"<<std::endl;
//        for(auto& j: i.second) {
//            std::cout<<"\t"<<j.first<<":\t"<<j.second<<std::endl;
//        }
//    }
//
//    Counts<std::string> keys = get_keys_naive(SampleString, 1, 2, 32);
//    std::cout<<"len(SampleString) = "<<SampleString.length()<<std::endl;
//    std::cout<<"len(keys) = "<<keys.size()<<std::endl;
//    std::cout<<"keys = {"<<std::endl;
//
//    std::cout<<"}"<<std::endl;
//
//    Counts<std::string> frequencies;
//    for (auto it=SampleString.begin(); it!=SampleString.end(); ++it) {
//        frequencies[std::string(1, *it)] += 1;
//    };
//
//    fCounts<std::string> weights = get_weights(&get_weight1, frequencies, SampleString.length(), 2);
//    for (auto& i: weights){
//        std::cout<<i.first<<": "<<i.second<<std::endl;
//    }
//
//    HuffmanEncoder<std::string> henc;
//    henc.InitFrequencies(frequencies);
//    henc.Encode();
//    auto codes = henc.outCodes;
//    std::cout << "str_codes1 = {" << std::endl;
//    for (auto it = codes.begin(); it != codes.end(); ++it)
//    {
//        std::cout << '"' << it->first << '"' << " : '";
//        std::copy(it->second.begin(), it->second.end(),
//                  std::ostream_iterator<bool>(std::cout));
//        std::cout << "'," << std::endl;
//    }
//    std::cout << "}" << std::endl;
//    EASY_END_BLOCK;
//    profiler::stopListen();
//    auto blocks_count = profiler::dumpBlocksToFile("test_profile.prof");
//    std::cout << "Blocks count: " << blocks_count << std::endl;
    return 0;
}
