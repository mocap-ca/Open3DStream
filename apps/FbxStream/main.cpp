

#include "schema_generated.h"
using namespace MyGame::Sample;

#include <iostream>
#include <chrono>
#include <thread>

#include "fbxloader.h"
#include "o3ds/model.h"
#include "o3ds/getTime.h"
#include "o3ds/broadcaster.h"

int main(int argc, char *argv[])
{

	if(argc != 3)
	{
		fprintf(stderr, "%s file.fbx url\n", argv[0]);
		return 1;
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
	O3DS::Broadcaster broadcaster;

	if (!broadcaster.start(argv[2], 20))
	{
		printf(broadcaster.mError.c_str());
		return 1;
	}

	// Serialize

	uint8_t buffer[1024 * 16];

redo:
	double zerof = GetTime();

	bool first = true;

	for (FbxTime t = time_info.Start; t < time_info.End; t = t + time_info.Inc)
	{
		double tick = GetTime() - zerof;
		int delay = (int)((t.GetSecondDouble() - tick) * 1000.f);
		printf("%f    %f   %f   %d\n", GetTime(), tick, t.GetSecondDouble(), delay);

		if (delay < 0) continue;

		for (auto i : refs)
		{
			i->mTime = t;
		}

		subjects.update(true);


		int ret = O3DS::Serialize(subjects, buffer, 1024 * 16, first);
		first = false;
		
		if (ret > 0)
		{


			if (!broadcaster.send(buffer, ret))
			{
				printf("Could not send\n");
				break;
			}
		}
		else
		{
			//printf("No data returned\n");
		}

		//printf("%d bytes\n", ret);

		if (delay > 0)
			std::this_thread::sleep_for(std::chrono::milliseconds(delay));
	}

	goto redo;


	return 0;

}

