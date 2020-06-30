#include "model.h"

#include <string>
#include <vector>

#include "schema_generated.h"
using namespace MyGame::Sample;


#pragma warning( disable : 4189 )


void O3DS::Transform::Update()
{
	FBVector3d trans = mModel->Translation;
	FBVector3d rot = mModel->Rotation;

	tx = trans[0];
	ty = trans[1];
	tz = trans[2];

	rx = rot[0];
	ry = rot[1];
	rz = rot[2];

}

void O3DS::SubjectItem::Traverse()
{
	mTransforms.clear();
	Traverse(dynamic_cast<FBModel*>(mModel));
}

void O3DS::SubjectItem::Traverse(FBModel *model)
{
	if (model == nullptr) return;

	mTransforms.push_back(O3DS::Transform(model));

	for (int i = 0; i < model->Children.GetCount(); i++)
	{
		FBModel *child = model->Children[i];
		if (child != nullptr)
		{
			Traverse(child);
		}
	}
}

int Serialize(std::vector<O3DS::SubjectItem> &data, uint8_t *outbuf, int buflen)
{
	flatbuffers::FlatBufferBuilder builder(4096);

	for (auto subject : data)
	{
		auto subject_name = builder.CreateString((const char*)subject.mName);

		FBComponent *model = subject.mModel;
		FBString name = subject.mName;

		int id1 = model->GetTypeId();
		int id2 = model->TypeInfo;

		std::vector<flatbuffers::Offset<MyGame::Sample::Transform>> skeleton;

		if (model->Is(FBModelNull::TypeInfo))
		{
			subject.Traverse();

			auto subject_name = builder.CreateString((const char *)subject.mName);

			for (auto t : subject.mTransforms)
			{
				auto t_name = builder.CreateString(t.mName);

				auto tr = MyGame::Sample::Translation(t.tx, t.ty, t.tz);
				auto ro = MyGame::Sample::Rotation(t.rx, t.ry, t.rz);

				skeleton.push_back(CreateTransform(builder, &tr, &ro, t_name));
			}

			auto transforms = builder.CreateVector(skeleton);
			auto subject = CreateSubject(builder, subject_name, transforms);
		}
	}

	uint8_t *buf = builder.GetBufferPointer();
	int size = builder.GetSize();

	if (size > buflen)
		return 0;

	memcpy(outbuf, buf, size);

	return size;

}
