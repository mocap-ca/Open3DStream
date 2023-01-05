#ifndef O3DS_SOCKET_ADDRESS_H
#define O3DS_SOCKET_ADDRESS_H


#include "socket_util.h"
#include <string>

namespace O3DS
{

	class SocketAddress
	{
	public:
		SocketAddress();
		SocketAddress(SOCKET s);
		SocketAddress(struct sockaddr_in);

		void        Clear();
		void        Set(unsigned long addr, unsigned short port);
		std::string GetName();

		struct sockaddr_in m_address;

		operator sockaddr* () { return (sockaddr*)&m_address; }
	};

}

#endif