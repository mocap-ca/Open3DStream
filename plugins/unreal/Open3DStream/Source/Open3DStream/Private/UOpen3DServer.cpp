#include "UOpen3DServer.h"
#include "o3ds/async_pair.h"
#include "o3ds/async_subscriber.h"
#include "SocketSubsystem.h"
#include "Interfaces/IPv4/IPv4Address.h"
#include "Common/UdpSocketBuilder.h"

#define LOCTEXT_NAMESPACE "Open3DStream"

void InDataFunc(void* ptr, void* data, size_t msg)
{
	static_cast<O3DSServer*>(ptr)->inData((const uint8*)data, msg);
}

O3DSServer::O3DSServer()
	: mServer(nullptr)
	, mTcp(nullptr)
	, mUdp(nullptr)
	, mUdpReceiver(nullptr)
	, mBuffer(nullptr)
	, mBufferSize(0)
	, mBytesToGet(0)
	, mPtr(0)
	, mTcpHasSync(false)
	, mGoodTime(0.0f)
	, mNoDataFlag(false)
{
}

O3DSServer::~O3DSServer()
{
	stop();
}

bool O3DSServer::start(const char* url, const char* protocol)
{
	stop();

	// NNG

	if (strncmp(protocol, "NNG Subscribe", 13) == 0)
	{
		mServer = new O3DS::AsyncSubscriber();
	}
	if (strncmp(protocol, "NNG Client", 10) == 0)
	{
		mServer = new O3DS::AsyncPairClient();
	}
	if (strncmp(protocol, "NNG Server", 10) == 0)
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
			std::string err = mServer->getError();
			this->error = FText::FromString(ANSI_TO_TCHAR(err.c_str()));
			stop();
			return false;
		}
	}

	// UDP

	if (strcmp(protocol, "UDP Server") == 0)
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

	// TCP

	if (strcmp(protocol, "TCP Client") == 0)
	{
		mTcpHasSync = false;

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

void O3DSServer::tick()
{
	uint32_t bucketSize;

	const unsigned char header[] = "\x00\xff\x03\xfeO3DS-START";

	if (!mNoDataFlag && FPlatformTime::Seconds() - mGoodTime > 1)
	{
		OnState.ExecuteIfBound(LOCTEXT("NoData", "No Data"));
		mNoDataFlag = true;
	}

	// Handle tcp - TODO: thread?
	if (mTcp)
	{
		while (1)
		{
			int32 read = 0;

			if (!mTcpHasSync)
			{
				mPtr = 0;
				mBytesToGet = 0;

				if (!ReadTcp(1))
					return;

				if (mBuffer[0] != 0x00) continue;

				bool ok = true;
				for (int i = 1; i < 14; i++)
				{
					if (!ReadTcp(1))
						return;
					if (mBuffer[i] != header[i]) {
						ok = false;  break;
					}
				}
				if (!ok) break;

				mTcpHasSync = true;

				if (!ReadTcp(4))
					return;

			}
			
			// Fill the buffer
			if (mBytesToGet)
			{
				if (!ReadTcp(0))
					return;
			}

			if (mPtr < 18)
			{
				return;
			}

			bucketSize = *(uint32_t*)(mBuffer + 14);

			if (mPtr == 18)
			{
				if (strncmp((char*)mBuffer, (char*)header, 14) != 0)
				{
					OnState.ExecuteIfBound(LOCTEXT("MalformedData", "Malformed Data"));
					mTcpHasSync = false;
				}
				// We have the header only, need to get the bucket
				if (!ReadTcp(bucketSize))
					return;
			}


			// Process
			TArray<uint8> Data;
			Data.Append((uint8*)(mBuffer+18), bucketSize);
			OnData.Execute(Data);
			OnState.ExecuteIfBound(LOCTEXT("Receiving Data", "Receiving Data"));

			mPtr = 0;

			ReadTcp(18);
		
		}
	}
}


bool O3DSServer::ReadTcp(size_t len)
{
	mBytesToGet += len;

	if (mBuffer == nullptr)
	{
		size_t sz = mBytesToGet < 4096 ? 4096 : mBytesToGet;
		mBuffer = (uint8*)malloc(mBytesToGet);
		mBufferSize = sz;
		mPtr = 0;
	}
	else
	{
		if (len + mPtr > mBufferSize)
		{
			mBuffer = (uint8*)realloc(mBuffer, mBytesToGet + mPtr);
			mBufferSize = mBytesToGet + mPtr;
		}		
	}

	int32 read = 0;
	if (!mTcp->Recv(mBuffer + mPtr, mBytesToGet, read))
	{
		OnState.ExecuteIfBound(LOCTEXT("Disconnected", "Disconnected"));
		return false;
	}

	if (read > 0)
	{
		mGoodTime = FPlatformTime::Seconds();
		mNoDataFlag = false;
	}

	mBytesToGet -= read;
	mPtr += read;
	return mBytesToGet == 0;
}