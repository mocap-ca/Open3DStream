

#include "schema_generated.h"
using namespace O3DS::Data;

#include <iostream>
#include <chrono>
#include <thread>

#include "fbxloader.h"
#include "o3ds/model.h"
#include "o3ds/getTime.h"

#include "o3ds/async_publisher.h"
#include "o3ds/async_pair.h"
#include "o3ds/pair.h"
#include "o3ds/publisher.h"
//#include "o3ds/request.h"
//#include "o3ds/pipeline.h"

// C:\cpp\git\github\Open3DStream\test_data\beta_fight.fbx tcp://127.0.0.1:6001  

int main(int argc, char *argv[])
{
	if(argc != 4)
	{
		fprintf(stderr, "%s file.fbx protocol url\n", argv[0]);
		fprintf(stderr, "Protocols: pub client server\n");
		return 1;
	}

	O3DS::Connector* connector = nullptr;

	if (strcmp(argv[2], "pub") == 0)
	{
		printf("Publishing on: %s\n", argv[3]);
		connector = new O3DS::AsyncPublisher();
	}
	if (strcmp(argv[2], "client") == 0)
	{
		printf("Conecting to on: %s\n", argv[3]);
		connector = new O3DS::ClientPair();
	}
	if (strcmp(argv[2], "server") == 0)
	{
		printf("Listening on: %s\n", argv[3]);
		connector = new O3DS::ServerPair();
	}

	if (!connector)
	{
		fprintf(stderr, "Invalid Protocol: %s\n", argv[2]);
		return 2;
	}

	O3DS::SubjectList subjects;

	O3DS::Fb::TimeInfo time_info;

	Load(argv[1], subjects, time_info);

	for (auto s : subjects)
	{
		printf("Subject: %s\n", s->mName.c_str());
		for (auto i : s->mTransforms)
		{
			printf("  %s\n", i->info().c_str());
		}
	}

	// Connect 

	if (!connector->start(argv[3]))
	{
		printf(connector->err().c_str());
		return 1;
	}

	// Serialize

	uint8_t buffer[1024 * 16];

redo:
	double zerof = GetTime();

	bool first = true;

	int skips = 0;

	for (FbxTime t = time_info.Start; t < time_info.End; t = t + time_info.Inc)
	{
	
		// Sync time
		double tick = GetTime() - zerof;
		double fdelay = (t.GetSecondDouble() - tick ) * 1000.;
		int delay = (int)(fdelay);

		if (delay < 0)
		{
			skips++;
			continue;
		}

		if(!skips)
			std::this_thread::sleep_for(std::chrono::milliseconds(delay));

		// Update subjects
		for (auto s : subjects)
		{
			for (auto transform : s->mTransforms)
			{
				dynamic_cast<O3DS::Fb::FbTransform*>(transform)->mTime = t;
				transform->update();
			}
		}

		// Serialize

		int ret = 0;
		if (first)
		{
			ret = subjects.Serialize(buffer, 1024 * 16);
			first = false;
		}
		else
		{
			ret = subjects.SerializeUpdate(buffer, 1024 * 16);
		}
		
		// Send

		if (ret > 0)
		{
			if (!connector->writeMsg((const char*)buffer, ret))
			{
				printf("Could not send: %s\n", connector->err().c_str());
			}
		}

		// O3DS::SubjectList s;
		// s.Parse((const char*)buffer, ret);

		// printf("%f    %f   %f   %f    %d  %d\n", GetTime(), tick, t.GetSecondDouble(), fdelay, skips, ret);

		skips = 0;

	}

	goto redo;


	return 0;

}

