#include "mobuModel.h"


#include <string>
#include <vector>





#pragma warning( disable : 4189 )


void MobuTransform::Update()
{
	FBMatrix MobuTransform;
	mModel->GetMatrix(MobuTransform, kModelTransformation, true, nullptr);

	FBMatrix MatOffset;
	FBRVector RotOffset(90, 0, 0);
	FBRotationToMatrix(MatOffset, RotOffset);
	FBMatrixMult(MobuTransform, MatOffset, MobuTransform);

	double ret[16];

	mMatrix(1,0) = -MobuTransform(1, 0);
	mMatrix(1,1) =  MobuTransform(1, 1);
	mMatrix(1,2) = -MobuTransform(1, 2);
	mMatrix(1,3) = -MobuTransform(1, 3);

	for (int j = 0; j < 4; ++j)
	{
		if (j == 1)
			continue;
		mMatrix(j, 0) =  MobuTransform(j, 0);
		mMatrix(j, 1) = -MobuTransform(j, 1);
		mMatrix(j, 2) =  MobuTransform(j, 2);
		mMatrix(1, 3) =  MobuTransform(j, 3);
	}

	for (int u = 0; u < 4; u++)
		for (int v = 0; v < 4; v++)
			mMatrix(u, v) = ret[u + v * 4];

}

void MobuSubject::Traverse()
{
	// Clear mTransforms and recursively parse mModel for transforms 
	mTransforms.clear();
	Traverse(dynamic_cast<FBModel*>(mModel), -1);
}

void MobuSubject::Traverse(FBModel *model, int parentId)
{
	if (model == nullptr) return;

	std::shared_ptr<MobuTransform> ptr = std::make_shared<MobuTransform>(model, parentId);
	mTransforms.push_back(std::dynamic_pointer_cast<O3DS::Transform>(ptr));

	int nextId = (int)mTransforms.size() - 1;

	for (int i = 0; i < model->Children.GetCount(); i++)
	{
		FBModel *child = model->Children[i];
		if (child != nullptr)
		{
			Traverse(child, nextId);
		}
	}
}
