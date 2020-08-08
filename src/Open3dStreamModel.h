#ifndef OPEN3D_STREAM_MODEL_H
#define OPEN3D_STREAM_MODEL_H

#include <fbsdk/fbsdk.h>
#include <vector>

#include "cml/cml.h"

namespace O3DS
{
	class Transform
	{
	public:
		Transform(std::string& name, int parentId)
			: mName(name)
			, mParentId(parentId)
		{}

		Transform(int parentId)
			: mName()
			, mParentId(parentId)
		{}

		Transform()
			: mName()
			, mParentId(-1)
		{}

		virtual ~Transform() {};

		cml::matrix44d mMatrix;
		cml::matrix44d mParentInverseMatrix;

		std::string mName;
		int mParentId;

	};

	class Subject
	{
	public:
		Subject() {}
		Subject(std::string name) : mName(name) {}
		std::string mName;
		std::vector<std::shared_ptr<Transform>> mTransforms;
	};

	int Serialize(std::vector<std::shared_ptr<O3DS::Subject>> &data, uint8_t *outbuf, int buflen, bool names);
	


} // FBStream


#endif
