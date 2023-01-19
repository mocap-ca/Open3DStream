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

#ifndef O3DS_UDP_FRAGMENT_H
#define O3DS_UDP_FRAGMENT_H

#include <vector>
#include <stdint.h>
#include <map>

class UdpFragmenter
{
public:

	UdpFragmenter(const char* data, size_t sz, size_t inFragSize);
	~UdpFragmenter();

	void makeFragment(uint32_t id, uint32_t seq, std::vector<char> &out);


	size_t mFragmentSize;
	char*  mBuffer;
	size_t mBufferSize;
	size_t mFrames;
};


class UdpCombiner
{
public:
	UdpCombiner();
	~UdpCombiner();

	bool addFragment(const char* data, size_t sz);

	bool isComplete();

	size_t mFrameSize;
	char* mBuffer;
	size_t mBufferSize;
	size_t mFrames;

	std::vector<bool> mFound;
};

class UdpMapper
{
public:
	UdpCombiner& getCombiner(uint32_t);
	std::map<uint32_t, UdpCombiner> items;
	bool addFragment(const char* data, size_t sz);

	bool getFrame(std::vector<char> &out);
};






#endif