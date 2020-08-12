

#include "schema_generated.h"
using namespace MyGame::Sample;

#include "windows.h"

#include "fbxloader.h"
#include "Open3dStreamModel.h"
#include "get_time.h"
#include "sender.h"

int main(int argc, char *argv[])
{

	O3DS::SubjectList subjects;

	O3DS::TimeInfo time_info;

	const char *file_path = "E:\\git\\github\\Open3DStream\\test_data\\beta_anim.fbx";

	Load(file_path, subjects, time_info);

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
	Sender sender;
	if (!sender.connect("tcp://localhost:5555"))
	{
		printf("Could not connect\n");
		return 1;
	}

	// Serialize

	uint8_t buffer[1024 * 16];

	double zerof = GetTime() * 1000.;




	bool first = true;

	for (FbxTime t = time_info.Start; t < time_info.End; t = t + time_info.Inc)
	{
		double tick = GetTime() * 1000. - zerof;
		double delay = t.GetSecondDouble() * 1000 - tick;
		printf("%f   %f   %f\n", tick, t.GetSecondDouble(), delay);

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
			if (!sender.send(buffer, ret))
			{
				printf("Could not send\n");
				break;
			}
		}
		else
		{
			printf("No data returned\n");
		}

		printf("%d bytes\n", ret);

		if (delay > 0)
			Sleep(delay);
	}


	return 0;

}

