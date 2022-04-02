#ifndef O3DS_WEBSOCKET_CONNECTOR
#define O3DS_WEBSOCKET_CONNECTOR

#include "base_connector.h"
#include <websocketpp/client.hpp>
#include <websocketpp/server.hpp>
#include <boost/thread.hpp>

#include <websocketpp/config/asio_no_tls_client.hpp>
#include <websocketpp/config/asio_no_tls.hpp>

#include <set>

using websocketpp::connection_hdl;

namespace O3DS
{

	typedef websocketpp::client<websocketpp::config::asio_client> client;
	typedef websocketpp::server<websocketpp::config::asio> server;

	typedef websocketpp::config::asio_client::message_type::ptr client_message_ptr;
	typedef server::message_ptr server_message_ptr;

	class Websocket : public AsyncConnector
	{
	public:
		Websocket();
		boost::thread_group threads;

	};

	class WebsocketClient : public Websocket
	{
	public:
		WebsocketClient();
		~WebsocketClient();

		virtual bool start(const char* url) override;

		virtual bool write(const char* data, size_t len) override;

		virtual size_t read(char* data, size_t len) override;

		virtual size_t read(char** data, size_t* len) override;

		void onMessage(websocketpp::connection_hdl hdl, client_message_ptr msg);
		void onClose(websocketpp::connection_hdl hdl);
		void onFail(websocketpp::connection_hdl hdl);

		bool connect();

		client mClient;
		std::string mUrl;

	private:
		client::connection_ptr _con;
	};


	class WebsocketBroadcastServer : public Websocket
	{
	public:
		WebsocketBroadcastServer();

		virtual bool start(const char* url) override;

		virtual bool write(const char* data, size_t len) override;

		virtual size_t read(char* data, size_t len) override;

		virtual size_t read(char** data, size_t* len) override;

		void onOpen(connection_hdl hdl) {
			mConnections.insert(hdl);
		}

		void onClose(connection_hdl hdl) {
			mConnections.erase(hdl);
		}

		void onMessage(connection_hdl hdl, server::message_ptr msg) {
			for (auto it : mConnections) {
				mServer.send(it, msg);
			}
		}


		server mServer;

		typedef std::set<connection_hdl, std::owner_less<connection_hdl>> TConnectionList;

		TConnectionList mConnections;


	};

} // O3DS


#endif // O3DS_WEBSOCKET_CONNECTOR