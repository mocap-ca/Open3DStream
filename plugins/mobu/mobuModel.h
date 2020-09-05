#ifndef OPEN3D_DEVICE_MODEL_H
#define OPEN3D_DEVICE_MODEL_H

#include <fbsdk/fbsdk.h>
#include <vector>

#include "o3ds/model.h"

class MobuUpdater : public O3DS::Updater
{
public:
	MobuUpdater(FBModel *model)
		: mModel(model)
	{}

	FBModel *mModel;

	int mParentId;

	void update(O3DS::Transform*);

	std::string info() { return std::string(mModel->GetFullName()); }


};

class MobuSubjectInfo : public O3DS::SubjectInfo
{
public:
	MobuSubjectInfo(FBModel *model) :mModel(model) {}

	FBModel *mModel;
};


void TraverseSubject(O3DS::Subject *subject, FBModel*, int parentId = -1);






#endif
