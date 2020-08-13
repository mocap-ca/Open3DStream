#include "mobuModel.h"

#include <string>
#include <vector>

#pragma warning( disable : 4189 )


void MobuUpdater::update(O3DS::Transform *transform)
{
	FBMatrix MobuTransform;
	mModel->GetMatrix(MobuTransform, kModelTransformation, true, nullptr);

	FBMatrix MatOffset;
	FBRVector RotOffset(90, 0, 0);
	FBRotationToMatrix(MatOffset, RotOffset);
	FBMatrixMult(MobuTransform, MatOffset, MobuTransform);

	double ret[16];

	transform->mMatrix(1,0) = -MobuTransform(1, 0);
	transform->mMatrix(1,1) =  MobuTransform(1, 1);
	transform->mMatrix(1,2) = -MobuTransform(1, 2);
	transform->mMatrix(1,3) = -MobuTransform(1, 3);

	for (int j = 0; j < 4; ++j)
	{
		if (j == 1)
			continue;
		transform->mMatrix(j, 0) =  MobuTransform(j, 0);
		transform->mMatrix(j, 1) = -MobuTransform(j, 1);
		transform->mMatrix(j, 2) =  MobuTransform(j, 2);
		transform->mMatrix(j, 3) =  MobuTransform(j, 3);
	}

	for (int u = 0; u < 4; u++)
		for (int v = 0; v < 4; v++)
			transform->mMatrix(u, v) = ret[u + v * 4];

}

void TraverseSubject(O3DS::Subject *subject, FBModel *model, int parentId)
{
	if (model == nullptr) return;

	std::string name = model->Name.operator const char *();
	subject->addTransform(name, parentId, new MobuUpdater(model));

	int nextId = subject->size() - 1;

	for (int i = 0; i < model->Children.GetCount(); i++)
	{
		FBModel *child = model->Children[i];
		if (child != nullptr)
		{
			TraverseSubject(subject, child, nextId);
		}
	}
}
