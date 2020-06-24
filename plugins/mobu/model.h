#ifndef __OPEN3D_DEVICE_MODEL_H__
#define __OPEN3D_DEVICE_MODEL_H__

#include <fbsdk/fbsdk.h>
#include <vector>


class Transform
{
public:
	Transform(FBModel *model)
		: mModel(model)
		, mName(model->Name.AsString())
	{}

	FBModel *mModel;
	std::string mName;

	void Update();

	double tx;
	double ty;
	double tz;

	double rx;
	double ry;
	double rz;
};

class SubjectItem
{
public:
	SubjectItem(FBComponent *model, FBString name) : mModel(model), mName(name) {}
	FBComponent *mModel;
	FBString mName;
	std::vector<Transform> mTransforms;

	void Traverse();
	void Traverse(FBModel*);

};



#endif
