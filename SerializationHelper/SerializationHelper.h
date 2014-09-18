#ifndef _INCLUDED_SerializationHelper_H
#define _INCLUDED_SerializationHelper_H

#include <vector>
#include <limits>
#include <stdint.h>

#define LITTLE_ENDIAN 0x41424344UL 
#define BIG_ENDIAN    0x44434241UL
#define PDP_ENDIAN    0x42414443UL
#define ENDIAN_ORDER  ('ABCD') 

#if ENDIAN_ORDER==LITTLE_ENDIAN
//#error "machine is little endian"
#	define IS_LITTLE_ENDIAN
#elif ENDIAN_ORDER==BIG_ENDIAN
//#error "machine is big endian"
#	define IS_BIG_ENDIAN
#endif

inline void writeTo(std::vector<unsigned char> & target, const bool & source)
{
	unsigned char value = 0 ? 1 : source;
	target.push_back(value);
}

inline void readFrom(std::vector<unsigned char>::const_iterator & source, bool & target)
{
	unsigned char value = *source++;
	target = (value != 0);
}

inline void writeTo(std::vector<unsigned char> & target, const unsigned char & source)
{
	target.push_back(source);
}

inline void readFrom(std::vector<unsigned char>::const_iterator & source, unsigned char & target)
{
	target = *source++;
}

inline void writeTo(std::vector<unsigned char> & target, const char & source)
{
	target.push_back(static_cast<unsigned char>(source));
}

inline void readFrom(std::vector<unsigned char>::const_iterator & source, char & target)
{
	target = static_cast<char>(*source++);
}

inline void write2BytesFromInteger(std::vector<unsigned char> & target, const unsigned char * const p)
{
#ifdef IS_LITTLE_ENDIAN
#	if	USHRT_MAX == UINT16_MAX
	{
		target.push_back(p[1]);
		target.push_back(p[0]);
	}

	// why truncate? Payloads created on machines with larger
	// short ints will not work on machines with 2 byte
	// shorts
#	elif USHRT_MAX == UINT32_MAX
	{
		target.push_back(p[3]);
		target.push_back(p[2]);
	}
#	elif USHRT_MAX == UINT64_MAX
	{
		target.push_back(p[7]);
		target.push_back(p[6]);
	}
#	endif//USHRT_MAX == UINT16_MAX
#else
	target.push_back(p[0]);
	target.push_back(p[1]);
	// no need for the rest, the right bits are in the right place already
#endif
}

inline void read2BytesToInteger(std::vector<unsigned char>::const_iterator & source, unsigned char * const p)
{
#ifdef IS_LITTLE_ENDIAN
#	if	USHRT_MAX == UINT16_MAX
	{
		p[1] = *source++;
		p[0] = *source++;
	}

	// why truncate? Payloads created on machines with larger
	// short ints will not work on machines with 2 byte
	// shorts
#	elif USHRT_MAX == UINT32_MAX
	{
		p[3] = *source++;
		p[2] = *source++;
		p[1] = 0;
		p[0] = 0;
	}
#	elif USHRT_MAX == UINT64_MAX
	{
		p[7] = *source++;
		p[6] = *source++;
		p[5] = 0;
		p[4] = 0;
		p[3] = 0;
		p[2] = 0;
		p[1] = 0;
		p[0] = 0;
	}
#	endif//USHRT_MAX == UINT16_MAX
#else
	p[0] = *source++;
	p[1] = *source++;
#	if USHRT_MAX == UINT32_MAX
	{
		p[2] = 0;
		p[3] = 0;
	}
#	elif USHRT_MAX == UINT64_MAX
	{
		p[2] = 0;
		p[3] = 0;
		p[4] = 0;
		p[5] = 0;
		p[6] = 0;
		p[7] = 0;
	}
#	endif//USHRT_MAX == UINT16_MAX
#endif
}

inline void write4BytesFromInt(std::vector<unsigned char> & target, const unsigned char * const p)
{
#ifdef IS_LITTLE_ENDIAN
#	if UINT_MAX == UINT32_MAX
	{
		target.push_back(p[3]);
		target.push_back(p[2]);
		target.push_back(p[1]);
		target.push_back(p[0]);
	}
#	elif UINT_MAX == UINT64_MAX
	{
		target.push_back(p[7]);
		target.push_back(p[6]);
		target.push_back(p[5]);
		target.push_back(p[4]);
	}
#	endif//UINT_MAX == UINT32_MAX
#else
	target.push_back(p[0]);
	target.push_back(p[1]);
	target.push_back(p[2]);
	target.push_back(p[3]);
#endif
}

inline void read4BytesToInteger(std::vector<unsigned char>::const_iterator & source, unsigned char * const p)
{
#ifdef IS_LITTLE_ENDIAN
#	if UINT_MAX == UINT32_MAX
	{
		p[3] = *source++;
		p[2] = *source++;
		p[1] = *source++;
		p[0] = *source++;
	}
#	elif UINT_MAX == UINT64_MAX
	{
		p[7] = *source++;
		p[6] = *source++;
		p[5] = 0;
		p[4] = 0;
		p[3] = 0;
		p[2] = 0;
		p[1] = 0;
		p[0] = 0;
	}
#	endif//USHRT_MAX == UINT16_MAX
#else
	p[0] = *source++;
	p[1] = *source++;
	p[2] = *source++;
	p[3] = *source++;
#	if UINT_MAX == UINT64_MAX
	{
		p[4] = 0;
		p[5] = 0;
		p[6] = 0;
		p[7] = 0;
	}
#	endif//USHRT_MAX == UINT16_MAX
#endif
}

inline void write8BytesFromInt(std::vector<unsigned char> & target, const unsigned char * const p)
{
#ifdef IS_LITTLE_ENDIAN
		target.push_back(p[7]);
		target.push_back(p[6]);
		target.push_back(p[5]);
		target.push_back(p[4]);
		target.push_back(p[3]);
		target.push_back(p[2]);
		target.push_back(p[1]);
		target.push_back(p[0]);
#else
	target.push_back(p[0]);
	target.push_back(p[1]);
	target.push_back(p[2]);
	target.push_back(p[3]);
	target.push_back(p[4]);
	target.push_back(p[5]);
	target.push_back(p[6]);
	target.push_back(p[7]);
#endif
}

inline void read8BytesToInteger(std::vector<unsigned char>::const_iterator & source, unsigned char * const p)
{
#ifdef IS_LITTLE_ENDIAN
	p[7] = *source++;
	p[6] = *source++;
	p[5] = *source++;
	p[4] = *source++;
	p[3] = *source++;
	p[2] = *source++;
	p[1] = *source++;
	p[0] = *source++;
#else
	p[0] = *source++;
	p[1] = *source++;
	p[2] = *source++;
	p[3] = *source++;
	p[4] = *source++;
	p[5] = *source++;
	p[6] = *source++;
	p[7] = *source++;
#endif
}

// Packing integer types will require honoring machine byte ordering
// and word size. Shorts are not smaller than chars, but the 
// the standard says nothing about them being more than 2 bytes. int's
// are no smaller than shorts, etc.
inline void writeTo(std::vector<unsigned char> & target, const unsigned short & source)
{
	write2BytesFromInteger(target, reinterpret_cast<const unsigned char * const>(&source));
}

inline void readFrom(std::vector<unsigned char>::const_iterator & source, unsigned short & target)
{
	read2BytesToInteger(source, reinterpret_cast<unsigned char * const>(&target));
}

// Packing integer types will require honoring machine byte ordering
// and word size. Shorts are not smaller than chars, but the 
// the standard says nothing about them being more than 2 bytes. int's
// are no smaller than shorts, etc.
inline void writeTo(std::vector<unsigned char> & target, const short & source)
{
	write2BytesFromInteger(target, reinterpret_cast<const unsigned char * const>(&source));
}

inline void readFrom(std::vector<unsigned char>::const_iterator & source, short & target)
{
	read2BytesToInteger(source, reinterpret_cast<unsigned char * const>(&target));
}

inline void writeTo(std::vector<unsigned char> & target, const unsigned int & source)
{
	write4BytesFromInt(target, reinterpret_cast<const unsigned char * const>(&source));
}

inline void readFrom(std::vector<unsigned char>::const_iterator & source, unsigned int & target)
{
	read4BytesToInteger(source, reinterpret_cast<unsigned char * const>(&target));
}

inline void writeTo(std::vector<unsigned char> & target, const int & source)
{
	write4BytesFromInt(target, reinterpret_cast<const unsigned char * const>(&source));
}

inline void readFrom(std::vector<unsigned char>::const_iterator & source, int & target)
{
	read4BytesToInteger(source, reinterpret_cast<unsigned char * const>(&target));
}

inline void writeTo(std::vector<unsigned char> & target, const unsigned long long & source)
{
	write8BytesFromInt(target, reinterpret_cast<const unsigned char * const>(&source));
}

inline void readFrom(std::vector<unsigned char>::const_iterator source, unsigned long long & target)
{
	read8BytesToInteger(source, reinterpret_cast<unsigned char * const>(&target));
}

inline void writeTo(std::vector<unsigned char> & target, const long long & source)
{
	write8BytesFromInt(target, reinterpret_cast<const unsigned char * const>(&source));
}

inline void readFrom(std::vector<unsigned char>::const_iterator source, long long & target)
{
	read8BytesToInteger(source, reinterpret_cast<unsigned char * const>(&target));
}

inline void writeTo(std::vector<unsigned char> & target, const float & source)
{
	write4BytesFromInt(target, reinterpret_cast<const unsigned char * const>(&source));
}

inline void readFrom(std::vector<unsigned char>::const_iterator source, float & target)
{
	read4BytesToInteger(source, reinterpret_cast<unsigned char * const>(&target));
}

inline void writeTo(std::vector<unsigned char> & target, const double & source)
{
	write8BytesFromInt(target, reinterpret_cast<const unsigned char * const>(&source));
}

inline void readFrom(std::vector<unsigned char>::const_iterator source, double & target)
{
	read8BytesToInteger(source, reinterpret_cast<unsigned char * const>(&target));
}

template<typename ValueType>
std::vector<unsigned char> & operator << (std::vector<unsigned char> & target, const ValueType & source)
{
	writeTo(target, source);
	return target;
}

template<typename ValueType>
std::vector<unsigned char>::const_iterator & operator >> (std::vector<unsigned char>::const_iterator & source, ValueType & target)
{
	readFrom(source, target);
	return source;
}

template<typename ValueType>
ValueType initializeFrom(std::vector<unsigned char>::const_iterator & source)
{
	ValueType result;
	source >> result;
	return result;
}

#endif//_INCLUDED_SerializationHelper_H
