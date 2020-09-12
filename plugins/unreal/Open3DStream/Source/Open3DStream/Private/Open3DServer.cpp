#include "Open3DServer.h"
#include "o3ds/async_pair.h"

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
	if (strcmp(protocol, "Pair-Client") == 0)
	{
		mServer = new O3DS::AsyncPair();
		mServer->setFunc(this, InDataFunc);
		mServer->connect(url);
		return true;
	}
	if (strcmp(protocol, "Pair-Server") == 0)
	{
		mServer = new O3DS::AsyncPair();
		mServer->setFunc(this, InDataFunc);
		mServer->listen(url);
		return true;
	}

	return false;
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

