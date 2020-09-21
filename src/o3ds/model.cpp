#include "model.h"

#include "schema_generated.h"
#include "getTime.h"

using namespace MyGame::Sample;

int O3DS::Serialize(const char *key, SubjectList &data, uint8_t *outbuf, int buflen, bool add_names)
{
	double timestamp = GetTime();

	flatbuffers::FlatBufferBuilder builder(4096);

	flatbuffers::Offset<flatbuffers::String> key_name = 0;

	if (key)
	{
		key_name = builder.CreateString(key);
	}

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

	auto root = CreateSubjectList(builder, builder.CreateVector(subjects), timestamp, key_name);

	builder.Finish(root);

	uint8_t *buf = builder.GetBufferPointer();
	int size = builder.GetSize();

	if (size > buflen)
		return 0;

	memcpy(outbuf, buf, size);

	return size;

}

void O3DS::Parse(std::string& key, SubjectList &outSubjects, const char *data, size_t len)
{
	auto root = GetSubjectList(data);

	double tt = root->time();

	auto subjects_data = root->subjects();

	for (uint32_t i = 0; i < subjects_data->size(); i++)
	{
		// For each subject
		auto inSubject = subjects_data->Get(i);
		std::string name = inSubject->name()->str();

		Subject *outSubject = outSubjects.findSubject(name);
		if (outSubject == nullptr)
		{
			outSubject = outSubjects.addSubject(name);
		}

		auto inNodes = inSubject->nodes();
		auto inNames = inSubject->names();

		if (inNodes->size() == inNames->size())
		{
			outSubject->clear();

			for (int n = 0; n < inNodes->size(); n++)
			{
				auto inNode = inNodes->Get(n);
				std::string inName = inSubject->names()->Get(n)->str();
				outSubject->addTransform(inName, inNode->parent());
			}
		}

		for (int n = 0; n < inNodes->size() && n < outSubject->mTransforms.size(); n++)
		{
			Transform *outTransform = outSubject->mTransforms.items[n];
			auto inNode = inNodes->Get(n);
			auto inTranslation = inNode->translation();
			auto inRotation = inNode->rotation();

			outTransform->mTranslation.v[0] = inTranslation->x();
			outTransform->mTranslation.v[1] = inTranslation->y();
			outTransform->mTranslation.v[2] = inTranslation->z();
			outTransform->mOrientation.v[0] = inRotation->x();
			outTransform->mOrientation.v[1] = inRotation->y();
			outTransform->mOrientation.v[2] = inRotation->z();
			outTransform->mOrientation.v[3] = inRotation->w();
		}

	}
	auto key_name = root->key();
	if (key_name) key.assign(key_name->str());



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

