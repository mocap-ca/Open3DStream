

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
#include "o3ds/websocket.h"
#include "o3ds/publisher.h"
//#include "o3ds/request.h"
//#include "o3ds/pipeline.h"
//

#define BUFSZ 1024 * 80

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
		connector = new O3DS::WebsocketClient();
	}
	if (strcmp(argv[2], "server") == 0)
	{
		printf("Listening on: %s\n", argv[3]);
		O3DS::WebsocketBroadcastServer* ws = new O3DS::WebsocketBroadcastServer();
		connector =ws;
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
		printf(connector->getError().c_str());
		return 1;
	}

	// Run



	// Serialize

	std::vector<char> buffer;

redo:
	double zerof = GetTime();

	bool first = true;
	int n = 0;

	int skips = 0;

	printf("**************   Loop\n");

	int frame = 0;

	for (FbxTime t = time_info.Start; t < time_info.End; t += time_info.Inc, frame++)
	{

		printf("T: %f\n", t.GetSecondDouble());
	
		// Sync time
		double tick = GetTime() - zerof;
		double fdelay = (t.GetSecondDouble() - tick ) * 1000.;
		int delay = (int)(fdelay);

		if (delay < 0)
		{
			skips++;
			printf("Delay %f\n", fdelay);
		}
		else
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(delay));

			// Introduce random delays to simulate packet buffering
			if (rand() < RAND_MAX / 10)
				std::this_thread::sleep_for(std::chrono::milliseconds(1000));

		}


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

		// printf("time: %f  %f   %f\n", zerof, t.GetSecondDouble(), zerof + t.GetSecondDouble());

		int ret = 0;
		if (frame % 100 == 0)
		{
			ret = subjects.Serialize(buffer, zerof + t.GetSecondDouble());
			first = false;
		}
		else
		{
			ret = subjects.SerializeUpdate(buffer, zerof + t.GetSecondDouble());
		}
		
		// Send

		if (ret > 0)
		{
			if (!connector->write((const char*)&buffer[0], ret))
			{
				printf("Could not send: %s\n", connector->getError().c_str());
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

