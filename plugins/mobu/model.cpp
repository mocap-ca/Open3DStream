#include "model.h"
#include "get_time.h"

#include <string>
#include <vector>

#include "schema_generated.h"
using namespace MyGame::Sample;


#pragma warning( disable : 4189 )


void O3DS::Transform::Update()
{
	FBMatrix MobuTransform;
	mModel->GetMatrix(MobuTransform, kModelTransformation, true, nullptr);

	FBMatrix MatOffset;
	FBRVector RotOffset(90, 0, 0);
	FBRotationToMatrix(MatOffset, RotOffset);
	FBMatrixMult(MobuTransform, MatOffset, MobuTransform);

	double ret[16];

	ret[4] = -MobuTransform(1, 0);
	ret[5] =  MobuTransform(1, 1);
	ret[6] = -MobuTransform(1, 2);
	ret[7] = -MobuTransform(1, 3);

	for (int j = 0; j < 4; ++j)
	{
		if (j == 1)
			continue;
		ret[j * 4 + 0] =  MobuTransform(j, 0);
		ret[j * 4 + 1] = -MobuTransform(j, 1);
		ret[j * 4 + 2] =  MobuTransform(j, 2);
		ret[j * 4 + 3] =  MobuTransform(j, 3);
	}

	mMatrix = FBMatrix(ret);


}

void O3DS::SubjectItem::Traverse()
{
	// Clear mTransforms and recursively parse mModel for transforms 
	mTransforms.clear();
	Traverse(dynamic_cast<FBModel*>(mModel), -1);
}

void O3DS::SubjectItem::Traverse(FBModel *model, int parentId)
{
	if (model == nullptr) return;

	mTransforms.push_back(O3DS::Transform(model, parentId));

	int nextId = (int)mTransforms.size() - 1;

	for (int i = 0; i < model->Children.GetCount(); i++)
	{
		FBModel *child = model->Children[i];
		if (child != nullptr)
		{
			Traverse(child, nextId);
		}
	}
}

int O3DS::Serialize(std::vector<O3DS::SubjectItem> &data, uint8_t *outbuf, int buflen, bool add_names)
{
	double timestamp = GetTime();

	flatbuffers::FlatBufferBuilder builder(4096);

	std::vector<flatbuffers::Offset<MyGame::Sample::Subject>> subjects;

	for (auto& subject : data)
	{
		auto subject_name = builder.CreateString((const char*)subject.mName);

		FBComponent *model = subject.mModel;
		FBString name = subject.mName;

		int id1 = model->GetTypeId();
		int id2 = model->TypeInfo;

		std::vector<flatbuffers::Offset<MyGame::Sample::Transform>> skeleton;

		if (model->Is(FBModelNull::TypeInfo))
		{
			// Update the transforms
			for (O3DS::Transform& transform : subject.mTransforms)
			{
				transform.Update();
				if (transform.mParentId >= 0)
				{
					FBMatrix m(subject.mTransforms[transform.mParentId].mMatrix);
					transform.mParentInverseMatrix = m.Inverse();
				}
					
			}

			auto subject_name = builder.CreateString((const char *)subject.mName);

			std::vector<flatbuffers::Offset<flatbuffers::String>> name_list;

			for (auto& transform : subject.mTransforms)
			{
				if (add_names)
					name_list.push_back(builder.CreateString(transform.mName));

				FBMatrix transformMatrix;

				if (transform.mParentId >= 0)
				{
					FBMatrixMult(transformMatrix, transform.mParentInverseMatrix, transform.mMatrix);
				}
				else
				{
					transformMatrix = transform.mMatrix;
				}

				FBTVector t;
				FBQuaternion q;

				FBMatrixToTranslation(t, transformMatrix);
				FBMatrixToQuaternion(q, transformMatrix);


				auto tr = MyGame::Sample::Translation(t[0], t[1], t[2]);
				auto ro = MyGame::Sample::Rotation(q[0], q[1], q[2], q[3]);
				skeleton.push_back(CreateTransform(builder, &tr, &ro, transform.mParentId));
			}

			auto transforms = builder.CreateVector(skeleton);
			auto nn = builder.CreateVector(name_list);
            auto subject = CreateSubject(builder, transforms, nn, subject_name);
			subjects.push_back(subject);		
		}
	}



	auto root = CreateSubjectList(builder, builder.CreateVector(subjects), timestamp);

	builder.Finish(root);

	uint8_t *buf = builder.GetBufferPointer();
	int size = builder.GetSize();

	if (size > buflen)
		return 0;

	memcpy(outbuf, buf, size);

	return size;

}
