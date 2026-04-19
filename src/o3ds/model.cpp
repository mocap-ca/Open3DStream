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
	return O3DS::Data::Direction_Unknown;
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
	return O3DS::Direction::Unknown;
}

O3DS::Data::DistanceUnit distUnit(enum O3DS::DistanceUnit d)
{
	switch (d)
	{
	case O3DS::DistanceUnit::Millimeter: return O3DS::Data::DistanceUnit_Millimeter;
	case O3DS::DistanceUnit::Centimeter: return O3DS::Data::DistanceUnit_Centimeter;
	case O3DS::DistanceUnit::Meter: return O3DS::Data::DistanceUnit_Meter;
	case O3DS::DistanceUnit::Kilometer: return O3DS::Data::DistanceUnit_Kilometer;
	case O3DS::DistanceUnit::Inch: return O3DS::Data::DistanceUnit_Inch;
	case O3DS::DistanceUnit::Foot: return O3DS::Data::DistanceUnit_Foot;
	case O3DS::DistanceUnit::Mile: return O3DS::Data::DistanceUnit_Mile;
	case O3DS::DistanceUnit::LightYear: return O3DS::Data::DistanceUnit_LightYear;
	case O3DS::DistanceUnit::Parsec: return O3DS::Data::DistanceUnit_Parsec;
	}
	return O3DS::Data::DistanceUnit_Unknown;
}	

enum O3DS::DistanceUnit distUnit(O3DS::Data::DistanceUnit d)
{
	switch (d)
	{
	case O3DS::Data::DistanceUnit_Millimeter: return O3DS::DistanceUnit::Millimeter;
	case O3DS::Data::DistanceUnit_Centimeter: return O3DS::DistanceUnit::Centimeter;
	case O3DS::Data::DistanceUnit_Meter: return O3DS::DistanceUnit::Meter;
	case O3DS::Data::DistanceUnit_Kilometer: return O3DS::DistanceUnit::Kilometer;
	case O3DS::Data::DistanceUnit_Inch: return O3DS::DistanceUnit::Inch;
	case O3DS::Data::DistanceUnit_Foot: return O3DS::DistanceUnit::Foot;
	case O3DS::Data::DistanceUnit_Mile: return O3DS::DistanceUnit::Mile;
	case O3DS::Data::DistanceUnit_LightYear: return O3DS::DistanceUnit::LightYear;
	case O3DS::Data::DistanceUnit_Parsec: return O3DS::DistanceUnit::Parsec;
	}
	return O3DS::DistanceUnit::Unknown;
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

		for(auto &m : other.mMatrices) {
			mMatrices.push_back(TransformMatrix(m.value.eval()));
		}

		for(auto &op : other.mTransformOrder) {
			mTransformOrder.push_back(op);
		}

		mName = other.mName;
		mParentId = other.mParentId;
		mReference = nullptr; // probably not a good idea to copy this.
	}

	void Transform::update()
	{
		// No implementation here

		bWorldMatrix = false;

		mMatrix = Matrix::Identity();

		int matrixId = 0;

		for (auto op : mTransformOrder)
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
				mMatrix = mMatrix * mMatrices[matrixId++].value;
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

		for (const auto& i : mMatrices)
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

		if (this->mTransformOrder != other.mTransformOrder) return false;

		if (this->mMatrices.size() != other.mMatrices.size()) return false;
		for (size_t i = 0; i < this->mMatrices.size(); i++)
		{
			if (this->mMatrices[i].value != other.mMatrices[i].value)
				return false;
		}

		return true;
	}

	flatbuffers::Offset<O3DS::Data::Transform> Transform::serialize(flatbuffers::FlatBufferBuilder& builder)
	{
		int matrixId = 0;

		std::vector<O3DS::Data::Matrix> matrices;
		std::vector<int8_t> components;

		O3DS::Data::Translation oTranslation;
		O3DS::Data::Rotation oRotation;
		O3DS::Data::Scale oScale;

		this->translation >> oTranslation;
		this->rotation >> oRotation;
		this->scale >> oScale;

		this->translation.sent();
		this->rotation.sent();

		for (const auto component : this->mTransformOrder) {
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

		for (int i = 0; i < this->mMatrices.size(); i++) {
			// Copy all matrices.  This allows embedding other data (offsets)
			O3DS::Data::Matrix matrix;
			this->mMatrices[matrixId++] >> matrix;
			matrices.push_back(matrix);
		}

		auto oTransformName = builder.CreateString(this->mName);

		auto ovMatrices = builder.CreateVectorOfStructs(matrices);

		auto ovComponents = builder.CreateVector(components);

		return CreateTransform(builder, this->mParentId, oTransformName,
			&oTranslation, &oRotation, &oScale,
			ovMatrices, ovComponents);
	}

	void Transform::parse(const O3DS::Data::Transform* inTransform)
	{
		this->mName = inTransform->name()->str();
		this->mParentId = inTransform->parent();

		auto inTranslation = inTransform->translation();
		auto inRotation    = inTransform->rotation();
		auto inScale       = inTransform->scale();

		if (inTranslation) { *inTranslation >> this->translation; }
		if (inRotation)    { *inRotation >> this->rotation; }
		if (inScale)       { *inScale >> this->scale; }

		// Add the components to the transform stack in the order they are defined.
		auto inComponents = inTransform->components();

		for (int8_t componentId : *inComponents)
		{
			if (componentId == O3DS::Data::Component::Component_Translation)
			{
				this->mTransformOrder.push_back(O3DS::TTranslation);
			}
			if (componentId == O3DS::Data::Component::Component_Rotation)
			{
				this->mTransformOrder.push_back(O3DS::TRotation);
			}
			if (componentId == O3DS::Data::Component::Component_Scale)
			{
				this->mTransformOrder.push_back(O3DS::TScale);
			}
			if (componentId == O3DS::Data::Component::Component_Matrix)
			{
				this->mTransformOrder.push_back(O3DS::TMatrix);
			}
		}

		auto inMatrix = inTransform->matrix();

		auto ovMatrices = inTransform->matrix();
		for (auto eachMatrix : *inMatrix) {
			O3DS::TransformMatrix matrix;
			*eachMatrix >> matrix;
			this->mMatrices.push_back(matrix);
		}
	}

	void Transform::setOrderTRS()
	{
		mTransformOrder.clear();
		mTransformOrder.push_back(O3DS::TTranslation);
		mTransformOrder.push_back(O3DS::TRotation);
		mTransformOrder.push_back(O3DS::TScale);
	
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
	
	//////////////////////////////
	// Subject

	Subject::Subject(void* ref)
		: mReference(ref)
		, mEnabled(true)
	{}

	Subject::Subject(const std::string& name, const std::string& uuid, void* ref)
		: mName(name)
		, mReference(ref)
		, mUuid(uuid)
		, mEnabled(true)
	{}


	void Subject::parseUpdate(const O3DS::Data::SubjectUpdate* inUpdate)
	{
		std::string uuid = inUpdate->uuid()->str();
		int id;

		// Update TRS

		for (const O3DS::Data::TranslationUpdate* inTranslation : *inUpdate->translation())
		{
			id = inTranslation->i();
			if (id < this->mTransforms.size()) {
				*inTranslation >> this->mTransforms[id]->translation;
			}
		}

		for (auto inRotation : *inUpdate->rotation())
		{
			id = inRotation->i();
			if (id < this->mTransforms.size()) {
				*inRotation >> this->mTransforms[id]->rotation;
			}
		}

		for (const auto& inScale : *inUpdate->scale())
		{
			id = inScale->i();
			if (id < this->mTransforms.size()) {
				*inScale >> this->mTransforms[id]->scale;
			}
		}
	}

	void Subject::parse(const O3DS::Data::SubjectData* inSubject, TransformBuilder* builder)
	{
		std::string subjectName = inSubject->name()->str();
		std::string subjectUuid = inSubject->uuid()->str();

		this->mName = subjectName;
		this->mUuid = subjectUuid;

		// Get the nodes (transforms) for this subject
		auto ovNodes = inSubject->nodes();

		// Clear the subject and add the transforms
		for (int n = 0; n < (int)ovNodes->size(); n++)
		{
			Transform* transform = this->addTransform(builder);
			transform->parse(ovNodes->Get(n));
		}
	}

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

	Transform* Subject::addTransform(TransformBuilder* builder)
	{
		std::unique_ptr<Transform> transform;
		Transform* ret = nullptr;
		if (builder) transform = builder->build();
		else         transform = std::make_unique<Transform>();
		ret = transform.get();
		mTransforms.mItems.push_back(std::move(transform));
		return ret;
	}

	Transform* Subject::addTransform(const std::string& name, int parentId, TransformBuilder* builder)
	{
		auto ret = addTransform(builder);
		ret->mName = name;
		ret->mParentId = parentId;
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

	bool Subject::calcMatrices(O3DS::ConversionContext* conv)
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
		for (Transform* transform : this->mTransforms) {
			if (transform->mParentId == -1)
			{
				// No Parent - matrix is world matrix
				transform->mWorldMatrix = transform->mMatrix;
				transform->bWorldMatrix = true;
				rootCount++;
			}
		}

		if (rootCount == 0)
		{
			mError = "Could not find a root";
			return false;
		}
		if (rootCount > 1)
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


		if (conv && !conv->isIdentity())
		{
			for (int transformId = 0; transformId < this->mTransforms.size(); transformId++) {
				auto transform = this->mTransforms[transformId];
				conv->convertMatrix(transform->mWorldMatrix);
			}
		}		

		return true;
	}

	flatbuffers::Offset<O3DS::Data::SubjectData> Subject::serialize(flatbuffers::FlatBufferBuilder& builder)
	{
		auto oSubjectName = builder.CreateString(this->mName);
		auto oSubjectUuid = builder.CreateString(this->mUuid);
		std::vector<flatbuffers::Offset<O3DS::Data::Transform>> ovSkeleton;

		for (Transform* t : this->mTransforms) {
						
			ovSkeleton.push_back(t->serialize(builder));
		}

		auto transforms = builder.CreateVector(ovSkeleton);
		return CreateSubjectData(builder, transforms, oSubjectName, oSubjectUuid);
	}

	flatbuffers::Offset<O3DS::Data::SubjectUpdate> Subject::serializeUpdate(flatbuffers::FlatBufferBuilder& builder, size_t &count, double deltaThreshold)
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
		return CreateSubjectUpdate(builder, tr, ro, sc, oSubjectUuid);
	}



	//////////////////////////////
	// RigidbodySubject

	RigidbodySubject::RigidbodySubject(void* ref)
		: Subject(ref)
	{}

	RigidbodySubject::RigidbodySubject(const std::string& name, const std::string& uuid, void* ref)
		: Subject(name, uuid, ref)
	{}

	void RigidbodySubject::parse(const O3DS::Data::Rigidbody* data, RigidbodyBuilder* builder)
	{
		Subject::parse(data->data(), builder);
	}

	void RigidbodySubject::parseUpdate(const O3DS::Data::RigidbodyUpdate* inUpdate)
	{
		Subject::parseUpdate(inUpdate->data());
	}

	//! Flatbuffer serialization
	flatbuffers::Offset<O3DS::Data::Rigidbody> RigidbodySubject::serialize(flatbuffers::FlatBufferBuilder& builder)
	{
		auto oSubjectData = Subject::serialize(builder);
		return CreateRigidbody(builder, oSubjectData);

	}

	//!	Flatbuffers serialization of updates only
	flatbuffers::Offset<O3DS::Data::RigidbodyUpdate> RigidbodySubject::serializeUpdate(flatbuffers::FlatBufferBuilder& builder, size_t& count, double deltaThreshold)
	{
		auto oSubjectUpdate = Subject::serializeUpdate(builder, count, deltaThreshold);
		return CreateRigidbodyUpdate(builder, oSubjectUpdate);
	}



	//////////////////////////////
	// PerformerSubject

	PerformerSubject::PerformerSubject(void* ref)
		: Subject(ref)
	{}

	PerformerSubject::PerformerSubject(const std::string& name, const std::string& uuid, void* ref)
		: Subject(name, uuid, ref)
	{}

	void PerformerSubject::parse(const O3DS::Data::Performer* data, JointBuilder* builder)
	{
		Subject::parse(data->data(), builder);
	}

	void PerformerSubject::parseUpdate(const O3DS::Data::PerformerUpdate* inUpdate)
	{
		Subject::parseUpdate(inUpdate->data());
	}

	//! Flatbuffer serialization
	flatbuffers::Offset<O3DS::Data::Performer> PerformerSubject::serialize(flatbuffers::FlatBufferBuilder& builder)
	{
		auto oSubjectData = Subject::serialize(builder);
		return CreatePerformer(builder, oSubjectData);	
	}

	//!	Flatbuffers serialization of updates only
	flatbuffers::Offset<O3DS::Data::PerformerUpdate> PerformerSubject::serializeUpdate(flatbuffers::FlatBufferBuilder& builder, size_t& count, double deltaThreshold)
	{
		auto oSubjectUpdate = Subject::serializeUpdate(builder, count, deltaThreshold);
		return CreatePerformerUpdate(builder, oSubjectUpdate);	
	}


	//////////////////////////////////
	// Camera Subject

	CameraSubject::CameraSubject(void* ref)
		: Subject(ref)
		, filmBackWidth(0.f)
		, filmBackHeight(0.f)
		, focalLength(0.f)
		, aspect(0.f)
		, focusDistance(0.f)
		, aperture(0.f)
	{}

	CameraSubject::CameraSubject(const std::string& name, const std::string& uuid, void* ref)
		: Subject(name, uuid, ref)
		, filmBackWidth(0.f)
		, filmBackHeight(0.f)
		, focalLength(0.f)
		, aspect(0.f)
		, focusDistance(0.f)
		, aperture(0.f)
	{}

	void CameraSubject::parse(const O3DS::Data::Camera* inCamera, CameraBuilder* builder)
	{
		Subject::parse(inCamera->data(), builder);

		this->filmBackWidth = inCamera->filmback_width();
		this->filmBackHeight = inCamera->filmback_height();
	}

	void CameraSubject::parseUpdate(const O3DS::Data::CameraUpdate* inCameraUpdate)
	{
		Subject::parseUpdate(inCameraUpdate->data());
		this->focalLength = inCameraUpdate->focal_length();
		this->focusDistance = inCameraUpdate->focus_distance();
		this->aperture = inCameraUpdate->aperture();
	}

	flatbuffers::Offset<O3DS::Data::Camera> CameraSubject::serialize(flatbuffers::FlatBufferBuilder& builder)
	{
		auto oSubject = Subject::serialize(builder);
		return CreateCamera(builder, oSubject, this->filmBackWidth, this->filmBackHeight);
	}

	flatbuffers::Offset<O3DS::Data::CameraUpdate> CameraSubject::serializeUpdate(
		flatbuffers::FlatBufferBuilder& builder, 
		size_t& count, 
		double deltaThreshold)
	{
		auto updateData = Subject::serializeUpdate(builder, count, deltaThreshold);
		return CreateCameraUpdate(builder, updateData, focalLength, focusDistance, aperture);
	}



	//////////////////////////////
	// Subject List

	SubjectList::SubjectList()
		: mTime(0.0)
		, mDeltaThreshold(1e-6 /*std::numeric_limits<double>::min()*/)
	{}

	SubjectList::~SubjectList()
	{}

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

	void SubjectList::update()
	{
		for (auto& i : mItems)
		{
			i->update();
		}
	}

	size_t SubjectList::activeCount() const
	{
		size_t ret = 0;
		for (const auto& subject : mItems)
		{
			if (subject->mEnabled) {
				ret++;
			}
		}

		return ret;
	}

	bool SubjectList::serialize(std::vector<char> &outbuf, double timestamp, const std::string &timecode)
	{	
		if (!allFinite()) {
			// All finite should set the error with the invalid joint name
			return false;
		}

		if(timestamp == 0.0) timestamp = GetTime();

		flatbuffers::FlatBufferBuilder builder;

		// Subjects

		std::vector<flatbuffers::Offset<O3DS::Data::Performer> > outPerformers;
		std::vector<flatbuffers::Offset<O3DS::Data::Rigidbody> > outRigidbodies;
		std::vector<flatbuffers::Offset<O3DS::Data::Camera> >    outCameras;

		std::vector<flatbuffers::Offset<O3DS::Data::PerformerUpdate>> outPerformerUpdates;
		std::vector<flatbuffers::Offset<O3DS::Data::RigidbodyUpdate>> outRigidbodyUpdates;
		std::vector<flatbuffers::Offset<O3DS::Data::CameraUpdate>>    outCameraUpdates;

		size_t count = 0;

		for (const auto& subject : mItems)
		{
			if (subject->mEnabled) {
				CameraSubject* cameraSubject = dynamic_cast<CameraSubject*>(subject.get());
				if (cameraSubject) {
					outCameras.push_back(cameraSubject->serialize(builder));
					outCameraUpdates.push_back(cameraSubject->serializeUpdate(builder, count, mDeltaThreshold));
				}

				PerformerSubject* performerSubject = dynamic_cast<PerformerSubject*>(subject.get());
				if (performerSubject) {
					outPerformers.push_back(performerSubject->serialize(builder));
					outPerformerUpdates.push_back(performerSubject->serializeUpdate(builder, count, mDeltaThreshold));

				}

				RigidbodySubject* rigidbodySubject = dynamic_cast<RigidbodySubject*>(subject.get());
				if (rigidbodySubject) {
					outRigidbodies.push_back(rigidbodySubject->serialize(builder));
					outRigidbodyUpdates.push_back(rigidbodySubject->serializeUpdate(builder, count, mDeltaThreshold));

				}				
			}
		}


		// Finish

		auto oTimecode = builder.CreateString(timecode);
		auto oPerformers = builder.CreateVector(outPerformers);
		auto oRigidbodies = builder.CreateVector(outRigidbodies);
		auto oCameras = builder.CreateVector(outCameras);
		auto ovPerformerUpdates = builder.CreateVector(outPerformerUpdates);
		auto ovRigidbodyUpdates = builder.CreateVector(outRigidbodyUpdates);
		auto ovCameraUpdates = builder.CreateVector(outCameraUpdates);

		auto root = CreateSubjectList(builder, 
			oPerformers, ovPerformerUpdates, 
			oRigidbodies, ovRigidbodyUpdates, 
			oCameras, ovCameraUpdates, 
			timestamp, oTimecode,
			dir(this->mContext.mX), dir(this->mContext.mY), dir(this->mContext.mZ), distUnit(this->mContext.mDistance));

		builder.Finish(root);

		finalize(builder, outbuf, 1);

		return true;
	}

	bool SubjectList::serializeUpdate(std::vector<char>& outbuf, size_t& count, double timestamp, const std::string& timecode)
	{		
		count = 0;

		if (!this->allFinite()) {
			// All finite should set the error with the invalid joint name
			return false;
		}

		if (timestamp == 0.0) { timestamp = GetTime(); }

		flatbuffers::FlatBufferBuilder builder;

		// Subjects

		std::vector<flatbuffers::Offset<O3DS::Data::PerformerUpdate>> outPerformerUpdates;
		std::vector<flatbuffers::Offset<O3DS::Data::RigidbodyUpdate>> outRigidbodyUpdates;
		std::vector<flatbuffers::Offset<O3DS::Data::CameraUpdate>>    outCameraUpdates;

		for (auto& subject : this->mItems)
		{
			if (subject->mEnabled) {
				auto cameraSubject = dynamic_cast<CameraSubject*>(subject.get());
				if (cameraSubject) {
					outCameraUpdates.push_back(cameraSubject->serializeUpdate(builder, count, mDeltaThreshold));
					continue;
				}
				auto performerSubject = dynamic_cast<PerformerSubject*>(subject.get());
				if (performerSubject) {
					outPerformerUpdates.push_back(performerSubject->serializeUpdate(builder, count, mDeltaThreshold));
					continue;
				}
				auto rigidbodySubject = dynamic_cast<RigidbodySubject*>(subject.get());
				if (rigidbodySubject) {
					outRigidbodyUpdates.push_back(rigidbodySubject->serializeUpdate(builder, count, mDeltaThreshold));
					continue;
				}
			}
		}

		auto ovPerformerUpdates = builder.CreateVector(outPerformerUpdates);
		auto ovRigidbodyUpdates = builder.CreateVector(outRigidbodyUpdates);
		auto ovCameraUpdates = builder.CreateVector(outCameraUpdates);

		auto oTimecode = builder.CreateString(timecode);

		O3DS::Data::Direction x = dir(this->mContext.mX);
		O3DS::Data::Direction y = dir(this->mContext.mY);
		O3DS::Data::Direction z = dir(this->mContext.mZ);
		O3DS::Data::DistanceUnit d = distUnit(this->mContext.mDistance);

		auto root = CreateSubjectList(builder, 0, ovPerformerUpdates, 0, ovRigidbodyUpdates, 0, ovCameraUpdates,  
			timestamp, oTimecode,
			x, y, z, d);

		builder.Finish(root);

		finalize(builder, outbuf, 2);

		return true;
	}


	bool SubjectList::parse(
		const char* data,
		size_t len,
		BuilderSet* builders,
		bool clearInactive)
	{
		if (data == nullptr) {
			mError = "Nullptr passed as data to parse";
			return false;
		}

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

		O3DS::Context senderContext(
			dir(root->x_axis()),
			dir(root->y_axis()),
			dir(root->z_axis()),
			static_cast<O3DS::DistanceUnit>(root->distance_unit())
		);

		this->mTime = root->time();
		this->mTimecode = root->timecode()->str();

		auto performers_data = root->performers();
		auto performer_updates = root->performer_updates();
		auto cameras_data = root->cameras();
		auto cameras_updates = root->camera_updates();
		auto rigidbody_data = root->rigidbodies();
		auto rigidbody_updates = root->rigidbody_updates();

		if (!this->mContext.valid())
		{
			this->mContext = senderContext;
			conversion.reset();
		}
		else
		{
			conversion = std::make_unique<ConversionContext>(senderContext, this->mContext);
			if (!conversion->IsValid()) {
				mError = "Invalid context conversion";
				return false;
			}
			conversion->Initialize();
		}

		if (clearInactive) {
			// Clear the list before populating
			this->mItems.clear();
		}


		if (performers_data)
		{
			for (uint32_t i = 0; i < performers_data->size(); i++)
			{
				auto performer = performers_data->Get(i);
				std::string subjectUuid = performer->data()->uuid()->str();

				// Check to see if this subject already exists
				PerformerSubject* outPerformer = this->findOrAddSubject<PerformerSubject>(subjectUuid);
				outPerformer->clearTransforms();
				JointBuilder* builder = nullptr;
				if (builders) { builder = builders->joint; }
				outPerformer->parse(performer, builder);
			}
		}

		if (performer_updates)
		{
			for (uint32_t i = 0; i < performer_updates->size(); i++)
			{
				auto inUpdate = performer_updates->Get(i);
				std::string uuid = inUpdate->data()->uuid()->str();

				// Find the subject to update, by uuid
				PerformerSubject* outSubject = this->findSubjectByUuid<PerformerSubject>(uuid);
				if (outSubject) {
					outSubject->parseUpdate(inUpdate);
				}
			}
		}

		if (rigidbody_data)
		{
			for (uint32_t i = 0; i < rigidbody_data->size(); i++)
			{
				auto rigidbody = rigidbody_data->Get(i);
				std::string subjectUuid = rigidbody->data()->uuid()->str();

				// Check to see if this subject already exists
				RigidbodySubject* outRigidbody = this->findOrAddSubject<RigidbodySubject>(subjectUuid);
				outRigidbody->clearTransforms();
				RigidbodyBuilder* builder = nullptr;
				if (builders) { builder = builders->rigidbody; }
				outRigidbody->parse(rigidbody, builder);
			}
		}

		if (rigidbody_updates)
		{
			for (uint32_t i = 0; i < rigidbody_updates->size(); i++)
			{
				auto inUpdate = rigidbody_updates->Get(i);
				std::string uuid = inUpdate->data()->uuid()->str();

				// Find the subject to update, by uuid
				RigidbodySubject* outSubject = this->findSubjectByUuid<RigidbodySubject>(uuid);
				if (outSubject) {
					outSubject->parseUpdate(inUpdate);
				}
			}
		}

		if (cameras_data)
		{
			for (uint32_t i = 0; i < cameras_data->size(); i++)
			{
				auto oCamera = cameras_data->Get(i);
				std::string uuid = oCamera->data()->uuid()->str();
				CameraSubject *camera = this->findOrAddSubject<CameraSubject>(uuid);
				CameraBuilder* builder = nullptr;
				if (builders) { builder = builders->camera; }
				camera->parse(oCamera, builder);
			}
		}

		if(cameras_updates)
		{
			for (uint32_t i = 0; i < cameras_updates->size(); i++)
			{
				auto inCam = cameras_updates->Get(i);
				std::string uuid = inCam->data()->uuid()->str();

				auto camera = this->findSubjectByUuid<CameraSubject>(uuid);
				if (camera) {
					camera->parseUpdate(inCam);
				}
			}
		}

		if (!allFinite()) {
			// All finite should set the error with the invalid joint name
			return false;
		}

		if (!calcMatrices()) {
			return false;
		}
		
		return true;
	}

	bool SubjectList::calcMatrices()
	{
		for (const auto& subject : mItems) {
			if (!subject->calcMatrices(conversion.get())) {
				mError = subject->mError;
				return false;
			}
		}
		return true;
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


