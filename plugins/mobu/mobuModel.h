#ifndef OPEN3D_DEVICE_MODEL_H
#define OPEN3D_DEVICE_MODEL_H

#include <fbsdk/fbsdk.h>
#include <vector>

#include "Open3dStreamModel.h"

class MobuTransform : public O3DS::Transform
{
public:
	MobuTransform(FBModel *model, int parentId)
		: Transform(parentId)
		, mModel(model)
	{
		FBString n = model->Name;
		const char *name = (const char *)n;
		mName.assign(name);
	}

	FBModel *mModel;

	int mParentId;

	void Update();


};

class MobuSubject : public O3DS::Subject
{
public:
	MobuSubject(FBComponent *model, FBString name)
		: O3DS::Subject()
		, mModel(model)
	{}
	FBComponent *mModel;

	void Traverse();
	void Traverse(FBModel*, int parentId);
};





#endif
