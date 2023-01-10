#include <stdio.h>
#include <vector>

#include "o3ds/udp.h"
#include "o3ds/xsens_parser.h"
#include "o3ds/binary_stream.h"
#include "o3ds/model.h"
#include "o3ds/async_publisher.h"
#include "o3ds/getTime.h"

#include "windows.h" 

// https://www.xsens.com/hubfs/Downloads/Manuals/MVN_real-time_network_streaming_protocol_specification.pdf

bool running;

BOOL WINAPI CtrlHandler(DWORD fdwCtrlType)
{
	if (fdwCtrlType == CTRL_C_EVENT)
	{
		running = false;
		return true;
	}

	return false;
}

int main(int argc, char *argv[])
{
	running = true;

	int frame = 0;

	SetConsoleCtrlHandler(CtrlHandler, TRUE);

	O3DS::UdpSocket udp;

	O3DS::SubjectList subjects;
	O3DS::AsyncPublisher *connector = new O3DS::AsyncPublisher();

	/*
	0  6 bytes ID String 
	6  4 bytes sample counter  
	10 1 byte datagram counter
	11 1 byte number of items
	12 4 bytes time code
	16 1 byte character ID
	17 1 byte number of body segments – from MVN 2019
	18 1 byte number of props – from MVN 2019
	19 1 byte number of finger tracking data segments – from MVN 2019
	20 2 bytes reserved for future use
	22 2 bytes size of payload
	24
	*/

	try
	{

		if (!connector->start("tcp://127.0.0.1:3001"))
		{
			printf(connector->getError().c_str());
			return 1;
		}


		udp.CreateSocket();

		udp.Bind(9763);

		udp.SetBlocking(true);

		char buf[10240];

		std::string system_name;

		int ret;

		O3DS::XSENS::Parser parser;

		std::vector<char> buffer;

		while (running)
		{
			size_t ret = udp.GetDatagram(buf, 10240);
			if (ret)
			{
				printf("\nData: %zd - ", ret);

				if (parser.parse(buf, ret))
				{
					if (frame++ % 100 == 0)
					{
						ret = parser.subjects.Serialize(buffer, GetTime());
					}
					else
					{
						ret = parser.subjects.SerializeUpdate(buffer, GetTime());
					}

					if (ret > 0)
					{
						if (!connector->write((const char*)&buffer[0], ret))
						{
							printf("Could not send: %s\n", connector->getError().c_str());
						}
					}
				}
				
			}
		}
	}
	catch (O3DS::SocketException e)
	{
		printf("Error: %s", e.str().c_str());
		return 1;
	}
	
	return 0;
}