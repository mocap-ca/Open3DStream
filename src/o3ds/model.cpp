/*
Open 3D Stream

Copyright 2020 Alastair Macleod

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include "model.h"
#include "schema_generated.h"
#include "getTime.h"

using namespace MyGame::Sample;

MyGame::Sample::Direction dir(enum O3DS::Direction d)
{
	switch (d)
	{
	case O3DS::Up: return MyGame::Sample::Direction_Up;
	case O3DS::Down: return MyGame::Sample::Direction_Down;
	case O3DS::Left: return MyGame::Sample::Direction_Left;
	case O3DS::Right: return MyGame::Sample::Direction_Right;
	case O3DS::Forward: return MyGame::Sample::Direction_Forward;
	case O3DS::Back: return MyGame::Sample::Direction_Back;
	}
	return MyGame::Sample::Direction_None;
}

enum O3DS::Direction dir(MyGame::Sample::Direction d)
{
	switch (d)
	{
	case MyGame::Sample::Direction_Up: return O3DS::Up;
	case MyGame::Sample::Direction_Down: return O3DS::Down;
	case MyGame::Sample::Direction_Left: return O3DS::Left;
	case MyGame::Sample::Direction_Right: return O3DS::Right;
	case MyGame::Sample::Direction_Forward: return O3DS::Forward;
	case MyGame::Sample::Direction_Back: return O3DS::Back;
	}
	return O3DS::None;
}

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

	auto root = CreateSubjectList(builder, builder.CreateVector(subjects), timestamp,
		dir(data.mContext.mX), dir(data.mContext.mY), dir(data.mContext.mZ));

	builder.Finish(root);

	uint8_t *buf = builder.GetBufferPointer();
	int size = builder.GetSize();

	if (size > buflen)
		return 0;

	memcpy(outbuf, buf, size);

	return size;
}

void O3DS::Parse(SubjectList &outSubjects, const char *data, size_t len)
{
	auto root = GetSubjectList(data);

	double tt = root->time();

	auto subjects_data = root->subjects();

	outSubjects.mContext.mX = dir(root->xAxis());
	outSubjects.mContext.mY = dir(root->yAxis());
	outSubjects.mContext.mZ = dir(root->zAxis());

	for (uint32_t i = 0; i < subjects_data->size(); i++)
	{
		// For each subject
		auto inSubject = subjects_data->Get(i);
		std::string name = inSubject->name()->str();

		// Check to see if this subject already exists
		Subject *outSubject = outSubjects.findSubject(name);
		if (outSubject == nullptr)
		{
			outSubject = outSubjects.addSubject(name);
		}

		// Get the nodes and names from the wire
		auto inNodes = inSubject->nodes();
		auto inNames = inSubject->names();


		if (inNodes->size() == inNames->size())
		{
			outSubject->clear();

			for (int n = 0; n < (int)inNodes->size(); n++)
			{
				auto inNode = inNodes->Get(n);
				std::string inName = inSubject->names()->Get(n)->str();
				outSubject->addTransform(inName, inNode->parent());
			}
		}

		for (int n = 0; n < (int)inNodes->size() && n < outSubject->mTransforms.size(); n++)
		{
			Transform *outTransform = outSubject->mTransforms.mItems[n];
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
				transform->mParentInverseMatrix = mTransforms.mItems[transform->mParentId]->mMatrix.Inverse();
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

