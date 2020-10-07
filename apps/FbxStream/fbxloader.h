#ifndef FBXLOADER_H
#define FBXLOADER_H

#include "o3ds/model.h"
#include <fbxsdk.h>
//#include <fbxnode.h>
#include <string>

namespace O3DS
{
	namespace Fb
	{
		struct TimeInfo
		{
			FbxTime Start;
			FbxTime End;
			FbxTime Inc;
		};

		class FbTransform : public O3DS::Transform
		{
		public:
			FbTransform(fbxsdk::FbxNode *node, int parentId);
			void update() override;
			std::string info();
			fbxsdk::FbxNode *mNode;
			fbxsdk::FbxTime mTime;
		};

		int Load(const char *path, O3DS::SubjectList& subjects, TimeInfo &time_info);

	} // Fbx
} // O3DS
#endif
