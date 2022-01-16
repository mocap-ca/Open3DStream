#ifndef OPEN3D_DEVICE_MOBU_MODEL_H
#define OPEN3D_DEVICE_MOBU_MODEL_H

#include <vector>
#include <string>

#include "o3ds/model.h"

#include <fbsdk/fbsdk.h>

// http://docs.autodesk.com/MB/2014/ENU/MotionBuilder-SDK-Documentation/cpp_ref/class_f_b_model.html

namespace O3DS
{
	namespace Mobu
	{
		class MobuTransform : public O3DS::Transform
		{
		public:
			MobuTransform(FBModel *model, int parentId);

			FBModel *mModel;

			void update() override;

			std::string info() { return std::string(mModel->GetFullName()); }
		};

		void TraverseSubject(O3DS::Subject *subject, FBModel*, int parentId = -1);
	}

}



#endif
