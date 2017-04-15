#ifndef BASE64_HPP_INCLUDED
#define BASE64_HPP_INCLUDED

#include <string>

//std::string base64_encode(unsigned char const* bytes_to_encode, unsigned int len);
std::string base64_encode(const std::string& bytes_to_encode);
std::string base64_decode(std::string const& s);

#endif // BASE64_HPP_INCLUDED
