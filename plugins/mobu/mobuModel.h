#ifndef OPEN3D_DEVICE_MOBU_MODEL_H
#define OPEN3D_DEVICE_MOBU_MODEL_H

#include <vector>
#include <string>

#include "o3ds/model.h"

#include <fbsdk/fbsdk.h>

namespace O3DS
{
	namespace Mobu
	{

		class MobuTransform : public O3DS::Transform
		{
		public:
			MobuTransform(FBModel *model, int parentId)
				: O3DS::Transform(parentId)
				, mModel(model)
			{

			}

			FBModel *mModel;

			void update();

			std::string info() { return std::string(mModel->GetFullName()); }


		};


		void TraverseSubject(O3DS::Subject *subject, FBModel*, int parentId = -1);


	}
}



#endif
