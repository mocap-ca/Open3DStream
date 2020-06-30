#ifndef __OPEN3D_DEVICE_MODEL_H__
#define __OPEN3D_DEVICE_MODEL_H__

#include <fbsdk/fbsdk.h>
#include <vector>


namespace O3DS
{

	class Transform
	{
	public:
		Transform(FBModel *model)
			: mModel(model)
			, mName(model->Name.AsString())
		{}

		FBModel *mModel;
		std::string mName;

		void Update();

		double tx;
		double ty;
		double tz;

		double rx;
		double ry;
		double rz;
	};

	class SubjectItem
	{
	public:
		SubjectItem(FBComponent *model, FBString name) : mModel(model), mName(name) {}
		FBComponent *mModel;
		FBString mName;
		std::vector<Transform> mTransforms;

		void Traverse();
		void Traverse(FBModel*);
	};

	int Serialize(std::vector<O3DS::SubjectItem> &data, uint8_t *outbuf, int buflen);



} // FBStream


#endif
