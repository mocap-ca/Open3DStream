#ifndef O3DS_TCP_H
#define O3DS_TCP_H

#ifdef _WIN32
#  ifndef WIN32_LEAN_AND_MEAN
#    define WIN32_LEAN_AND_MEAN
#  endif
#  include <Ws2tcpip.h>
#  include <winsock2.h>
#  include "Wininet.h"
#  define SOCKET_WOULDBLOCK WSAEWOULDBLOCK
#  define SOCKET_CONNRESET  WSAECONNRESET
#  define SOCKET_NULL       NULL
#  define CLOSESOCKET       closesocket
#  define SOCKLENTYPE       int
#else
#  include <sys/socket.h>
#  include <sys/types.h>
#  include <netinet/in.h>
#  include <arpa/inet.h>
#  include <netdb.h>
#  include <errno.h>
#  include <unistd.h>
#  include <fcntl.h>

#  define SOCKET             int
#  define SOCKET_WOULDBLOCK  EWOULDBLOCK
#  define SOCKET_NULL        0
#  define INVALID_SOCKET     -1
#  define SOCKET_ERROR       -1
#  define CLOSESOCKET        close
#  define SOCKADDR           sockaddr
#  define SOCKLENTYPE        socklen_t
#endif

#include <string>
#include <vector>

namespace O3DS
{

	//! Thrown by Socket and HttpSocket
	class SocketException
	{
	public:
		//! Create a socket error without an error code from std::string message
		SocketException(std::string s);

		//! Create a socket error object with error code
		SocketException(std::string s, int err);

		//! Create a socket error object including host and port data
		SocketException(std::string s, int err, std::string host, int port);

		//! Destory
		inline ~SocketException() {};

		//! Return the error as a string
		std::string str();

		//! Return m_str
		std::string msg() { return m_str; }

		//! return m_err
		int error() { return m_err; }


	private:
		std::string m_str; //!< The error string
		int         m_err; //!< The error code
	};

	class TcpSocket
	{
	public:
		TcpSocket();
		TcpSocket(SOCKET s);
		~TcpSocket();

		// Socket Creation/Destruction
		void CreateSocket();    //!< Calls socket() - throws SocketException
		void DestroySocket();           //!< Calls closesocket() - throws SocketException

		static int GetError();

		void     SetBlocking(bool blocking);

		//! open a connection to the host/port
		void     Connect(const char* host, unsigned short port);

		//! open a connection to the host/port
		void     Connect(sockaddr_in* host, unsigned short port);

		void     Disconnect();         //!< shut down a connection
		void     DisconnectSend();     //!< shut down a connection for sending data
		void     DisconnectReceive();  //!< shut down a connection for reading data

		//! Bind the server to a port. 
		/*! Throws a SocketException if bind fails
			This method is called by listen, so likely does not need to be called directly
		 */
		void     Bind(unsigned short port);

		//! Listen for a connection.
		/*! Set the socket options for blocking, calles Bind then Listen.
			If blocking is false, function should return right away
			Throws a SocketException if Bind or Listen fails */
		void     Listen(int port, bool nonBlocking, int maxConnections);

		//! Accept an incoming connection.  
		/*! Will poll the listening socket and wait for one second for an incoming connection.  The
			reason for doing this rather than using a blocking socket is on unix, the accept will
			not unblock when the socket is destoryed, so putting this in a thread and checking the
			shutdown status is required.
		*/
		//! \retval true if a connection was made

		bool     Accept(SOCKET& s, int timeout = 0);


		void     Send(std::vector<char>& data);  //!< Puts data on the wire.
		void     Send(void* data, int len);  //!< Puts data on the wire

		//! Receive data
		/*! Will keep reading in data till TIMEOUT has been exceeded or connection is closed. */
		bool     Receive(std::vector<char>& data);

		//! Receive a specific length of data
		void     Receive(void* buf, int len, int* recd);

		//! Recieve a specific length of data, and keep going till its all read.
		void     Receive(void* buf, int len);

		//! Receive everything up to a token
		/*! Will keep reading in data till TIMEOUT has been exceed or the token is read */
		bool     ReceiveToToken(void* data, int len, const char token, int* bytesRead = NULL);

		//! The platform socket object
		SOCKET       m_socket;

		bool deleteOnClose;

#ifdef WIN32
		inline static bool wsok = false;
		static WSADATA ws;
		static void Cleanup() { WSACleanup(); }
#endif
	};

}



#endif