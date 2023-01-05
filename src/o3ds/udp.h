#ifndef O3DS_UDP_SOCKET_H
#define O3DS_UDP_SOCKET_H

#include "socket_exception.h"
#include "socket_util.h"
#include "socket_address.h"

namespace O3DS
{
	class UdpSocket : public Socket
	{
	public:
		UdpSocket();
		UdpSocket(SOCKET s);
		~UdpSocket();

		virtual void CreateSocket() override;

		//! Set broadcast destination - stored in m_sendDest
		/*! Socket must be UDP */
		void     SetDestination(const char*, unsigned short port);

		//! Set broadcast destination - stored in m_sendDest
		/*! Socket must be UDP */
		void     SetDestination(struct sockaddr_in& in, unsigned short port);

		//! Send a datagram to the destination specified by SetDestination()
		/*! uses m_sendDest as desination
			Socket must be UDP
		*/
		void     SendTo(const void* data, size_t len);


		size_t GetDatagram(char* buf, int len);

		SocketAddress m_sendDest;

		SocketAddress m_from;

	};
}

#endif