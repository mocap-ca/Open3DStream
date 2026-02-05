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
#include "math.h"
#include "model.h"
#include "getTime.h"
#include "CRC.h"
#include <algorithm>
#include <iterator>
#include <sstream>
#include <iostream>

//////////////////////////////////
// Operators

void operator >>(const O3DS::TransformTranslation& src, O3DS::Data::Translation &dst)
{
	dst = O3DS::Data::Translation(
		(float)src.value.x(),
		(float)src.value.y(),
		(float)src.value.z());
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
		(float)src.value.x(),
		(float)src.value.y(),
		(float)src.value.z(),
		(float)src.value.w());
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
		(float)src.value.x(),
		(float)src.value.y(),
		(float)src.value.z());
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
		static_cast<float>(src.value(0, 0)),
		static_cast<float>(src.value(0, 1)),
		static_cast<float>(src.value(0, 2)),
		static_cast<float>(src.value(0, 3)),

		static_cast<float>(src.value(1, 0)),
		static_cast<float>(src.value(1, 1)),
		static_cast<float>(src.value(1, 2)),
		static_cast<float>(src.value(1, 3)),

		static_cast<float>(src.value(2, 0)),
		static_cast<float>(src.value(2, 1)),
		static_cast<float>(src.value(2, 2)),
		static_cast<float>(src.value(2, 3)),

		static_cast<float>(src.value(3, 0)),
		static_cast<float>(src.value(3, 1)),
		static_cast<float>(src.value(3, 2)),
		static_cast<float>(src.value(3, 3))
	);
}

void operator>>(const O3DS::Data::Matrix& src,
	O3DS::TransformMatrix& dst)
{
	dst.value <<
		src.m00(), src.m01(), src.m02(), src.m03(),
		src.m10(), src.m11(), src.m12(), src.m13(),
		src.m20(), src.m21(), src.m22(), src.m23(),
		src.m30(), src.m31(), src.m32(), src.m33();
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
	/////////////////////////////////////////
	// Transform 

	Transform::Transform(const std::string& name, int parentId, void *ref)
		: translation(0, 0, 0)
		, rotation(Eigen::Quaterniond::Identity())
		, scale(1, 1, 1)
		, mMatrix(Matrix::Identity())
		, mWorldMatrix(Matrix::Identity())
		, bWorldMatrix(false)
		, mName(name)
		, mParentId(parentId)
		, mReference(ref)
	{}

	Transform::Transform(int parentId)
		: translation(0, 0, 0)
		, rotation(Eigen::Quaterniond::Identity())
		, scale(1, 1, 1)
		, mMatrix(Matrix::Identity())
		, mWorldMatrix(Matrix::Identity())
		, bWorldMatrix(false)
		, mName()
		, mParentId(parentId)
		, mReference(nullptr)
	{}

	Transform::Transform()
		: translation(0, 0, 0)
		, rotation(Eigen::Quaterniond::Identity())
		, scale(1, 1, 1)
		, mMatrix(Matrix::Identity())
		, mWorldMatrix(Matrix::Identity())
		, bWorldMatrix(false)
		, mName()
		, mParentId(-1)
		, mReference(nullptr)
	{}

	Transform::Transform(const Transform& other)
	{
		//translation.value = other.translation.value.eval();	
		translation.value.x() = other.translation.value.x();
		translation.value.y() = other.translation.value.y();
		translation.value.z() = other.translation.value.z();

		translation.lastSentValue = Eigen::Vector3d::Zero();
		rotation.value = Eigen::Quaterniond(other.rotation.value);
		rotation.lastSentValue = Eigen::Quaterniond::Identity();

		scale.value = other.scale.value.eval();
		scale.lastSentValue = Eigen::Vector3d::Zero();

		mMatrix = other.mMatrix.eval();

		mWorldMatrix = other.mMatrix.eval();

		bWorldMatrix = other.bWorldMatrix;

		for(auto &m : other.matrices) {
			matrices.push_back(TransformMatrix(m.value.eval()));
		}

		for(auto &op : other.transformOrder) {
			transformOrder.push_back(op);
		}

		mName = other.mName;
		mParentId = other.mParentId;
		mReference = nullptr; // probably not a good idea to copy this.
	}

	Transform::~Transform()
	{};

	void Transform::update()
	{
		// No implementation here

		bWorldMatrix = false;

		mMatrix = Matrix::Identity();

		int matrixId = 0;

		for (auto op : transformOrder)
		{
			if (op == O3DS::TTranslation)
			{
				mMatrix = mMatrix * translate(translation.value);
			}
			if (op == O3DS::TRotation)
			{
				mMatrix = mMatrix * rotation.asMatrix();
			}
			if (op == O3DS::TScale)
			{
				mMatrix = mMatrix * O3DS::scale(scale.value);
			}
			if (op == O3DS::TMatrix)
			{
				mMatrix = mMatrix * matrices[matrixId++].value;
			}
		}
	}

	bool Transform::allFinite()
	{
		if (!mMatrix.allFinite())      return false;
		if (!mWorldMatrix.allFinite()) return false;

		if (!translation.value.allFinite()) return false;
		if (!scale.value.allFinite())       return false;

		if (!rotation.value.coeffs().allFinite()) return false;
		// coeffs() = (x, y, z, w)

		for (const auto& i : matrices)
		{
			if (!i.value.allFinite())
				return false;
		}

		return true;
	}

	bool Transform::operator==(const Transform &other) const
	{
		if (this->mName != other.mName) return false;
		if (this->mParentId != other.mParentId) return false;

		if (this->translation.value != other.translation.value) return false;

		auto a = this->rotation.value.normalized();
		auto b = other.rotation.value.normalized();
		auto d = a.angularDistance(b);
		if (d > 1e-6) { return false; }

		if (this->scale.value != other.scale.value) return false;

		if (this->transformOrder != other.transformOrder) return false;

		if (this->matrices.size() != other.matrices.size()) return false;
		for (size_t i = 0; i < this->matrices.size(); i++)
		{
			if (this->matrices[i].value != other.matrices[i].value)
				return false;
		}

		return true;
	}

	///////////////////////////////
	// Transform List

	TransformList::~TransformList()
	{
		mItems.clear();
	}

	size_t TransformList::size() const
	{
		return mItems.size();
	}	

	void TransformList::clear() {
		mItems.clear();	
	}

	void TransformList::update()
	{
		for (auto& i : mItems)
			i->update();
	}

	TransformList::iterator TransformList::begin() { return { mItems.begin() }; }
	TransformList::iterator TransformList::end() { return { mItems.end() }; }

	Transform* TransformList::at(size_t id)
	{
		if (id >= mItems.size()) return nullptr;
		return mItems[id].get();
	}

	Transform* TransformList::operator[](size_t id) { return mItems[id].get(); }

	Transform* TransformList::find(const std::string& name)
	{
		for (size_t i = 0; i < mItems.size(); i++)
		{
			if (mItems[i]->mName == name)
				return mItems[i].get();
		}
		return nullptr;
	}

	bool TransformList::operator ==(const TransformList& other) const
	{
		if (mItems.size() != other.mItems.size())
			return false;

		// TODO - allow for out of order compare of lists
		for (size_t i = 0; i < mItems.size(); i++)
		{
			if (mItems[i]->operator==(*other.mItems[i]) == false) {
				return false;
			}
		}
		return true;
	}

	// Subject

	Subject::Subject(void* info)
		: mReference(info)
		, mEnabled(true)
	{}

	Subject::Subject(std::string name, std::string uuid, void* info)
		: mName(name)
		, mReference(info)
		, mUuid(uuid)
		, mEnabled(true)
	{}

	bool Subject::allFinite()
	{
		for (auto i : mTransforms) {
			if (!i->allFinite()) {
				mError = "Invalid transform: " + i->mName;
				return false;
			}
		}
		return true;
	}

	Transform* Subject::addTransform(const std::string& name, int parentId, TransformBuilder* builder)
	{
		std::unique_ptr<Transform> transform;
		Transform* ret = nullptr;
		if (builder) transform = builder->build(name, parentId);
		else         transform = std::make_unique<Transform>(name, parentId);
		ret = transform.get();
		mTransforms.mItems.push_back(std::move(transform));
		return ret;
	}

	void Subject::addTransform(std::unique_ptr<Transform> item)
	{
		mTransforms.mItems.push_back(std::move(item));
	}

	void Subject::clearTransforms()
	{
		mTransforms.clear();	
	}

	void Subject::clearAll()
	{
		mTransforms.clear();
		mName.clear();
		mUuid.clear();
		mJoints.clear();
		mReference = nullptr;
		mError.clear();
	}

	void Subject::update()
	{
		mTransforms.update();
	}

	size_t Subject::size()
	{
		return mTransforms.mItems.size();
	}

	bool Subject::CalcMatrices()
	{
		for (Transform* transform : this->mTransforms)
		{
			if (!transform->allFinite())
			{
				mError = "Invalid xform: " + transform->mName;
				return false;
			}

			transform->update();
			
			if (!transform->allFinite()) {
				mError = "Bad calc for: " + transform->mName;
				return false;
			}
		}

		// Calculate world matrix

		// Find the root first
		int rootCount = 0;	
		for(Transform* transform : this->mTransforms) {
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
		int iterations = 0;
		while (!done)
		{
			if (++iterations > this->mTransforms.size())
			{
				mError = "Cycle detected in transform hierarchy";
				return false;
			}

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

				if (transform->mParentId < 0 || transform->mParentId >= this->mTransforms.size())
				{
					mError = "Invalid Parent Id";
					return false;
				}

				auto parentTransform = this->mTransforms[transform->mParentId];
				if (!parentTransform->bWorldMatrix)
				{
					// Parent has not been calculated yet - will get on next outer loop
					continue;
				}

				transform->mWorldMatrix = parentTransform->mWorldMatrix * transform->mMatrix;
				transform->bWorldMatrix = true;

				if (!transform->mWorldMatrix.allFinite()) {
					mError = "WM Error: " + transform->mName;
					return false;
				}
				done = false;
			}
		}

		return true;
	}

	flatbuffers::Offset<O3DS::Data::Subject> Subject::Serialize(flatbuffers::FlatBufferBuilder& builder)
	{
		auto oSubjectName = builder.CreateString(this->mName);
		auto oSubjectUuid = builder.CreateString(this->mUuid);
		auto oFormat = builder.CreateString(this->mContext.mFormat);
		std::vector<flatbuffers::Offset<O3DS::Data::Transform>> ovSkeleton;

		O3DS::Data::Translation translation;
		O3DS::Data::Rotation rotation;
		O3DS::Data::Scale scale;

		for (Transform* t : this->mTransforms) {
			int matrixId = 0;

			std::vector<O3DS::Data::Matrix> matrices;
			std::vector<int8_t> components;

			t->translation >> translation;
			t->rotation >> rotation;
			t->scale >> scale;

			if (!t->translation.value.allFinite()) {
								std::cerr << "ERROR" << std::endl;
			}
			if (!t->rotation.value.coeffs().allFinite()) {
								std::cerr << "ERROR" << std::endl;
			}

			t->translation.sent();
			t->rotation.sent();

			for (const auto component : t->transformOrder) {
				if (component == O3DS::TTranslation) {
					components.push_back(O3DS::Data::Component::Component_Translation);
				}

				if (component == O3DS::TRotation) {
					components.push_back(O3DS::Data::Component::Component_Rotation);
				}

				if (component == O3DS::TScale) {
					components.push_back(O3DS::Data::Component::Component_Scale);
				}

				if (component == O3DS::TMatrix) {
					components.push_back(O3DS::Data::Component::Component_Matrix);
				}
			}

			for (int i = 0; i < t->matrices.size(); i++) {
				// Copy all matrices.  This allows embedding other data (offsets)
				O3DS::Data::Matrix matrix;
				t->matrices[matrixId++] >> matrix;
				matrices.push_back(matrix);
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
						dir(this->mContext.mZ), oFormat, oSubjectUuid);
	}

	flatbuffers::Offset<O3DS::Data::SubjectUpdate> Subject::SerializeUpdate(flatbuffers::FlatBufferBuilder& builder, size_t &count, double deltaThreshold)
	{
		auto oSubjectName = builder.CreateString(this->mName);
		auto oSubjectUuid = builder.CreateString(this->mUuid);

		std::vector<O3DS::Data::TranslationUpdate> translations;
		std::vector<O3DS::Data::RotationUpdate> rotations;
		std::vector<O3DS::Data::ScaleUpdate> scales;

		for (int transformId=0; transformId < this->mTransforms.size(); transformId++)
		{
			const auto& t = this->mTransforms[transformId];

			if (!t->allFinite())
			{
				mError = "Transform has non-finite values: " + t->mName;
				continue;
			}
			if (t->translation.delta() > deltaThreshold)
			{
				translations.push_back(O3DS::Data::TranslationUpdate(
					(float)t->translation.value.x(),
					(float)t->translation.value.y(),
					(float)t->translation.value.z(), transformId));
				t->translation.sent();
				count++;
			}

			auto delta = t->rotation.delta();
			if (delta > deltaThreshold)
			{
				rotations.push_back(O3DS::Data::RotationUpdate(
					(float)t->rotation.value.x(),
					(float)t->rotation.value.y(),
					(float)t->rotation.value.z(),
					(float)t->rotation.value.w(), transformId));
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

		}

		auto tr = builder.CreateVectorOfStructs(translations);
		auto ro = builder.CreateVectorOfStructs(rotations);
		auto sc = builder.CreateVectorOfStructs(scales);
		return CreateSubjectUpdate(builder, oSubjectName, tr, ro, sc, oSubjectUuid);
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

		return static_cast<int>(outbuf.size());
	}

	//////////////////////////////
	// Subject List

	SubjectList::SubjectList()
		: mTime(0.0)
		, mDeltaThreshold(1e-6 /*std::numeric_limits<double>::min()*/)
	{}

	SubjectList::~SubjectList()
	{
		mItems.clear();
	}

	bool SubjectList::allFinite()
	{
		for (const auto& s : mItems)
		{
			if (!s->allFinite()) {
				mError = s->mError;
				return false;
			}
		}
		return true;
	}

	Subject* SubjectList::addSubject(std::string name, std::string uuid, void* ref)
	{
		auto s = std::make_unique<Subject>(name, uuid, ref);
		Subject* sptr = s.get();
		mItems.push_back(std::move(s));
		return sptr;
	}

	Subject* SubjectList::findSubjectByName(const std::string& name)
	{
		for (auto& i : mItems)
		{
			if (i->mName == name)
				return i.get();
		}
		return nullptr;
	}

	Subject* SubjectList::findSubjectByUuid(const std::string& uuid)
	{
		for (auto& i : mItems)
		{
			if (i->mUuid == uuid)
				return i.get();
		}
		return nullptr;
	}

	void SubjectList::update()
	{
		for (auto& i : mItems)
		{
			i->update();
		}
	}

	int Subject::SerializeUpdate(std::vector<char>& outbuf, size_t& count, double deltaThreshold, double timestamp)
	{
		if (!allFinite()) {
			// All finite should set the error with the invalid joint name
			return 0;
		}

		if (timestamp == 0.0) { timestamp = GetTime(); }

		flatbuffers::FlatBufferBuilder builder;

		std::vector<flatbuffers::Offset<O3DS::Data::SubjectUpdate>> outSubjectUpdates;
		outSubjectUpdates.push_back(this->SerializeUpdate(builder, count, deltaThreshold));

		auto ovSubjectUpdates = builder.CreateVector(outSubjectUpdates);

		auto root = CreateSubjectList(builder, 0, ovSubjectUpdates, timestamp);

		builder.Finish(root);

		finalize(builder, outbuf, 2);

		return static_cast<int>(outbuf.size());
	}

	int SubjectList::Serialize(std::vector<char> &outbuf, double timestamp)
	{	
		if (!allFinite()) {
			// All finite should set the error with the invalid joint name
			return 0;
		}

		if(timestamp == 0.0) timestamp = GetTime();

		flatbuffers::FlatBufferBuilder builder;

		std::vector<flatbuffers::Offset<O3DS::Data::Subject> > subjects;

		for (const auto& subject : mItems)
		{
			if (subject->mEnabled) {
				flatbuffers::Offset<O3DS::Data::Subject> s = subject->Serialize(builder);
				subjects.push_back(s);
			}
		}

		auto ovSubjects = builder.CreateVector(subjects);

		auto root = CreateSubjectList(builder, ovSubjects, 0, timestamp);

		builder.Finish(root);

		finalize(builder, outbuf, 1);

		return static_cast<int>(outbuf.size());
	}



	int SubjectList::SerializeUpdate(std::vector<char> &outbuf, size_t& count, double timestamp)
	{
		if (!this->allFinite()) {
			// All finite should set the error with the invalid joint name
			return 0;
		}

		if (timestamp == 0.0) { timestamp = GetTime(); }

		flatbuffers::FlatBufferBuilder builder;

		std::vector<flatbuffers::Offset<O3DS::Data::SubjectUpdate>> outSubjectUpdates;

		for (auto& subject : this->mItems)
		{			
			if (subject->mEnabled) {
				outSubjectUpdates.push_back(subject->SerializeUpdate(builder, count, mDeltaThreshold));
			}
		}

		auto ovSubjectUpdates = builder.CreateVector(outSubjectUpdates);

		auto root = CreateSubjectList(builder, 0, ovSubjectUpdates, timestamp);

		builder.Finish(root);

		finalize(builder, outbuf, 2);

		return static_cast<int>(outbuf.size());
	}



	bool SubjectList::Parse(const char *data, size_t len, TransformBuilder *builder, bool clearInactive)
	{
		if (len < 8) {
			mError = "Buffer too small";
			return false;
		}
	
		std::uint32_t crc = CRC::Calculate(data + 8, len - 8, CRC::CRC_32());

		std::uint32_t flags;
		std::uint32_t check;
		std::memcpy(&flags, data, 4);
		std::memcpy(&check, data + 4, 4);

		mError = "";

		if (flags != 0x0001 && flags != 0x0002) {
			mError = "Invalid data structure";
			return false;
		}

		if (crc != check) {
			mError = "CRC Check failed";
			return false;
		}

		const uint8_t* fb = reinterpret_cast<const uint8_t*>(data + 8);
		size_t fb_len = len - 8;

		flatbuffers::Verifier verifier(fb, fb_len);
		if (!verifier.VerifyBuffer<O3DS::Data::SubjectList>()) {
			mError = "FlatBuffer verification failed";
			return false;
		}


		auto root = O3DS::Data::GetSubjectList(fb);

		this->mTime = root->time();

		auto subjects_data = root->subjects();
		auto updates_data = root->updates();

		if (subjects_data)
		{
			// Clear the list before populating
			if (clearInactive) {
				this->mItems.clear();
			}
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

		if (!allFinite()) {
			// All finite should set the error with the invalid joint name
			return false;
		}

		for (const auto& subject : mItems) {
			if(!subject->CalcMatrices()) {
				mError = subject->mError;
				return false;
			}
		}

		return true;
	}

	void SubjectList::ParseSubject(const O3DS::Data::Subject *inSubject,  TransformBuilder *builder )
	{
		std::string subjectName = inSubject->name()->str();
		std::string subjectUuid = inSubject->uuid()->str();

		// Check to see if this subject already exists
		Subject *outSubject = this->findSubjectByUuid(subjectUuid);
		if (outSubject == nullptr)
		{
			// Add it
			outSubject = this->addSubject(subjectName, subjectUuid);
		}
		else
		{
			outSubject->clearTransforms();
		}

		outSubject->mName = subjectName;
		outSubject->mUuid = subjectUuid;
		outSubject->mContext.mX = dir(inSubject->x_axis());
		outSubject->mContext.mY = dir(inSubject->y_axis());
		outSubject->mContext.mZ = dir(inSubject->z_axis());
		outSubject->mContext.mFormat = inSubject->format()->str();

		// Get the nodes (transforms) for this subject
		auto ovNodes = inSubject->nodes();
			
		// Clear the subject and add the transforms
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
					outTransform->transformOrder.push_back(O3DS::TMatrix);
				}
			}

			// Copy all matrices, allows adding other matrix data to be used as offsets
			for (auto eachMatrix : *inMatrix) {
				auto transformMatrix = O3DS::TransformMatrix();
				*eachMatrix >> transformMatrix;
				outTransform->matrices.push_back(transformMatrix);
			}
		}
	}

	void SubjectList::ParseUpdate(
		const O3DS::Data::SubjectUpdate *inUpdate,
		TransformBuilder *builder)
	{
		std::string name = inUpdate->name()->str();
		std::string uuid = inUpdate->uuid()->str();
		int id;

		// Find the subject to update, by name
		O3DS::Subject *outSubject = this->findSubjectByUuid(uuid);
		if (!outSubject)
			return;

		// Update TRS

		for (const auto& inTranslation : *inUpdate->translations())
		{
			id = inTranslation->i();
			if (id < outSubject->mTransforms.size()) {
				*inTranslation >> outSubject->mTransforms[id]->translation;
			}
		}

		for (auto inRotation : *inUpdate->rotation())
		{
			id = inRotation->i();
			if (id < outSubject->mTransforms.size()) {
				*inRotation >> outSubject->mTransforms[id]->rotation;
			}
		}

		for (const auto& inScale : *inUpdate->scale())
		{
			id = inScale->i();
			if (id < outSubject->mTransforms.size()) {
				*inScale >> outSubject->mTransforms[id]->scale;
			}
		}
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
		std::uint32_t crc = CRC::Calculate(buf, size, CRC::CRC_32());
		const char* crcptr = (const char*)&crc;
		std::copy(crcptr, crcptr + 4, back_inserter(outbuf));

		// Data
		std::copy(buf, buf + size, back_inserter(outbuf));
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

