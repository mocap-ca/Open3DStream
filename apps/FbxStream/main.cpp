

#include "schema_generated.h"
using namespace MyGame::Sample;

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

	O3DS::TimeInfo time_info;

	Load(argv[1], subjects, time_info);

	subjects.update(true);

	std::vector<O3DS::MobuUpdater*> refs;
	for (auto s : subjects)
	{
		for (auto t : s->mTransforms)
		{
			refs.push_back((O3DS::MobuUpdater*)t->mVisitor);
		}
	}

	for (auto s : subjects)
	{
		printf("Subject: %s\n", s->mName.c_str());
		for (auto i : s->mTransforms)
		{
			printf("  %s\n", i->mVisitor->info().c_str());
		}
	}

	// Connect 
	//O3DS::Publisher publisher;

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
		double tick = GetTime() - zerof;
		int delay = (int)((t.GetSecondDouble() - tick) * 1000.f);

		if (delay < 0)
		{
			skips++;
			continue;
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(delay));


		printf("%f    %f   %f   %d    %d\n", GetTime(), tick, t.GetSecondDouble(), delay, skips);

		skips = 0;

		for (auto i : refs)
		{
			i->mTime = t;
		}

		subjects.update(true);

		int ret = O3DS::Serialize(subjects, buffer, 1024 * 16, true);
		first = false;
		
		if (ret > 0)
		{
			if (!connector->writeMsg((const char*)buffer, ret))
			{
				printf("Could not send: %s\n", connector->err().c_str());
				break;
			}
		}
		else
		{
			//printf("No data returned\n");
		}

		//printf("%d bytes\n", ret);

	}

	goto redo;


	return 0;

}

