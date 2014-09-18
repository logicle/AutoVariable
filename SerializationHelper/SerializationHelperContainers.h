#ifndef _INCLUDED_SerializationHelperContainers_H
#define _INCLUDED_SerializationHelperContainers_H

#include "SerializationHelper.h"
#include <string>
#include <vector>
#include <map>

template<typename SequenceType>
void writeSequenceTo(std::vector<unsigned char> & target, const SequenceType & source)
{
	const size_t length = source.size();
	target << length;

	for (auto e : source)
	{
		target << e;
	}
}

inline void writeTo(std::vector<unsigned char> & target, const std::string & source)
{
	target << source.length();
	for (auto c : source)
	{
		target << c;
	}
}

inline void readFrom(std::vector<unsigned char>::const_iterator & source, std::string & target)
{
	size_t length(initializeFrom<size_t>(source));
	size_t i = 0;
	target.clear();

	for (i = 0; i < length; ++i)
	{
		target.push_back(*source++);
	}
}

template<typename ElementType>
void writeTo(std::vector<unsigned char> & target, const std::vector<ElementType> & source)
{
	writeSequenceTo(target, source);
}

template<typename ElementType>
void readFrom(std::vector<unsigned char>::const_iterator & source, std::vector<ElementType> & target)
{
	size_t length(initializeFrom<size_t>(source));
	size_t i = 0;
	target.clear();

	for (i = 0; i < length; ++i)
	{
		const ElementType e(initializeFrom<ElementType>(source));
		target.push_back(e);
	}
}

template<typename First, typename Second>
void writeTo(std::vector<unsigned char> & target, const std::pair<First, Second> & source)
{
	target << source.first;
	target << source.second;
}

template<typename First, typename Second>
void readFrom(std::vector<unsigned char>::const_iterator & source, std::pair<First, Second> & target)
{
	source >> target.first;
	source >> target.second;
}

template<typename Key, typename Value>
void writeTo(std::vector<unsigned char> & target, const std::map<Key, Value> & source)
{
	writeSequenceTo(target, source);
}

template<typename Key, typename Value>
void readFrom(std::vector<unsigned char>::const_iterator & source, std::map<Key, Value> & target)
{
	size_t length(initializeFrom<size_t>(source));
	size_t i = 0;
	target.clear();

	for (i = 0; i < length; ++i)
	{
		const std::pair<Key, Value> e(initializeFrom<std::pair<Key, Value> >(source));
		target.insert(e);
	}
}

#endif//_INCLUDED_SerializationHelperContainers_H
