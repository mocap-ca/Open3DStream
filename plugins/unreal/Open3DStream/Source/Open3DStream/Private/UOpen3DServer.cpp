#include "UOpen3DServer.h"
#include "o3ds/async_pair.h"
#include "o3ds/async_subscriber.h"
#include "SocketSubsystem.h"
#include "Interfaces/IPv4/IPv4Address.h"
#include "Common/UdpSocketBuilder.h"


void InDataFunc(void* ptr, void* data, size_t msg)
{
	static_cast<O3DSServer*>(ptr)->inData((const uint8*)data, msg);
}

O3DSServer::O3DSServer()
{
	mServer = nullptr;
	mTcp = nullptr;
	mUdp = nullptr;
	mUdpReceiver = nullptr;
}

O3DSServer::~O3DSServer()
{
	stop();
}

bool O3DSServer::start(const char* url, const char* protocol)
{
	stop();

	// NNG

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

	if (mServer)
	{
		mServer->setFunc(this, InDataFunc);
		if (mServer->start(url)) {
			return true;
		}
		else {
			stop();
			return false;
		}
	}

	// TCP

	if (strcmp(protocol, "UDP") == 0)
	{
		TSharedRef<FInternetAddr> addr = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();
		addr->SetAnyAddress();
		FString surl(url);
		addr->SetPort(FCString::Atoi(*surl));

		FString parseme(url);

		if (parseme.StartsWith("udp://")) {
			parseme = parseme.Right(parseme.Len() - 6);
		}

		int32 pos;
		if (parseme.FindChar(':', pos))
		{
			FString ip = parseme.Left(pos);
			FString port = parseme.Right(parseme.Len() - pos - 1);

			FIPv4Address address;
			FIPv4Address::Parse(ip, address);

			FIPv4Endpoint Endpoint(address, FCString::Atoi(*port));

			mUdp = FUdpSocketBuilder(TEXT("O3dsUdp"))
				.AsNonBlocking()
				.AsReusable()
				.BoundToEndpoint(Endpoint)
				.WithReceiveBufferSize(65507u);


			FTimespan ThreadWaitTime = FTimespan::FromMilliseconds(100);
			FString ThreadName = FString::Printf(TEXT("O3DS UDP Receiver"));

			if (mUdpReceiver) delete mUdpReceiver;
			mUdpReceiver = new FUdpSocketReceiver(mUdp, ThreadWaitTime, *ThreadName);

			mUdpReceiver->OnDataReceived().BindLambda([this](const FArrayReaderPtr& DataPtr, const FIPv4Endpoint& Endpoint)
				{
					TArray<uint8> Data;
					Data.AddUninitialized(DataPtr->TotalSize());
					DataPtr->Serialize(Data.GetData(), DataPtr->TotalSize());
					if (OnData.IsBound()) { OnData.Execute(Data); }
				});

			mUdpReceiver->Start();

		}
	}

	if (strcmp(protocol, "TCP") == 0)
	{
		mTcp = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateSocket(NAME_Stream, TEXT("default"), false);

		FString parseme(url);

		if (parseme.StartsWith("tcp://")) {
			parseme = parseme.Right(parseme.Len() - 6);
		}

		int32 pos;
		if (parseme.FindChar(':', pos))
		{
			FString ip = parseme.Left(pos);
			FString port = parseme.Right(parseme.Len() - pos - 1);

			FIPv4Address address;
			FIPv4Address::Parse(ip, address);

			TSharedRef<FInternetAddr> addr = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();
			addr->SetIp(address.Value);
			addr->SetPort(FCString::Atoi(*port));

			if (!mTcp->Connect(*addr))
			{
				UE_LOG(LogTemp, Warning, TEXT("Could not connect "));
				mTcp->Close();
				delete mTcp;
				mTcp = nullptr;
				return false;
			}

			mTcp->SetNonBlocking();
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Invalid address - should be eg: 127.0.0.1:5000"));
			mTcp->Close();
			delete mTcp;
			mTcp = nullptr;
			return false;
		}
	}

	return true;
}

void O3DSServer::stop()
{
	if (mServer)
	{
		mServer->stop();
		delete mServer;
		mServer = nullptr;
	}

	if (mUdp)
	{
		mUdpReceiver->Stop();
		delete mUdpReceiver;
		mUdpReceiver = nullptr;

		mUdp->Close();
		ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->DestroySocket(mUdp);
		mUdp = nullptr;
	}

	if (mTcp)
	{
		mTcp->Close();
		ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->DestroySocket(mTcp);
		mTcp = nullptr;
	}
}

bool O3DSServer::write(const char *msg, size_t len)
{
	if (!mServer) return false;
	return mServer->write(msg, len);
}

void O3DSServer::inData(const uint8 *msg, size_t len)
{
	// NNG Data
	TArray<uint8> Data;
	Data.Append((uint8*)msg, len);
	if (OnData.IsBound()) { OnData.Execute(Data); }
}

