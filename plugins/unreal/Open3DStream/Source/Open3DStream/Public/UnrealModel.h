#ifndef O3DS_UNREAL_MODEL_H
#define O3DS_UNREAL_MODEL_H

#include "o3ds/model.h"
#include <string>

namespace O3DS
{
	namespace Unreal
	{
		class UnTransform : public O3DS::Transform
		{
		public:
			UnTransform(std::string name, int parentId)
				: O3DS::Transform(name, parentId) {}


			virtual ~UnTransform() {};
		};

		class UnBuilder : public O3DS::TransformBuilder
		{
			O3DS::Transform *build(std::string name, int parentId)
			{
				return (O3DS::Transform*) new UnTransform(name, parentId);
			}
		};
	}
}


#endif
