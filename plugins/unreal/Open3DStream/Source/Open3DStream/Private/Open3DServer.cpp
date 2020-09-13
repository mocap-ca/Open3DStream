#include "Open3DServer.h"
#include "o3ds/async_pair.h"
#include "o3ds/async_subscriber.h"

void InDataFunc(void *ptr, void *data, size_t msg)
{
	static_cast<UServer*>(ptr)->inData((const char*)data, msg);
}

UServer::UServer()
	: mServer(nullptr)
{}

bool UServer::start(const char *url, const char *protocol)
{
	if (mServer)
	{
		delete(mServer);
		mServer = nullptr;
			
	}
	if (strcmp(protocol, "Subscribe") == 0)
	{
		mServer = new O3DS::AsyncSubscriber();
	}
	if (strcmp(protocol, "Client") == 0)
	{
		mServer = new O3DS::AsyncPairClient();
	}
	if (strcmp(protocol, "Server") == 0)
	{
		mServer = new O3DS::AsyncPairServer();
	}

	if (!mServer)
		return false;

	mServer->setFunc(this, InDataFunc);
	mServer->start(url);

	return false;
}

void UServer::stop()
{
	if (!mServer) return;
	delete mServer;
	mServer = nullptr;
}

bool UServer::write(const char *msg, size_t len)
{
	if (!mServer) return false;
	return mServer->write(msg, len);
}

void UServer::inData(const char *msg, size_t len)
{
	mutex.Lock();
	buffer.resize(len);
	buffer.assign(msg, msg + len);
	mutex.Unlock();

	DataDelegate.ExecuteIfBound();
}

