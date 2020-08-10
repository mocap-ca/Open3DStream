#include "Open3dStreamModel.h"

#include "schema_generated.h"
#include "get_time.h"
#include "cml/cml.h"

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

		for (auto& transform : subject->mTransforms)
		{
			if (add_names)
				name_list.push_back(builder.CreateString(transform->mName));

			cml::matrix44d transformMatrix;

			if (transform->mParentId >= 0)
			{
				transformMatrix = transform->mParentInverseMatrix * transform->mMatrix;
			}
			else
			{
				transformMatrix = transform->mMatrix;
			}

			cml::quaternion<double> q;
			cml::quaternion_rotation_matrix(q, transformMatrix);

			cml::vector3d t;
			t = cml::matrix_get_translation(transformMatrix);

			auto tr = MyGame::Sample::Translation(t[0], t[1], t[2]);
			auto ro = MyGame::Sample::Rotation(q.x(), q.y(), q.z(), q.w());
			skeleton.push_back(CreateTransform(builder, &tr, &ro, transform->mParentId));
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
