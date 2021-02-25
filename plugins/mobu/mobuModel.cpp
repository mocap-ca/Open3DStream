#include "mobuModel.h"

#include <string>
#include <vector>


#pragma warning( disable : 4189 )

namespace O3DS
{
	namespace Mobu
	{
		void operator >> (const FBVector3d& src, O3DS::Vector3d& dst)
		{
			dst.v[0] = src.mValue[0];
			dst.v[1] = src.mValue[1];
			dst.v[2] = src.mValue[2];
		}

		void setRotation(const FBVector3d& src, const FBModelRotationOrder& order, O3DS::Matrixd& dst)
		{
			O3DS::Matrixd x = O3DS::Matrixd::RotateX(O3DS::rad(src.mValue[0]));
			O3DS::Matrixd y = O3DS::Matrixd::RotateY(O3DS::rad(src.mValue[1]));
			O3DS::Matrixd z = O3DS::Matrixd::RotateZ(O3DS::rad(src.mValue[2]));

			O3DS::Matrixd m;

			if (order == FBModelRotationOrder::kFBEulerXYZ) dst = x * y * z;
			if (order == FBModelRotationOrder::kFBEulerXZY) dst = x * z * y;
			if (order == FBModelRotationOrder::kFBEulerYZX) dst = y * z * x;
			if (order == FBModelRotationOrder::kFBEulerYXZ) dst = y * x * z;
			if (order == FBModelRotationOrder::kFBEulerZXY) dst = z * x * y;
			if (order == FBModelRotationOrder::kFBEulerZYX) dst = z * y * x;
			if (order == kFBSphericXYZ)
			{
				dst = x * y * z;
			}
		}

		void setRotation(const FBVector3d& src, const FBModelRotationOrder &order, O3DS::Vector4d& dst)
		{
			O3DS::Matrixd m;
			setRotation(src, order, m);
			dst = m.GetQuaternion();
		}

		MobuTransform::MobuTransform(FBModel *model, int parentId)
			: O3DS::Transform(std::string(model->Name.AsString()), parentId)
			, mModel(model)
		{

			// const char* name = model->GetFullName();


			FBModelRotationOrder rotOrder = mModel->RotationOrder;
			(FBVector3d)(mModel->Translation) >> this->translation.value;
			setRotation(mModel->Rotation, rotOrder, this->rotation.value);
			(FBVector3d)(mModel->Scaling) >> this->scale.value;

			O3DS::Matrixd pre;
			O3DS::Matrixd post;

			//setRotation(mModel->PreRotation, rotOrder, pre);
			//setRotation(mModel->PostRotation, rotOrder, post);
			
			setRotation(mModel->PreRotation, FBModelRotationOrder::kFBEulerXYZ, pre);
			setRotation(mModel->PostRotation, FBModelRotationOrder::kFBEulerXYZ, post);

			this->matrices.push_back(pre);
			this->matrices.push_back(post);

			this->transformOrder.push_back(O3DS::ComponentType::TTranslation);
			this->transformOrder.push_back(O3DS::ComponentType::TMatrix);
			this->transformOrder.push_back(O3DS::ComponentType::TRotation);
			this->transformOrder.push_back(O3DS::ComponentType::TMatrix);
			this->transformOrder.push_back(O3DS::ComponentType::TScale);

		}

		void MobuTransform::update()
		{
			FBModelRotationOrder rotOrder = mModel->RotationOrder;
			(FBVector3d)(mModel->Translation) >> this->translation.value;
			setRotation(mModel->Rotation, rotOrder, this->rotation.value);
			(FBVector3d)(mModel->Scaling) >> this->scale.value;
		}

		void TraverseSubject(O3DS::Subject *subject, FBModel *model, int parentId)
		{
			// Called when mobu starts
			if (model == nullptr) return;

			if (parentId == -1)
			{
				// Clear the subject list when starting
				subject->clear();
			}

			subject->addTransform(new MobuTransform(model, parentId));

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
