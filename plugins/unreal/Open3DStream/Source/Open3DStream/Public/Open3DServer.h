#pragma once

#include "CoreMinimal.h"

#include "o3ds/base_server.h"

DECLARE_DELEGATE(FOnNngData);

#include <vector>

void InDataFunc(void *ptr, void *msg, size_t len);

class UServer
{
public:
	UServer();

	bool start(const char *url, const char *protocol);
	bool write(const char *data, size_t len);

	void inData(const char *msg, size_t len);

	O3DS::BaseServer *mServer;

	FOnNngData DataDelegate;

	std::vector<char> buffer;

	FCriticalSection mutex;

};

