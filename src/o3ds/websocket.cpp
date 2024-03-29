#include "websocket.h"
#include <websocketpp/uri.hpp>
#include <boost/asio.hpp>


using websocketpp::lib::placeholders::_1;
using websocketpp::lib::placeholders::_2;

namespace O3DS
{

	// BASE

	Websocket::Websocket()
		: AsyncConnector()
		
	{}

	// CLIENT

	WebsocketClient::WebsocketClient() 
		: Websocket()
	{
		// Set logging to be pretty verbose (everything except message payloads)
		mClient.set_access_channels(websocketpp::log::alevel::all);
		mClient.clear_access_channels(websocketpp::log::alevel::frame_payload);

		mClient.set_message_handler(bind(&WebsocketClient::onMessage, this, ::_1, ::_2));

		mClient.set_close_handler(bind(&WebsocketClient::onClose, this, ::_1));

		mClient.set_fail_handler(bind(&WebsocketClient::onFail, this, ::_1));

		mClient.init_asio();
	}

	WebsocketClient::~WebsocketClient()
	{
	}

	void WebsocketClient::stop()
	{
		mClient.stop();
	}


	void WebsocketClient::onMessage(websocketpp::connection_hdl hdl, client_message_ptr msg)
	{
		if (mInDataFunc)
		{
			std::string message = msg->get_payload();
			mInDataFunc(mContext, (void*)message.c_str(), message.size());
		}
	}

	void WebsocketClient::onClose(websocketpp::connection_hdl hdl)
	{
		std::cout << "Closed" << std::endl;
	}

	void WebsocketClient::onFail(websocketpp::connection_hdl hdl)
	{
		std::cout << "Fail" << std::endl;
		std::this_thread::sleep_for(std::chrono::seconds(8));
		this->connect();
	}

	bool WebsocketClient::connect()
	{
		std::cout << "CONNECTING" << std::endl;

		websocketpp::lib::error_code errorCode;
		_con = mClient.get_connection(mUrl, errorCode);
		if (errorCode)
		{
			setError(errorCode.message().c_str());
			return false;
		}

		try
		{
			mClient.connect(_con);

			threads.create_thread([&]()
				{ 
					std::cout << "RUN" << std::endl;
					mClient.run();
					std::cout << "END" << std::endl;
				});

			return true;
		}
		catch (websocketpp::exception const& e) {
			std::cout << "ERROR" << std::endl;
			setError(e.what());
		}

		return false;
	}

	bool WebsocketClient::start(const char* url)
	{
		mUrl = url;
		connect();
		return true;
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

	void WebsocketBroadcastServer::stop()
	{
		mServer.stop_listening();
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
