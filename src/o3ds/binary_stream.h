#ifndef O3DS_BINARY_STREAM_H
#define O3DS_BINARY_STREAM_H

#include <string>
#include <cstdint>
#include <sstream>
#include <iostream>
#include <vector>

namespace O3DS
{
	inline uint64_t swap(uint64_t v) {
#if _WIN32
		return _byteswap_uint64(v);
#else
		return __builtin_bswap64(v);
#endif
	}

	inline uint32_t swap(uint32_t v) {
#if _WIN32
		return _byteswap_ulong(v);
#else
		return __builtin_bswap32(v);
#endif
	}

	inline uint16_t swap(uint16_t v) {
#if _WIN32
		return _byteswap_ushort(v);
#else
		return __builtin_bswap16(v);
#endif
	}

	inline float swap(float v)
	{
		uint32_t ref = *(uint32_t*)&v;
		uint32_t s = swap(ref);
		return *(float*)&s;
	}

	inline double swap(double v)
	{
		uint64_t ref = *(uint64_t*)&v;
		uint64_t s = swap(ref);
		return *(double*)&s;
	}


	class StreamException : public std::string {
	public:
		StreamException(const char* msg)
			: std::string(msg)
		{}

		StreamException(std::string &msg)
			: std::string(msg)
		{}
	};

	class BinaryStream : public std::vector<uint8_t>
	{
		//! Holds a character array and allows traversing and popping typed values from it
	public:
		BinaryStream();

		BinaryStream(uint8_t* data, size_t len);

		size_t ptr;

		bool swaporder;

		bool done();
		std::vector<uint8_t>::iterator pos() { return begin() + ptr; }

		void clear();
		size_t peek(uint8_t* data, size_t dataSize);
	
		// GENERIC
		void get(void* buf, size_t len, const char* msg = 0);
		void push(void* buf, size_t len);

		// FLOAT
		void get(float& value, const char* msg = 0);
		void push(float value) { push(&value, sizeof(float)); }

		// DOUBLE
		void get(double& value, const char* msg = 0);
		void push(double value) { push(&value, sizeof(double)); }

		//  UNSIGNED 16 bit int
		void get(uint16_t& value, const char* msg = 0);
		void push(uint16_t value) { push(&value, sizeof(uint16_t)); }

		//  UNSIGNED 32 bit int
		void get(uint32_t& value, const char* msg = 0);
		void push(uint32_t value) { push(&value, sizeof(uint32_t)); }

		//  UNSIGNED 64 bit int
		void get(uint64_t& value, const char* msg = 0);
		void push(uint64_t value) { push(&value, sizeof(uint64_t)); }

		//  char
		void get(uint8_t& value, const char* msg = 0);
		void push(uint8_t value) { this->push_back(value); }

		// RAW
		void get(std::vector<uint8_t>& value, const char* msg = 0);
		void push(std::vector<uint8_t> value);

		// STRING
		void get(std::string& value, const char* msg = 0);
		void push(std::string value);

		// Dump to std::cout
		void dump();
	};

	BinaryStream& operator << (BinaryStream& stream, float value);
	BinaryStream& operator << (BinaryStream& stream, double value);
	BinaryStream& operator << (BinaryStream& stream, std::string value);
	BinaryStream& operator << (BinaryStream& stream, uint32_t value);
	BinaryStream& operator << (BinaryStream& stream, uint16_t value);
	BinaryStream& operator << (BinaryStream& stream, uint64_t value);
	BinaryStream& operator << (BinaryStream& stream, uint8_t value);
	BinaryStream& operator << (BinaryStream& stream, const std::vector<uint8_t>& value);

	BinaryStream& operator >> (BinaryStream& stream, float& result);
	BinaryStream& operator >> (BinaryStream& stream, double& result);
	BinaryStream& operator >> (BinaryStream& stream, std::string& result);
	BinaryStream& operator >> (BinaryStream& stream, uint16_t& result);
	BinaryStream& operator >> (BinaryStream& stream, uint32_t& result);
	BinaryStream& operator >> (BinaryStream& stream, uint64_t& result);
	BinaryStream& operator >> (BinaryStream& stream, uint8_t& result);
	BinaryStream& operator >> (BinaryStream& stream, std::vector<uint8_t>& value);

}

#endif // BINARY_STREAM_H
