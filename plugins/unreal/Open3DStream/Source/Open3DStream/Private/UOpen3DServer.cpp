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
	, mPtr(0)
	, mGoodTime(0.0f)
	, mNoDataFlag(false)
	, mState(eState::SYNC)
{
}

O3DSServer::~O3DSServer()
{
	stop();
}

bool O3DSServer::start(FText Url, FText Protocol )
{
	stop();

	const char* surl = TCHAR_TO_ANSI(*Url.ToString());
	const char* sprotocol = TCHAR_TO_ANSI(*Protocol.ToString());

	FText::Format(LOCTEXT("ConnectingString", "Connecting {0}"), Protocol);

	// NNG
	if (strncmp(sprotocol, "NNG Subscribe", 13) == 0)
	{
		mServer = new O3DS::AsyncSubscriber();
	}
	if (strncmp(sprotocol, "NNG Client", 10) == 0)
	{
		mServer = new O3DS::AsyncPairClient();
	}
	if (strncmp(sprotocol, "NNG Server", 10) == 0)
	{
		mServer = new O3DS::AsyncPairServer();
	}

	if (mServer)
	{
		mServer->setFunc(this, InDataFunc);
		if (mServer->start(surl)) {
			return true;
		}
		else {
			std::string err = mServer->getError();
			OnState.ExecuteIfBound(FText::FromString(ANSI_TO_TCHAR(err.c_str())), true);
			stop();
			return false;
		}
	}

	// UDP

	if (strcmp(sprotocol, "UDP Server") == 0)
	{
		FString parseme(surl);

		if (parseme.StartsWith("udp://")) {
			parseme = parseme.Right(parseme.Len() - 6);
		}

		if (parseme.StartsWith("tcp://")) {
			OnState.ExecuteIfBound(LOCTEXT("InvalidAddressUdpPrefix", "Invalid Address (TCP)"), true);
			return false;
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

			if (mUdp == nullptr) {
				OnState.ExecuteIfBound(LOCTEXT("InvalidUDP", "Could not create udp socket"), true);
				return false;
			}

			FTimespan ThreadWaitTime = FTimespan::FromMilliseconds(100);
			FString ThreadName = FString::Printf(TEXT("O3DS UDP Receiver"));

			if (mUdpReceiver) delete mUdpReceiver;
			mUdpReceiver = new FUdpSocketReceiver(mUdp, ThreadWaitTime, *ThreadName);

			mUdpReceiver->OnDataReceived().BindLambda([this](const FArrayReaderPtr& DataPtr, const FIPv4Endpoint& Endpoint)
				{
					TArray<uint8> Data;
					Data.AddUninitialized(DataPtr->TotalSize());
					DataPtr->Serialize(Data.GetData(), DataPtr->TotalSize());
					mUdpMapper.addFragment((const char*)Data.GetData(), Data.Num());
					std::vector<char> buf;
					if (mUdpMapper.getFrame(buf))
					{
						if (OnData.IsBound()) { OnData.Execute(TArray<uint8>((uint8*)buf.data(), buf.size())); }
					}
				});

			mUdpReceiver->Start();

		}
	}

	// TCP

	if (strcmp(sprotocol, "TCP Client") == 0)
	{
		mState = eState::SYNC;

		mTcp = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateSocket(NAME_Stream, TEXT("default"), false);

		FString parseme(surl);

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
				OnState.ExecuteIfBound(LOCTEXT("NotConnected", "TCP Not Connected"), true);
				mTcp->Close();
				delete mTcp;
				mTcp = nullptr;
				return false;
			}

			mTcp->SetNonBlocking();
			OnState.ExecuteIfBound(LOCTEXT("TCPConnected", "TCP Connected"), false);
		}
		else
		{
			OnState.ExecuteIfBound(LOCTEXT("InvalidAddress", "Invalid Address"), true);
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
		OnState.ExecuteIfBound(LOCTEXT("NoData", "No Data"), true);
		mNoDataFlag = true;
	}

	// Handle tcp - TODO: thread?
	if (mTcp)
	{
		while (1)
		{
			int32 read = 0;

			if (mState == eState::SYNC)
			{
				mPtr = 0;

				if (!ReadTcp(1))
					return;

				if (mBuffer[0] != 0x00) continue;

				bool ok = true;
				for (int i = 1; i < 14; i++)
				{
					if (!ReadTcp(i+1))
						return;
					if (mBuffer[i] != header[i]) {
						ok = false;  break;
					}
				}
				if (!ok) break;

				mState = eState::HEADER;
			}

			if (mState == eState::HEADER)
			{
				while (mPtr < 18)
				{
					if (!ReadTcp(18))
						return;
				}

				if (strncmp((char*)mBuffer, (char*)header, 14) != 0)
				{
					OnState.ExecuteIfBound(LOCTEXT("MalformedData", "Malformed Data"), true);
					mState = eState::SYNC;
					continue;
				}
				mState = eState::DATA;
			}

			if (mState == eState::DATA)
			{
				bucketSize = *(uint32_t*)(mBuffer + 14);

				if (bucketSize > 1024 * 50)
				{
					OnState.ExecuteIfBound(LOCTEXT("MalformedData", "Malformed Data"), true);
					mState = eState::SYNC;
					continue;
				}

				while (mPtr < bucketSize + 18)
				{
					if (!ReadTcp(bucketSize + 18))
						return;
				}


				// Process
				TArray<uint8> Data;
				Data.Append((uint8*)(mBuffer + 18), bucketSize);
				OnData.Execute(Data);
				OnState.ExecuteIfBound(LOCTEXT("Receiving Data", "Receiving Data"), false);

				mState = eState::HEADER;

				mPtr = 0;
			}
		}
	}
}


bool O3DSServer::ReadTcp(size_t len)
{
	// Keep reading util the buffer is len bytes in size.
	// Return false if no data was read so we can exit the tick.

	if (len <= mPtr)
		return true;

	if (mBuffer == nullptr)
	{
		size_t sz = len < 4096 ? 4096 : len;
		mBuffer = (uint8*)malloc(sz);
		mBufferSize = sz;
		mPtr = 0;
	}
	else
	{
		if (len + mPtr > mBufferSize)
		{
			mBuffer = (uint8*)realloc(mBuffer, len + mPtr);
			mBufferSize = len + mPtr;
		}		
	}

	int32 read = 0;
	if (!mTcp->Recv(mBuffer + mPtr, len - mPtr, read))
	{
		OnState.ExecuteIfBound(LOCTEXT("TCPError", "TCP Error"), true);
		return false;
	}

	if (read > 0)
	{
		mGoodTime = FPlatformTime::Seconds();
		mNoDataFlag = false;
	}

	mPtr += read;
	return read != 0;
}
