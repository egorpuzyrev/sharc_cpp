#ifndef COMPRESSOR_HPP_INCLUDED
#define COMPRESSOR_HPP_INCLUDED

#include <string>
#include <vector>

std::string compress_block(std::string text, size_t block_size);
std::string decompress_block(std::string text);

std::string compress(std::string text, size_t block_size);
std::string decompress(std::string text);



const char ID[] = "sharc";
const char VERSION[] = "03";

struct CompressedFileHeader;
struct CompressedBlock;

struct CompressedFileHeader {
    /// section 1
    /// format description
    char id[5];
    char version[2];

    /// section 2
    /// blocks description
    uint64_t blocks_count;
    uint64_t uncompressed_block_length;
    uint64_t last_block_len;

    std::vector<CompressedBlock> compressed_blocks;
};

struct CompressedBlock {

    uint64_t compressed_block_length;
    char preprocessor;
    char keys_compression_method;

    uint32_t keys_count;
    uint32_t uncompressed_keys_list_length;
    uint32_t compressed_keys_list_length;
    uint32_t terminator_code_length;

    std::string keys;
    std::string keys_mask;

    std::string terminator_code;
    std::string keys_codes;
    std::string transitions_codes;

    uint32_t encoded_message_length;
    uint32_t transitions_keys_count;
    uint32_t transitions_keys_length;
    std::string transitions_leys_mask;

};

#endif // COMPRESSOR_HPP_INCLUDED
