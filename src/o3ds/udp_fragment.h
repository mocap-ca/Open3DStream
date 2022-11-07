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