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

int O3DS::Serialize(std::vector<O3DS::SubjectItem> &data, uint8_t *outbuf, int buflen, bool add_names)
{
	flatbuffers::FlatBufferBuilder builder(4096);

	std::vector<flatbuffers::Offset<MyGame::Sample::Subject>> subjects;

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

			std::vector<flatbuffers::Offset<flatbuffers::String>> name_list;

			for (auto t : subject.mTransforms)
			{
				if (add_names)
					name_list.push_back(builder.CreateString(t.mName));

				auto tr = MyGame::Sample::Translation(t.tx, t.ty, t.tz);
				auto ro = MyGame::Sample::Rotation(t.rx, t.ry, t.rz);
				int parent = 0;

				skeleton.push_back(CreateTransform(builder, &tr, &ro, parent));
			}

			auto transforms = builder.CreateVector(skeleton);
			auto nn = builder.CreateVector(name_list);
            auto subject = CreateSubject(builder, transforms, nn, subject_name);
			subjects.push_back(subject);		
		}
	}

	auto root = CreateSubjectList(builder, builder.CreateVector(subjects));

	builder.Finish(root);

	uint8_t *buf = builder.GetBufferPointer();
	int size = builder.GetSize();

	if (size > buflen)
		return 0;

	memcpy(outbuf, buf, size);

	return size;

}
