#include "websocket.h"
#include <websocketpp/uri.hpp>
#include <boost/asio.hpp>


using websocketpp::lib::placeholders::_1;
using websocketpp::lib::placeholders::_2;

namespace O3DS
{


	void WebsocketClient::onMessage(websocketpp::connection_hdl hdl, client_message_ptr msg)
	{
		std::cout << "on_message called with hdl: " << hdl.lock().get() << std::endl;
		/*
		if (mInDataFunc)
		{
			std::string message = msg->get_payload();
			mInDataFunc(mContext, (void*)message.c_str(), message.size());
		}*/
	}

	// BASE

	Websocket::Websocket()
		: AsyncConnector()
		, mContext(nullptr)
		, mInDataFunc(nullptr)
	{

	}

	void Websocket::setFunc(void *ctx, inDataFunc f)
	{
		mContext = ctx;
		mInDataFunc = (void*)f;
	}

	// CLIENT

	WebsocketClient::WebsocketClient() : Websocket()
	{
		// Set logging to be pretty verbose (everything except message payloads)
		mClient.set_access_channels(websocketpp::log::alevel::all);
		mClient.clear_access_channels(websocketpp::log::alevel::frame_payload);

		mClient.set_message_handler(bind(&WebsocketClient::onMessage, this, ::_1, ::_2));

		mClient.init_asio();
	}

	bool WebsocketClient::start(const char* url)
	{
		try
		{
			websocketpp::lib::error_code errorCode;
			client::connection_ptr con = mClient.get_connection(url, errorCode);
			if (errorCode)
			{
				setError(errorCode.message().c_str());
				return false;
			}

			mClient.connect(con);

			for (int n = 0; n < 2; ++n) {
				threads.create_thread([&] { mClient.run(); });
			}

			return true;
		}
		catch (websocketpp::exception const& e) {
			setError(e.what());
		}

		

		return false;
	}

	bool WebsocketClient::write(const char* data, size_t len)
	{
		return false;
	}

	size_t WebsocketClient::read(char* data, size_t len)
	{
		return 0;
	}

	size_t WebsocketClient::read(char** data, size_t* len)
	{
		return 0;
	}

	// Websocket Broadcast Server

	WebsocketBroadcastServer::WebsocketBroadcastServer() : Websocket()
	{
		mServer.set_access_channels(websocketpp::log::alevel::all);
		mServer.clear_access_channels(websocketpp::log::alevel::frame_payload);
		mServer.init_asio();

		mServer.set_open_handler(bind(&WebsocketBroadcastServer::onOpen, this, ::_1));
		mServer.set_close_handler(bind(&WebsocketBroadcastServer::onClose, this, ::_1));
		mServer.set_message_handler(bind(&WebsocketBroadcastServer::onMessage, this, ::_1, ::_2));
	}

	bool WebsocketBroadcastServer::start(const char* url)
	{
		try {
			std::string surl(url);
			websocketpp::uri ref(surl);

			if (!ref.get_valid())
			{
				setError("Invalid url");
				return false;
			}

			boost::system::error_code ec;
			boost::asio::ip::address ip_address = boost::asio::ip::address::from_string(url, ec);

			std::cout << "IP: " << ip_address << "   port: " << ref.get_port() << std::endl;
			mServer.listen(ref.get_port());

			mServer.start_accept();

			for (int n = 0; n < 2; ++n) {
				threads.create_thread([&] { mServer.run(); });
			}


			return true;
		}
		catch (websocketpp::exception const& e) {
			setError(e.what());
		}
		return false;
	}

	bool WebsocketBroadcastServer::write(const char* data, size_t len)
	{
		for (auto it : mConnections) {
			try
			{ 
				websocketpp::lib::error_code err;
				mServer.send(it, data, len, websocketpp::frame::opcode::binary, err);
			}
			catch (websocketpp::exception const& e) {
				setError(e.what());
			}
		}
		return true;
	}

	size_t WebsocketBroadcastServer::read(char* data, size_t len)
	{
		return 0;
	}

	size_t WebsocketBroadcastServer::read(char** data, size_t* len)
	{
		return 0;
	}


}
