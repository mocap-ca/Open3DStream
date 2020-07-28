#ifndef OPEN3DSTREAMBUFFER_H
#define OPEN3DSTREAMBUFFER_H

#include <vector>

//#pragma optimize( "", off )

class Open3DBuffer
{
public:

	void push(uint8 *ptr, size_t len)
	{
		data.insert(data.end(), ptr, ptr + len);
	}

	size_t pull(uint8 **result)
	{
		if (data.size() < 8) return 0;

		int32_t len;

		uint8 *ptr = (uint8*)&len;

		std::copy(data.begin()+4, data.begin() + 8, ptr);

		if (data[0] != 3 || data[1] != 2)
			return (size_t)-1;

		if (data.size() < len + 8)
			return 0;

		*result = &*(data.begin() + 8);

		work_ptr = data.begin() + 8 + len;

		return len;
	}

	bool punt()
	{
		auto a = data.begin();
		auto len = work_ptr - a;

		data.erase(data.begin(), work_ptr);
		work_ptr = data.begin();
		return true;

	}

	std::vector<uint8> data;

	std::vector<uint8>::iterator work_ptr;



};


//#pragma optimize( "", on )


#endif