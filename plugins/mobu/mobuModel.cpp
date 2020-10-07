#include "mobuModel.h"

#include <string>
#include <vector>


#pragma warning( disable : 4189 )

namespace O3DS
{
	namespace Mobu
	{

		void MobuTransform::update()
		{
			// Udpates mMatrix only
			FBMatrix MobuTransform;
			mModel->GetMatrix(MobuTransform, kModelTransformation, true, nullptr);

			for (int u = 0; u < 4; u++)
			{
				for (int v = 0; v < 4; v++)
				{
					this->mMatrix(u, v) = MobuTransform(u, v);
				}
			}

			/*
			FBMatrix MatOffset;
			FBRVector RotOffset(90, 0, 0);
			FBRotationToMatrix(MatOffset, RotOffset);
			FBMatrixMult(MobuTransform, MatOffset, MobuTransform);

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
			*/

		}

		void TraverseSubject(O3DS::Subject *subject, FBModel *model, int parentId)
		{
			if (model == nullptr) return;

			if (parentId == -1)
			{
				// Clear the subject list when starting
				subject->clear();
			}

			std::string name = model->Name.operator const char *();
			subject->addTransform(name, parentId);

			int nextId = (int)subject->size() - 1;

			for (int i = 0; i < model->Children.GetCount(); i++)
			{
				FBSDKNamespace::FBModel *child = model->Children[i];
				if (child != nullptr)
				{
					TraverseSubject(subject, child, nextId);
				}
			}
		}


	}
}
