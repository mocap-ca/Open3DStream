#include "model.h"

#include <string>
#include <vector>

#pragma warning( disable : 4189 )



void Transform::Update()
{
	FBVector3d trans = mModel->Translation;
	FBVector3d rot = mModel->Rotation;

	tx = trans[0];
	ty = trans[1];
	tz = trans[2];

	rx = rot[0];
	ry = rot[1];
	rz = rot[2];

}

void SubjectItem::Traverse()
{
	mTransforms.clear();
	Traverse(dynamic_cast<FBModel*>(mModel));
}

void SubjectItem::Traverse(FBModel *model)
{
	if (model == nullptr) return;

	mTransforms.push_back(Transform(model));

	for (int i = 0; i < model->Children.GetCount(); i++)
	{
		FBModel *child = model->Children[i];
		if (child != nullptr)
		{
			Traverse(child);
		}
	}

}