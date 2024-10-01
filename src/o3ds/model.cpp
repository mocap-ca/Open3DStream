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
#include "getTime.h"
#include "CRC.h"
#include <algorithm>
#include <iterator>
#include <sstream>

using namespace O3DS::Data;

void operator >>(const O3DS::TransformTranslation& src, O3DS::Data::Translation &dst)
{
	dst = O3DS::Data::Translation(
		(float)src.value.v[0],
		(float)src.value.v[1],
		(float)src.value.v[2]);
}

void operator >>(const O3DS::Data::Translation& src, O3DS::TransformTranslation &dst)
{
	dst = O3DS::TransformTranslation(src.x(), src.y(), src.z());
}

void operator >>(const O3DS::Data::TranslationUpdate& src, O3DS::TransformTranslation &dst)
{
	dst = O3DS::TransformTranslation(src.x(), src.y(), src.z());
}

void operator >>(const O3DS::TransformRotation& src, O3DS::Data::Rotation &dst)
{
	dst = O3DS::Data::Rotation(
		(float)src.value.v[0],
		(float)src.value.v[1],
		(float)src.value.v[2],
		(float)src.value.v[3]);
}

void operator >>(const O3DS::Data::Rotation& src, O3DS::TransformRotation &dst)
{
	dst = O3DS::TransformRotation(src.x(), src.y(), src.z(), src.w());
}

void operator >>(const O3DS::Data::RotationUpdate& src, O3DS::TransformRotation &dst)
{
	dst = O3DS::TransformRotation(src.x(), src.y(), src.z(), src.w());
}


void operator >>(const O3DS::TransformScale& src, O3DS::Data::Scale &dst)
{
	dst = O3DS::Data::Scale(
		(float)src.value.v[0],
		(float)src.value.v[1],
		(float)src.value.v[2]);
}

void operator >>(const O3DS::Data::Scale& src, O3DS::TransformScale &dst)
{
	dst = O3DS::TransformScale(src.x(), src.y(), src.z());
}

void operator >>(const O3DS::Data::ScaleUpdate& src, O3DS::TransformScale &dst)
{
	dst = O3DS::TransformScale(src.x(), src.y(), src.z());
}

void operator >>(const O3DS::TransformMatrix& src, O3DS::Data::Matrix &dst)
{
	dst = O3DS::Data::Matrix(
		(float)src.value.m[0][0],
		(float)src.value.m[0][1],
		(float)src.value.m[0][2],
		(float)src.value.m[0][3],
		(float)src.value.m[1][0],
		(float)src.value.m[1][1],
		(float)src.value.m[1][2],
		(float)src.value.m[1][3],
		(float)src.value.m[2][0],
		(float)src.value.m[2][1],
		(float)src.value.m[2][2],
		(float)src.value.m[2][3],
		(float)src.value.m[3][0],
		(float)src.value.m[3][1],
		(float)src.value.m[3][2],
		(float)src.value.m[3][3]);
}

void operator >>(const O3DS::Data::Matrix& src, O3DS::TransformMatrix &dst)
{
	dst.value.m[0][0] = src.m00();
	dst.value.m[0][1] = src.m01();
	dst.value.m[0][2] = src.m02();
	dst.value.m[0][3] = src.m03();
	dst.value.m[1][0] = src.m10();
	dst.value.m[1][1] = src.m11();
	dst.value.m[1][2] = src.m12();
	dst.value.m[1][3] = src.m13();
	dst.value.m[2][0] = src.m20();
	dst.value.m[2][1] = src.m21();
	dst.value.m[2][2] = src.m22();
	dst.value.m[2][3] = src.m23();
	dst.value.m[3][0] = src.m30();
	dst.value.m[3][1] = src.m31();
	dst.value.m[3][2] = src.m32();
	dst.value.m[3][3] = src.m33();
}

O3DS::Data::Direction dir(enum O3DS::Direction d)
{
	switch (d)
	{
	case O3DS::Direction::Up: return O3DS::Data::Direction_Up;
	case O3DS::Direction::Down: return O3DS::Data::Direction_Down;
	case O3DS::Direction::Left: return O3DS::Data::Direction_Left;
	case O3DS::Direction::Right: return O3DS::Data::Direction_Right;
	case O3DS::Direction::Forward: return O3DS::Data::Direction_Forward;
	case O3DS::Direction::Back: return O3DS::Data::Direction_Back;
	}
	return O3DS::Data::Direction_None;
}

enum O3DS::Direction dir(O3DS::Data::Direction d)
{
	switch (d)
	{
	case O3DS::Data::Direction_Up: return O3DS::Direction::Up;
	case O3DS::Data::Direction_Down: return O3DS::Direction::Down;
	case O3DS::Data::Direction_Left: return O3DS::Direction::Left;
	case O3DS::Data::Direction_Right: return O3DS::Direction::Right;
	case O3DS::Data::Direction_Forward: return O3DS::Direction::Forward;
	case O3DS::Data::Direction_Back: return O3DS::Direction::Back;
	}
	return O3DS::Direction::None;
}

namespace O3DS
{

	// Transform 

	Transform::Transform(const std::string& name, int parentId, void *ref)
		: bWorldMatrix(false)
		, mName(name)
		, mParentId(parentId)
		, mReference(ref)	
	{}

	Transform::Transform(int parentId)
		: bWorldMatrix(false)
		, mName()
		, mParentId(parentId)
		, mReference(nullptr)
	{}

	Transform::Transform()
		: bWorldMatrix(false)
		, mName()
		, mParentId(-1)
		, mReference(nullptr)
	{}

	Transform::~Transform()
	{};

	bool Transform::nan()
	{
		if (mMatrix.HasNan()) return true;
		if (mWorldMatrix.HasNan()) return true;
		if (translation.value.v[0] != translation.value.v[0]) return true;
		if (translation.value.v[1] != translation.value.v[1]) return true;
		if (translation.value.v[2] != translation.value.v[2]) return true;
		if (rotation.value.v[0] != rotation.value.v[0]) return true;
		if (rotation.value.v[1] != rotation.value.v[1]) return true;
		if (rotation.value.v[2] != rotation.value.v[2]) return true;
		if (rotation.value.v[3] != rotation.value.v[3]) return true;
		if (scale.value.v[0] != scale.value.v[0]) return true;
		if (scale.value.v[1] != scale.value.v[1]) return true;
		if (scale.value.v[2] != scale.value.v[2]) return true;

		for (const auto& i : matrices) {
			if(i.value.HasNan()) { return true; }
		}
		return false;
	}

	bool Subject::CalcMatrices()
	{
		for (auto& transform : this->mTransforms)
		{
			transform->bWorldMatrix = false;
			auto &m = transform->mMatrix;
			m = Matrixd();

			if(m.HasNan())
			{
				mError = "Matrix NAN";
				return false;
			}

			int matrixId = 0;

			for (auto op : transform->transformOrder)
			{
				if (op == O3DS::TTranslation)
				{
					m = Matrixd::TranslateXYZ(transform->translation.value) * m;
				}
				if (op == O3DS::TRotation)
				{
					m = transform->rotation.asMatrix() * m;
				}
				if (op == O3DS::TScale)
				{
					m = m.Scale(transform->scale.value) * m;
				}
				if (op == O3DS::TMatrix)
				{
					m = transform->matrices[matrixId++].value * m;
				}
			}
		}

		// Calculate world matrix

		// Find the root first
		int rootCount = 0;	
		for(auto transform : this->mTransforms) {
			if (transform->mParentId == -1)
			{
				// No Parent - matrix is world matrix
				transform->mWorldMatrix = transform->mMatrix;
				transform->bWorldMatrix = true;
				rootCount++;
			}
		}

		if(rootCount == 0)
		{
			mError = "Could not find a root";
			return false;
		}
		if(rootCount > 1)
		{
			mError = "More than one root found";
			return false;
		}

		bool done = false;
		while (!done)
		{
			// Assume we are done, and flag as not done when we do work
			done = true;
			for (int transformId = 0; transformId < this->mTransforms.size(); transformId++)
			{
				auto transform = this->mTransforms[transformId];
				if (transform->bWorldMatrix) {
					 continue;
				}

				if (transformId == transform->mParentId)
				{
					std::ostringstream oss;
					oss << "ParentId of " << transform->mName << " points to self (" << transformId << ")";
					mError = oss.str();
					return false;
				}

				if (transform->mParentId < 0 || transform->mParentId > this->mTransforms.size())
				{
					mError = "Invalid Parent Id";
					return false;
				}

				auto& parentTransform = this->mTransforms.mItems[transform->mParentId];
				if (!parentTransform->bWorldMatrix)
				{
					// Parent has not been calculated yet
					continue;
				}

				transform->mWorldMatrix = transform->mMatrix * parentTransform->mWorldMatrix;
				transform->bWorldMatrix = true;
				done = false;
			}
		}

		return true;
	}



	// Subject

	flatbuffers::Offset<O3DS::Data::Subject> Subject::Serialize(flatbuffers::FlatBufferBuilder& builder)
	{
		
		auto oSubjectName = builder.CreateString(this->mName);
		auto oFormat = builder.CreateString(this->mContext.mFormat);
		std::vector<flatbuffers::Offset<O3DS::Data::Transform>> ovSkeleton;

		O3DS::Data::Translation translation;
		O3DS::Data::Rotation rotation;
		O3DS::Data::Scale scale;

		for (auto& t : this->mTransforms) {
			int matrixId = 0;

			std::vector<O3DS::Data::Matrix> matrices;
			std::vector<int8_t> components;

			t->translation >> translation;
			t->rotation >> rotation;
			t->scale >> scale;

			for (const auto component : t->transformOrder) {
				if (component == O3DS::TTranslation)
				components.push_back(O3DS::Data::Component::Component_Translation);

				if (component == O3DS::TRotation)
				components.push_back(O3DS::Data::Component::Component_Rotation);

				if (component == O3DS::TScale)
				components.push_back(O3DS::Data::Component::Component_Scale);

				if (component == O3DS::TMatrix) {
					components.push_back(O3DS::Data::Component::Component_Matrix);
					O3DS::Data::Matrix matrix;
					t->matrices[matrixId++] >> matrix;
					matrices.push_back(matrix);
				}
			}

			auto oTransformName = builder.CreateString(t->mName);

			// flatbuffers::Offset<flatbuffers::Vector<const O3DS::Data::Matrix *>> oatrices;
			auto ovMatrices = builder.CreateVectorOfStructs(matrices);

			auto ovComponents = builder.CreateVector(components);

			ovSkeleton.push_back(CreateTransform(builder, t->mParentId, oTransformName,
											&translation, &rotation, &scale,
											ovMatrices, ovComponents));
		}

		auto transforms = builder.CreateVector(ovSkeleton);
		return CreateSubject(builder, transforms, oSubjectName,
						dir(this->mContext.mX), dir(this->mContext.mY),
						dir(this->mContext.mZ), oFormat);
	}

	flatbuffers::Offset<O3DS::Data::SubjectUpdate> Subject::SerializeUpdate(flatbuffers::FlatBufferBuilder& builder, size_t &count, double deltaThreshold)
	{
		auto oSubjectName = builder.CreateString(this->mName);

		std::vector<O3DS::Data::TranslationUpdate> translations;
		std::vector<O3DS::Data::RotationUpdate> rotations;
		std::vector<O3DS::Data::ScaleUpdate> scales;

		int transformId = 0;

		for (const auto& t : this->mTransforms)
		{
			if (t->nan())
			{
				continue;
			}
			if (t->translation.delta() > deltaThreshold)
			{
				translations.push_back(O3DS::Data::TranslationUpdate(
					(float)t->translation.value.v[0],
					(float)t->translation.value.v[1],
					(float)t->translation.value.v[2], transformId));
				t->translation.sent();
				count++;
			}

			if (t->rotation.delta() > deltaThreshold)
			{
				rotations.push_back(O3DS::Data::RotationUpdate(
					(float)t->rotation.value.v[0],
					(float)t->rotation.value.v[1],
					(float)t->rotation.value.v[2],
					(float)t->rotation.value.v[3], transformId));
				t->rotation.sent();
				count++;
			}

			/*
		if (t->scale.delta() > 0.001)
		{
			scales.push_back(O3DS::Data::ScaleUpdate(
				(float)t->scale.value.v[0],
				(float)t->scale.value.v[1],
				(float)t->scale.value.v[2], transformId));
			t->scale.sent();
		}*/

			transformId++;
		}

		auto tr = builder.CreateVectorOfStructs(translations);
		auto ro = builder.CreateVectorOfStructs(rotations);
		auto sc = builder.CreateVectorOfStructs(scales);
		return CreateSubjectUpdate(builder, oSubjectName, tr, ro, sc);
	}

	int Subject::Serialize(std::vector<char> &outbuf, double timestamp)
	{
		if (timestamp == 0.0) timestamp = GetTime();
		flatbuffers::FlatBufferBuilder builder;

		std::vector<flatbuffers::Offset<O3DS::Data::Subject> > subjects;
		
		flatbuffers::Offset<O3DS::Data::Subject> s = this->Serialize(builder);
		subjects.push_back(s);

		auto ovSubjects = builder.CreateVector(subjects);

		auto root = CreateSubjectList(builder, ovSubjects, 0, timestamp);

		builder.Finish(root);

		finalize(builder, outbuf, 1);

		return outbuf.size();
	}

	int Subject::SerializeUpdate(std::vector<char>& outbuf, size_t& count, double deltaThreshold, double timestamp)
	{
		if (timestamp == 0.0)
		{
			timestamp = GetTime();
		}

		flatbuffers::FlatBufferBuilder builder;

		std::vector<flatbuffers::Offset<O3DS::Data::SubjectUpdate>> outSubjectUpdates;
		outSubjectUpdates.push_back(this->SerializeUpdate(builder, count, deltaThreshold));

		auto ovSubjectUpdates = builder.CreateVector(outSubjectUpdates);

		auto root = CreateSubjectList(builder, 0, ovSubjectUpdates, timestamp);

		builder.Finish(root);

		finalize(builder, outbuf, 1);

		return outbuf.size();
	}

	int SubjectList::Serialize(std::vector<char> &outbuf, double timestamp)
	{	
		if(timestamp == 0.0) timestamp = GetTime();

		flatbuffers::FlatBufferBuilder builder;

		std::vector<flatbuffers::Offset<O3DS::Data::Subject> > subjects;

		for (O3DS::Subject* subject : this->mItems)
		{
			flatbuffers::Offset<O3DS::Data::Subject> s = subject->Serialize(builder);
			subjects.push_back(s);
		}

		auto ovSubjects = builder.CreateVector(subjects);

		auto root = CreateSubjectList(builder, ovSubjects, 0, timestamp);

		builder.Finish(root);

		finalize(builder, outbuf, 1);

		return outbuf.size();
	}

	void finalize(flatbuffers::FlatBufferBuilder& builder, std::vector<char>& outbuf, std::uint32_t flags)
	{
		outbuf.resize(0);

		uint8_t* buf = builder.GetBufferPointer();
		int size = builder.GetSize();

		// Flags
		//std::uint32_t flags = 0x0001;
		const char* flagptr = (const char*)&flags;
		std::copy(flagptr, flagptr + 4, back_inserter(outbuf));

		// Checksum
		std::uint32_t crc = CRCPP::CRC::Calculate(buf, size, CRCPP::CRC::CRC_32());
		const char* crcptr = (const char*)&crc;
		std::copy(crcptr, crcptr + 4, back_inserter(outbuf));

		// Data
		std::copy(buf, buf + size, back_inserter(outbuf));
	}



	// Subject List

	int SubjectList::SerializeUpdate(std::vector<char> &outbuf, size_t& count, double timestamp)
	{
		if (timestamp == 0.0)
		{
			timestamp = GetTime();
		}

		flatbuffers::FlatBufferBuilder builder;

		std::vector<flatbuffers::Offset<O3DS::Data::SubjectUpdate>> outSubjectUpdates;

		for (auto& subject : this->mItems)
		{			
			outSubjectUpdates.push_back(subject->SerializeUpdate(builder, count, mDeltaThreshold));
		}

		auto ovSubjectUpdates = builder.CreateVector(outSubjectUpdates);

		auto root = CreateSubjectList(builder, 0, ovSubjectUpdates, timestamp);

		builder.Finish(root);

		finalize(builder, outbuf, 1);

		return outbuf.size();
	}

	bool SubjectList::Parse(const char *data, size_t len, TransformBuilder *builder)
	{
		std::uint32_t crc = CRCPP::CRC::Calculate(data + 8, len - 8, CRCPP::CRC::CRC_32());

		std::uint32_t flags = *(std::uint32_t*)data;
		std::uint32_t check = *(std::uint32_t*)(data + 4);

		mError = "";

		if (flags != 0x0001) {
			mError = "Invalid data structure";
			return false;
		}

		if (crc != check) {
			mError = "CRC Check failed";
			return false;
		}

		auto root = GetSubjectList(data+8);

		this->mTime = root->time();

		auto subjects_data = root->subjects();
		auto updates_data = root->updates();

		auto ovSubjects = root->subjects();

		if (subjects_data)
		{
			for (uint32_t i = 0; i < subjects_data->size(); i++)
			{
				// For each subject
				this->ParseSubject(subjects_data->Get(i), builder);
			}
		}

		if (updates_data)
		{
			for (uint32_t i = 0; i < updates_data->size(); i++)
			{
				// For each update
				this->ParseUpdate(updates_data->Get(i), builder);
			}
		}

		for (auto subject : this->mItems) {
			if(!subject->CalcMatrices()) {
				mError = subject->mError;
				return false;
			}
		}

		return true;
	}

	void SubjectList::ParseSubject(const O3DS::Data::Subject *inSubject, TransformBuilder *builder)
	{
		std::string subjectName = inSubject->name()->str();

		// Check to see if this subject already exists
		Subject *outSubject = this->findSubject(subjectName);
		if (outSubject == nullptr)
		{
			// Add it
			outSubject = this->addSubject(subjectName);
		}

		outSubject->mContext.mX = dir(inSubject->x_axis());
		outSubject->mContext.mY = dir(inSubject->y_axis());
		outSubject->mContext.mZ = dir(inSubject->z_axis());
		outSubject->mContext.mFormat = inSubject->format()->str();

		// Get the nodes (transforms) for this subject
		auto ovNodes = inSubject->nodes();
			
		// Clear the subject and add the transfoms
		outSubject->clear();
		for (int n = 0; n < (int)ovNodes->size(); n++)
		{
			auto inNode = ovNodes->Get(n);
			auto inName = inNode->name();
			auto inTranslation = inNode->translation();
			auto inRotation = inNode->rotation();
			auto inScale = inNode->scale();
			auto inMatrix = inNode->matrix();
			auto inComponents = inNode->components();

			auto inMatrixIter = inMatrix->begin();

			std::string transformName = inName->str();
			Transform *outTransform = outSubject->addTransform(transformName, inNode->parent());

			// Add the components to the transform stack in the order they are defined.
			for (int8_t componentId : *inComponents)
			{
				if (componentId == O3DS::Data::Component::Component_Translation)
				{
					*inTranslation >> outTransform->translation;
					outTransform->transformOrder.push_back(O3DS::TTranslation);
				}
				if (componentId == O3DS::Data::Component::Component_Rotation)
				{
					*inRotation >> outTransform->rotation;
					outTransform->transformOrder.push_back(O3DS::TRotation);
				}
				if (componentId == O3DS::Data::Component::Component_Scale)
				{
					*inScale >> outTransform->scale;
					outTransform->transformOrder.push_back(O3DS::TScale);
				}
				if (componentId == O3DS::Data::Component::Component_Matrix)
				{
					auto transformMatrix = O3DS::TransformMatrix();
					**inMatrixIter++ >> transformMatrix;
					outTransform->matrices.push_back(transformMatrix);
					outTransform->transformOrder.push_back(O3DS::TMatrix);
				}
			}
		}
	}

	void SubjectList::ParseUpdate(
		const O3DS::Data::SubjectUpdate *inUpdate,
		TransformBuilder *builder)
	{
		std::string name = inUpdate->name()->str();
		int id;

		// Find the subject to update, by name
		O3DS::Subject *outSubject = this->findSubject(name);
		if (!outSubject)
			return;

		// Update TRS

		for (auto inTranslation : *inUpdate->translations())
		{
			id = inTranslation->i();
			if (id < outSubject->mTransforms.size())
				*inTranslation >> outSubject->mTransforms[id]->translation;
			else
				break;
		}

		for (auto inRotation : *inUpdate->rotation())
		{
			id = inRotation->i();
			if (id < outSubject->mTransforms.size())
				*inRotation >> outSubject->mTransforms[id]->rotation;
			else
				break;
		}

		for (auto inScale : *inUpdate->scale())
		{
			id = inScale->i();
			if (id < outSubject->mTransforms.size())
				*inScale >> outSubject->mTransforms[id]->scale;
			else
				break;
		}
	}



} // namespace O3DS

/*
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
}*/

