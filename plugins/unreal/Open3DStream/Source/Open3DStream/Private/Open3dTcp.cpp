#include "Open3dTcp.h"

bool Open3dTCP::OnTcpData(FSocket *TcpSource)
{
	// Returns true if the is no data or the data was processed okay
	// Returns false if the data is wrong and the connection is closed

	int32 bytesread = 0;

	while (1)
	{
		TcpSource->Recv(temp_buffer, 1024 * 12, bytesread, ESocketReceiveFlags::None);

		if (bytesread < 1)
			return true;

		buffer->push(temp_buffer, bytesread);

		size_t blocksize = 0;

		uint8 *ptr = 0;

		while (1)
		{
			blocksize = buffer->pull(&ptr);

			if (blocksize == 0)
				break;

			if (blocksize == (size_t)-1)
			{
				TcpSource->Close();
				return false;
			}

			buffer->punt();
		}
	}
	return true;
}
