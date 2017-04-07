#ifndef GLOBAL_HPP_INCLUDED
#define GLOBAL_HPP_INCLUDED

#include <map>

template <typename DataType>
using Counts = std::map<DataType, size_t>;

template <typename DataType>
using fCounts = std::map<DataType, float>;

#endif // GLOBAL_HPP_INCLUDED
