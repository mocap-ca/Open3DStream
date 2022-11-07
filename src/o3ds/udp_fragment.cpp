#include "udp_fragment.h"

#include <vector>

#define HEADERSIZE 16


UdpFragmenter::UdpFragmenter(const char* data, size_t sz, size_t inFragSize)
	: mFragmentSize(inFragSize)
	, mBuffer((char*)malloc(sz))
	, mBufferSize(sz)
	, mFrames((sz + inFragSize - 1) / inFragSize)
{
	if (mBuffer != nullptr)
		memcpy(mBuffer, data, sz);
	else
		mBufferSize = 0;
}

UdpFragmenter::~UdpFragmenter()
{
	if (mBuffer) free(mBuffer);
}

void UdpFragmenter::makeFragment(
	uint32_t inId,
	uint32_t inSeq,
	std::vector<char>& out
)
{
	out.clear();

	uint32_t heading[4];
	heading[0] = inId;
	heading[1] = inSeq;
	heading[2] = mBufferSize;
	heading[3] = mFragmentSize;

	char* ptr = (char*)heading;

	out.insert(out.end(), ptr, ptr + HEADERSIZE);

	ptr = mBuffer + inSeq * mFragmentSize;

	if (inSeq == mFrames - 1) {
		size_t sz = mBufferSize - (mFrames - 1) * mFragmentSize;
		out.insert(out.end(), ptr, ptr + sz);
	}
	else {
		out.insert(out.end(), ptr, ptr + mFragmentSize);
	}	

}

UdpCombiner::UdpCombiner()
	: mFrameSize(0)
	, mBuffer(0)
	, mBufferSize(0)
	, mFrames(0)
{
}

UdpCombiner::~UdpCombiner()
{
	if (mBuffer) free(mBuffer);
}

bool UdpCombiner::addFragment(const char* data, size_t sz)
{
	if (sz < HEADERSIZE) return false;
	if (sz > 65511) return false;

	uint32_t* heading = (uint32_t*)data;

	uint32_t seq      = heading[1];
	uint32_t bufSz    = heading[2];
	uint32_t fragSize = heading[3];

	int frames = (bufSz + fragSize - 1) / fragSize;

	if (seq >= frames)
		return false;

	if (seq * (fragSize-1) > bufSz)
		return false;
	
	if (mBufferSize == 0)
	{
		mFound.assign(frames, false);
		mBuffer = (char*)malloc(bufSz);
		mBufferSize = bufSz;
	}
	else
	{
		if (mBufferSize != bufSz)
			return false;
	}

	mFound[seq] = true;

	if (seq != frames - 1 && sz != fragSize + HEADERSIZE) {
		return false;
	}

	if (seq == frames - 1)
	{
		size_t sz = bufSz - (frames - 1) * fragSize + HEADERSIZE;
	}

	memcpy(mBuffer + seq * fragSize, data + HEADERSIZE, sz - HEADERSIZE);

	return true;

}

bool UdpCombiner::isComplete()
{
	for (auto& i : mFound)
	{
		if (!i) return false;
	}
	return true;
}

UdpCombiner& UdpMapper::getCombiner(uint32_t id)
{
	auto pos = items.find(id);
	if (pos != items.end())
	{
		return pos->second;
	}

	items.emplace(id, UdpCombiner());
	return items[id];

}

bool UdpMapper::addFragment(const char* data, size_t sz)
{
	if (sz < HEADERSIZE) return false;
	if (sz > 65511) return false;

	uint32_t* heading = (uint32_t*)data;

	uint32_t frameId = heading[0];
	uint32_t fragmentId = heading[1];

	//if (fragmentId > 40)
    // return false;

	this->getCombiner(frameId).addFragment(data, sz);
}

bool UdpMapper::getFrame(std::vector<char> &out)
{
	for (auto &i : items)
	{
		if (i.second.isComplete())
		{
			out.insert(out.end(), i.second.mBuffer, i.second.mBuffer + i.second.mBufferSize);

			for(auto &j = items.begin(); j != items.end();)
			{ 
				if (j->first <= i.first) {
					j = items.erase(j);
				}
				else {
					j++;
				}
			}

			return true;
		}
	}
	return false;
}

