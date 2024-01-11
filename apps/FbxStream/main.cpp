#include <iostream>
#include <chrono>
#include <thread>

#include "fbxloader.h"
#include "o3ds/o3ds.h"
#include "o3ds/model.h"
#include "o3ds/getTime.h"

#include "o3ds/async_publisher.h"
#include "o3ds/async_pair.h"
#include "o3ds/pair.h"
#include "o3ds/publisher.h"
#include "o3ds/request.h"
#include "o3ds/pipeline.h"

#define BUFSZ 1024 * 80

int main(int argc, char *argv[])
{
	printf("Open3DStream v%s\n", O3DS::version);

	if(argc != 3)
	{
		fprintf(stderr, "%s file.fbx url\n", argv[0]);
		return 1;
	}

	O3DS::Connector* connector = new O3DS::AsyncPublisher();

	O3DS::SubjectList subjects;

	O3DS::Fb::TimeInfo time_info;

	if (Load(argv[1], subjects, time_info) != 0)
	{
		fprintf(stderr, "Unable to load fbx: %s\n", argv[1]);
		return 1;
	}

	for (auto s : subjects)
	{
		printf("Subject: %s\n", s->mName.c_str());
		for (auto i : s->mTransforms)
		{
			printf("  %s\n", i->info().c_str());
		}
	}

	// Connect 

	if (!connector->start(argv[2]))
	{
		printf(connector->getError().c_str());
		return 1;
	}

	// Run

	std::vector<char> buffer;

redo:
	double zerof = GetTime();

	bool first = true;
	int n = 0;

	int skips = 0;

	printf("**************   Loop\n");

	int frame = 0;

	O3DS::SubjectList subjectTest;


	for (FbxTime t = time_info.Start; t < time_info.End; t += time_info.Inc, frame++)
	{

		// printf("T: %f\n", t.GetSecondDouble());
	
		// Sync time
		double tick = GetTime() - zerof;
		double fdelay = (t.GetSecondDouble() - tick ) * 1000.;
		int delay = (int)(fdelay);

		if (delay < 0)
		{
			skips++;
			//printf("Delay %f\n", fdelay);
		}
		else
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(delay));

			// Introduce random delays to simulate packet buffering
			if (rand() < RAND_MAX / 50)
				std::this_thread::sleep_for(std::chrono::milliseconds(900));
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

		printf("time: %f  %f   %f\n", zerof, t.GetSecondDouble(), zerof + t.GetSecondDouble());

		int ret = 0;
		if (frame % 200 == 0)
		{
			ret = subjects.Serialize(buffer, zerof + t.GetSecondDouble());
			first = false;
		}
		else
		{
			size_t count = 0;
			ret = subjects.SerializeUpdate(buffer, count, zerof + t.GetSecondDouble());
		}
		
		// Send

		if (ret > 0)
		{
			auto ptr = (const char*)&buffer[0];
			if (!connector->write(ptr, ret))
			{
				printf("Could not send: %s\n", connector->getError().c_str());
			}

			if(!subjectTest.Parse(ptr, ret)) {
				printf("Could not parse\n");
				return 1;
			}
		}

		// printf("%f    %f   %f   %f    %d  %d\n", GetTime(), tick, t.GetSecondDouble(), fdelay, skips, ret);

		skips = 0;

	}

	goto redo;


	return 0;

}

