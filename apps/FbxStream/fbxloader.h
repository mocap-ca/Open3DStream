#ifndef FBXLOADER_H
#define FBXLOADER_H

#include "Open3dStreamModel.h"
#include <fbxsdk.h>
#include <string>

namespace O3DS
{

	struct TimeInfo
	{
		FbxTime Start;
		FbxTime End;
		FbxTime Inc;
	};

	class MobuUpdater : public O3DS::Updater
	{
	public:
		MobuUpdater(FbxNode *node);
		void update(O3DS::Transform *t);
		std::string info();
		FbxNode *mNode;
		FbxTime mTime;
	};

	int Load(const char *path, O3DS::SubjectList& subjects, TimeInfo &time_info);

}
#endif
