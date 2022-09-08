#include "base_connector.h"

#include <string>
#include <string.h>
#include "nng/nng.h"

namespace O3DS
{
	Connector::Connector() : mState(NOTSTARTED) {};

	Connector::~Connector()
	{
	}

	const std::string& Connector::getError()
	{
		return mError;
	}

	void Connector::setError(const char* msg)
	{
		mError = msg;
		mState = Connector::STATE_ERROR;
	}

	void AsyncConnector::setFunc(void* ctx, InDataFunc f)
	{
		mContext = ctx;
		mInDataFunc = f;
	}

} // namespace
