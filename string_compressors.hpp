#ifndef STRING_COMPRESSORS_H_INCLUDED
#define STRING_COMPRESSORS_H_INCLUDED

#include <sstream>
#include <boost/iostreams/filtering_streambuf.hpp>
#include <boost/iostreams/copy.hpp>
#include <boost/iostreams/filter/gzip.hpp>
#include <boost/iostreams/filter/bzip2.hpp>

//class Gzip {
//public:
//    static std::string compress(const std::string& data);
//    static std::string decompress(const std::string& data);
//};
//
//class Bzip2 {
//public:
//    static std::string compress(const std::string& data);
//    static std::string decompress(const std::string& data);
//};


class Gzip {
public:
	static std::string compress(const std::string& data)
	{
		namespace bio = boost::iostreams;

		std::stringstream compressed;
		std::stringstream origin(data);

		bio::filtering_streambuf<bio::input> out;
		out.push(bio::gzip_compressor(bio::gzip_params(bio::gzip::best_compression)));
		out.push(origin);
		bio::copy(out, compressed);

		return compressed.str();
	}

	static std::string decompress(const std::string& data)
	{
		namespace bio = boost::iostreams;

		std::stringstream compressed(data);
		std::stringstream decompressed;

		bio::filtering_streambuf<bio::input> out;
		out.push(bio::gzip_decompressor());
		out.push(compressed);
		bio::copy(out, decompressed);

		return decompressed.str();
	}
};


class Bzip2 {
public:
	static std::string compress(const std::string& data)
	{
		namespace bio = boost::iostreams;

		std::stringstream compressed;
		std::stringstream origin(data);

		bio::filtering_streambuf<bio::input> out;
		//out.push(bio::bzip2_compressor(bio::bzip2_params(bio::bzip2::best_compression)));
		out.push(bio::bzip2_compressor(bio::bzip2_params()));
		out.push(origin);
		bio::copy(out, compressed);

		return compressed.str();
	}

	static std::string decompress(const std::string& data)
	{
		namespace bio = boost::iostreams;

		std::stringstream compressed(data);
		std::stringstream decompressed;

		bio::filtering_streambuf<bio::input> out;
		out.push(bio::bzip2_decompressor());
		out.push(compressed);
		bio::copy(out, decompressed);

		return decompressed.str();
	}
};

#endif // STRING_COMPRESSORS_H_INCLUDED
