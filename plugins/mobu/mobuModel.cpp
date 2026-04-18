#include "mobuModel.h"

#include <string>
#include <vector>


#pragma warning( disable : 4189 )

namespace O3DS
{
	namespace Mobu
	{
		void operator >> (const FBVector3d& src, Eigen::Vector3d& dst)
		{
			dst = Eigen::Vector3d(src.mValue[0], src.mValue[1], src.mValue[2]);
		}

		void setRotation(const FBVector3d& src, const FBModelRotationOrder& order, O3DS::Matrix& dst)
		{
			O3DS::Matrix x = O3DS::rotateX(O3DS::rad(src.mValue[0]));
			O3DS::Matrix y = O3DS::rotateY(O3DS::rad(src.mValue[1]));
			O3DS::Matrix z = O3DS::rotateZ(O3DS::rad(src.mValue[2]));

			O3DS::Matrix m;

			if (order == FBModelRotationOrder::kFBEulerXYZ) dst = z * y * x;
			if (order == FBModelRotationOrder::kFBEulerXZY) dst = y * z * x;
			if (order == FBModelRotationOrder::kFBEulerYZX) dst = x * z * y;
			if (order == FBModelRotationOrder::kFBEulerYXZ) dst = z * x * y;
			if (order == FBModelRotationOrder::kFBEulerZXY) dst = y * x * z;
			if (order == FBModelRotationOrder::kFBEulerZYX) dst = x * y * z;
			if (order == kFBSphericXYZ)
			{
				dst = z * y * x;
			}
		}

		void setRotation(const FBVector3d& src, const FBModelRotationOrder &order, Eigen::Quaterniond& dst)
		{
			O3DS::Matrix m;
			setRotation(src, order, m);
			dst = toQuaternion(m);
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

			O3DS::Matrix pre;
			O3DS::Matrix post;

			//setRotation(mModel->PreRotation, rotOrder, pre);
			//setRotation(mModel->PostRotation, rotOrder, post);
			
			setRotation(mModel->PreRotation, FBModelRotationOrder::kFBEulerXYZ, pre);
			setRotation(mModel->PostRotation, FBModelRotationOrder::kFBEulerXYZ, post);

			this->mMatrices.push_back(pre);
			this->mMatrices.push_back(post);

			this->mTransformOrder.push_back(O3DS::ComponentType::TTranslation);
			this->mTransformOrder.push_back(O3DS::ComponentType::TMatrix);
			this->mTransformOrder.push_back(O3DS::ComponentType::TRotation);
			this->mTransformOrder.push_back(O3DS::ComponentType::TMatrix);
			this->mTransformOrder.push_back(O3DS::ComponentType::TScale);

		}

		void MobuTransform::update()
		{
			FBModelRotationOrder rotOrder = mModel->RotationOrder;
			(FBVector3d)(mModel->Translation) >> this->translation.value;
			setRotation(mModel->Rotation, rotOrder, this->rotation.value);
			(FBVector3d)(mModel->Scaling) >> this->scale.value;
		}

		void TraverseSubject(O3DS::Subject* subject)
		{
			FBModel* model = static_cast<FBModel*>(subject->mReference);
			TraverseSubject(subject, model);
		}

		void TraverseSubject(O3DS::Subject *subject, FBModel *model, int parentId)
		{
			// Called when mobu starts
			if (model == nullptr) return;

			if (parentId == -1)
			{
				// Clear the subject list transform list when starting
				subject->clearTransforms();
			}

			if (subject->mJoints.size() > 0)
			{
				// Search for matching joint names in the joint name list 
				// Used to filter the parsed joints to a subset used for sending
				std::vector<std::string>& names = subject->mJoints;
				std::string jointNameStr(model->Name.operator const char* ());
				if (std::find(names.begin(), names.end(), jointNameStr) == names.end())
					return;
			}

			subject->addTransform(std::make_unique<MobuTransform>(model, parentId));


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
