#pragma once

#include "Engine/Engine.h"
#include "CoreMinimal.h"

#include "Runtime/Sockets/Public/Sockets.h"
#include "Runtime/Networking/Public/Interfaces/IPv4/IPv4Address.h"
#include "Runtime/Networking/Public/Interfaces/IPv4/IPv4Endpoint.h"
#include "Common/UdpSocketReceiver.h"

#include "Open3DTcpThread.h"
#include "Open3DBuffer.h"

class OPEN3DSTREAM_API Open3dTCP
{
public:
	Open3dTCP(Open3DBuffer* ref) 
		: TcpThread(nullptr)
		, buffer(ref) {};

	~Open3dTCP()
	{
		if (TcpThread)
			TcpThread->Stop();
	}

	void Start(int port)
	{
		this->TcpThread = new O3DS_TcpThread(port);
		this->TcpThread->DataDelegate.BindRaw(this, &Open3dTCP::OnTcpData);
	}

	bool OnTcpData(FSocket *);

	O3DS_TcpThread*     TcpThread;
	Open3DBuffer*       buffer;
	uint8               temp_buffer[1024 * 12];


};