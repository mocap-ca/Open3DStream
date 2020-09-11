#pragma once

#include "CoreMinimal.h"

#include "o3ds/async_subscriber.h"

DECLARE_DELEGATE(FOnNngData);

#include <vector>

class USubscriber : public O3DS::AsyncSubscriber
{
public:
	USubscriber();

	void in_data(const char *msg, size_t len) override;
	void in_pipe() override;

	FOnNngData DataDelegate;

	std::vector<char> buffer;

	FCriticalSection mutex;

};

