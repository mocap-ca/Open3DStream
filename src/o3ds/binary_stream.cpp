/*
Open 3D Stream

Copyright 2022 Alastair Macleod

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include "binary_stream.h"

#include <algorithm>
#include <iostream>
#include <istream>
#include <string>
#include <iterator>

namespace O3DS
{
	BinaryStream::BinaryStream() : ptr(0), swaporder(false) {};

	BinaryStream::BinaryStream(uint8_t* data, size_t len)
		: std::vector<uint8_t>(data, data + len)
		, ptr(0)
		, swaporder(false)
	{}

	bool BinaryStream::done()
	{
		return ptr >= this->size();
	}

	void BinaryStream::clear()
	{
		std::vector<uint8_t>::clear();
		ptr = 0;
	}

	size_t BinaryStream::peek(uint8_t* data, size_t dataSize)
	{
		size_t count = std::min(dataSize, this->size() - ptr);
		memcpy(data, &this->at(ptr), count);
		return count;
	}

	void BinaryStream::get(void* buf, size_t len, const char* msg)
	{
		size_t sz = this->size();
		if (ptr + len > sz)
		{
			std::ostringstream oss;
			oss << "Attempt to read " << len << " bytes with " << (this->size() - ptr) << " bytes remaining";
			if (msg) oss << " while getting " << msg;
			ptr = this->size();
			memset(buf, 0, len);
			throw StreamException(oss.str().c_str());
		}
		memcpy(buf, &this->at(ptr), len);
		ptr += len;
	}

	void BinaryStream::push(void* buf, size_t len)
	{
		char* ptr = (char*)buf;
		for (size_t i = 0; i < len; i++)
			this->push_back(ptr[i]);
	}

	void BinaryStream::get(uint8_t& value, const char* msg)
	{
		if (ptr >= this->size())
		{
			std::string error("Attempt to a char read past the end of the buffer");
			if (msg) { error += " while getting "; error += msg; }
			value = -1;
			throw StreamException(error);
		}
		value = this->at(ptr++);
	}

	void BinaryStream::get(std::vector<uint8_t>& value, const char* msg)
	{
		uint16_t len;
		this->get(len);
		value.resize(len);
		for (uint32_t i = 0; i < len; i++)
		{
			if (ptr >= this->size())
			{
				std::string error("Read past end while getting data buffer");
				if (msg) { error += " while getting "; error += msg; }
				throw StreamException(error);
			}
			value[i] = this->at(ptr++);

		}
	}

	void BinaryStream::push(std::vector<uint8_t> value)
	{
		uint16_t len = (uint32_t)value.size();
		this->push(len);
		this->push((void*)value.data(), value.size());
	}

	void BinaryStream::get(std::string& value, const char* msg)
	{
		value = "";
		if (done())
		{
			std::string error("Attempt to read past end looking for string");
			if (msg) { error += " while getting "; error += msg; }
			throw StreamException(error);
		}
		uint8_t len = (uint8_t)this->at(ptr++);
		if (len + ptr > this->size())
		{
			std::string error("Incomplete string");
			if (msg) { error += " while getting "; error += msg; }
			throw StreamException(error);

		}
		for (uint8_t i = 0; i < len && ptr < this->size(); i++)
			value.push_back(this->at(ptr++));
	}

	void BinaryStream::push(std::string value)
	{
		this->push_back((char)value.size());
		std::copy(value.begin(), value.end(), std::back_inserter(*this));
	}

	void BinaryStream::get(float& value, const char* msg)
	{
		get(&value, sizeof(float), msg);
		if (swaporder) value = O3DS::swap(value);
	}

	void BinaryStream::get(double& value, const char* msg)
	{
		get(&value, sizeof(float), msg);
		if (swaporder) value = O3DS::swap(value);
	}

	void BinaryStream::get(uint32_t& value, const char* msg)
	{
		get(&value, sizeof(uint32_t), msg);
		if (swaporder) value = O3DS::swap(value);
	}

	void BinaryStream::get(uint16_t& value, const char* msg)
	{
		get(&value, sizeof(uint16_t), msg);
		if (swaporder) value = O3DS::swap(value);
	}

	void BinaryStream::get(uint64_t& value, const char* msg)
	{
		get(&value, sizeof(uint64_t), msg);
		if (swaporder) value = O3DS::swap(value);
	}

	void BinaryStream::dump()
	{
		for (int i = 0; i < this->size(); i++)
			std::cout << "  " << (int)this->at(i) << " ";
		std::cout << std::endl;
	}

	BinaryStream& operator << (BinaryStream& stream, float value) { stream.push(value); return stream; }
	BinaryStream& operator << (BinaryStream& stream, double value) { stream.push(value); return stream; }
	BinaryStream& operator << (BinaryStream& stream, std::string value) { stream.push(value); return stream; }
	BinaryStream& operator << (BinaryStream& stream, uint16_t value) { stream.push(value); return stream; }
	BinaryStream& operator << (BinaryStream& stream, uint32_t value) { stream.push(value); return stream; }
	BinaryStream& operator << (BinaryStream& stream, uint64_t value) { stream.push(value); return stream; }
	BinaryStream& operator << (BinaryStream& stream, uint8_t value) { stream.push(value); return stream; }
	BinaryStream& operator << (BinaryStream& stream, const std::vector<uint8_t>& value) { stream.push(value); return stream; }

	BinaryStream& operator >> (BinaryStream& stream, float& result) { stream.get(result); return stream; }
	BinaryStream& operator >> (BinaryStream& stream, double& result) { stream.get(result); return stream; }
	BinaryStream& operator >> (BinaryStream& stream, std::string& result) { stream.get(result); return stream; }
	BinaryStream& operator >> (BinaryStream& stream, uint16_t& result) { stream.get(result); return stream; }
	BinaryStream& operator >> (BinaryStream& stream, uint32_t& result) { stream.get(result); return stream; }
	BinaryStream& operator >> (BinaryStream& stream, uint64_t& result) { stream.get(result); return stream; }
	BinaryStream& operator >> (BinaryStream& stream, uint8_t& result) { stream.get(result); return stream; }
	BinaryStream& operator >> (BinaryStream& stream, std::vector<uint8_t>& result) { stream.get(result); return stream; }
}
