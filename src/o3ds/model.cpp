#include "model.h"

#include "schema_generated.h"
#include "getTime.h"

using namespace MyGame::Sample;

int O3DS::Serialize(SubjectList &data, uint8_t *outbuf, int buflen, bool add_names)
{
	double timestamp = GetTime();

	flatbuffers::FlatBufferBuilder builder(4096);

	std::vector<flatbuffers::Offset<MyGame::Sample::Subject>> subjects;

	for (auto& subject : data)
	{
		auto subject_name = builder.CreateString(subject->mName);

		std::vector<flatbuffers::Offset<MyGame::Sample::Transform>> skeleton;

		std::vector<flatbuffers::Offset<flatbuffers::String>> name_list;

		for (auto& t : subject->mTransforms)
		{
			if (add_names)
				name_list.push_back(builder.CreateString(t->mName));

			auto tr = MyGame::Sample::Translation((float)t->mTranslation[0], 
				(float)t->mTranslation[1], 
				(float)t->mTranslation[2]);
			auto ro = MyGame::Sample::Rotation((float)t->mOrientation[0], 
				(float)t->mOrientation[1], 
				(float)t->mOrientation[2], 
				(float)t->mOrientation[3]);
			skeleton.push_back(CreateTransform(builder, &tr, &ro, t->mParentId));
		}

		auto transforms = builder.CreateVector(skeleton);
		auto nn = builder.CreateVector(name_list);
		auto s = CreateSubject(builder, transforms, nn, subject_name);
		subjects.push_back(s);
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


void O3DS::Subject::update(bool useWorldMatrix)
{
	for (auto transform : mTransforms)
	{
		transform->update();
	}


	if (useWorldMatrix)
	{
		for (auto transform : mTransforms)
		{
			if (transform->mParentId >= 0)
			{
				transform->mParentInverseMatrix = mTransforms.items[transform->mParentId]->mMatrix.Inverse();
			}
		}

		for (auto i : mTransforms)
		{
			O3DS::Matrix<double> transformMatrix;
			if (i->mParentId >= 0)
			{
				transformMatrix = i->mMatrix * i->mParentInverseMatrix;
			}
			else
			{
				transformMatrix = i->mMatrix;
			}
			i->mTranslation = transformMatrix.GetTranslation();
			i->mOrientation = transformMatrix.GetQuaternion();
		}
	}
}

