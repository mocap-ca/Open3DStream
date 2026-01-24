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

#ifndef OPEN3D_STREAM_MODEL_H
#define OPEN3D_STREAM_MODEL_H

#include <vector>
#include <string>

#include "context.h"
#include "transform_component.h"
#include "o3ds_generated.h"
#include "math.h"

namespace O3DS
{

	/*! \class Transform model.h o3ds/model.h */
	//! Defines a single transform with name and parent id reference
	class Transform
	{
	public:

		EIGEN_MAKE_ALIGNED_OPERATOR_NEW

		Transform(const std::string& name, int parentId, void *ref = nullptr);

		Transform(int parentId);

		Transform();

		virtual ~Transform();

		virtual void update() {}
		virtual std::string info() { return std::string(); }

		bool nan() const;

		bool operator == (const Transform &other) const;

		TransformTranslation translation;
		TransformRotation    rotation;
		TransformScale       scale;

		Matrix      mMatrix;
		Matrix      mWorldMatrix;
		bool        bWorldMatrix;

		std::vector<TransformMatrix> matrices;
		std::vector<enum ComponentType> transformOrder;

		std::string mName;
		int mParentId;

		void *mReference;

	};

	//! Platform specific builder to make a transform object
	class TransformBuilder
	{
	public:
		virtual std::unique_ptr<Transform> build(std::string name, int parentId) = 0;
	};


	/*! \class TransformList model.h o3ds/model.h */
	//! A list (std::vector) of Transform objects
	class TransformList
	{
	private:
		TransformList(const TransformList&) = delete;
		TransformList& operator=(const TransformList&) = delete;
		TransformList(TransformList&&) = delete;
		TransformList& operator=(TransformList&&) = delete;

	public:
		TransformList() {}

		//! deletes the transform objects in the list
		~TransformList()
		{
			mItems.clear();
		}

		//! Returns the number of transforms
		size_t size() const { return mItems.size(); }

		//! Delete the transform objects and clear the least.
		void clear()
		{
			mItems.clear();
		}

		void update()
		{
			for (auto& i : mItems)
				i->update();
		}

		struct iterator {
			using inner_it = std::vector<std::unique_ptr<Transform>>::iterator;
			inner_it it;

			Transform* operator*() const { return it->get(); }
			iterator& operator++() { ++it; return *this; }
			bool operator!=(const iterator& other) const { return it != other.it; }
		};

		iterator begin() { return { mItems.begin() }; }
		iterator end() { return { mItems.end() }; }

		Transform* at(size_t id)
		{
			if (id >= mItems.size()) return nullptr;
			return mItems[id].get();
		}

		Transform *operator[](size_t id) { return mItems[id].get(); }

		std::vector<std::unique_ptr<Transform>> mItems;

		Transform* find(const std::string &name)
		{
			for (size_t i = 0; i < mItems.size(); i++)
			{
				if (mItems[i]->mName == name)
					return mItems[i].get();
			}
			return nullptr;
		}

		bool operator ==(const TransformList &other) const
		{
			if (mItems.size() != other.mItems.size())
				return false;

			for (size_t i = 0; i < mItems.size(); i++)
			{
				if (mItems[i]->operator==(*other.mItems[i]) == false) {
					return false;
				}
			}
			return true;
		}
	};


	/*! \class Subject model.h o3ds/model.h
	 *  The subject can also have a SubjectInfo reference for implementation specific data */
	//! A collection of transforms, with a name
	class Subject
	{
	public:
		Subject(void *info = nullptr) 
			: mReference(info)
		{}

		Subject(std::string name, std::string uuid, void *info = nullptr)
			: mName(name)
			, mReference(info) 
			, mUuid(uuid)
		{}

		std::string   mName;
		std::string   mUuid;
		std::vector<std::string> mJoints;

		TransformList mTransforms;
		void*         mReference;
		Context       mContext;
		std::string   mError;

		Transform* addTransform(const std::string& name, int parentId, TransformBuilder *builder = nullptr)
		{
			std::unique_ptr<Transform> transform;
			Transform* ret = nullptr;
			if (builder) transform = builder->build(name, parentId);
			else         transform = std::make_unique<Transform>(name, parentId);
			ret = transform.get();
			mTransforms.mItems.push_back(std::move(transform));
			return ret;
		}

		void addTransform(std::unique_ptr<Transform> item)
		{
			mTransforms.mItems.push_back(std::move(item));
		}

		void clear()
		{
			mTransforms.clear();
		}

		void update()
		{
			mTransforms.update();
		}
		
		size_t size()
		{
			return mTransforms.mItems.size();
		}

		bool CalcMatrices();

		flatbuffers::Offset<O3DS::Data::Subject> Serialize(flatbuffers::FlatBufferBuilder& builder);

		flatbuffers::Offset<O3DS::Data::SubjectUpdate> SerializeUpdate(flatbuffers::FlatBufferBuilder& builder, size_t& count, double deltaThreshold);

		int Serialize(std::vector<char>& outbuf, double timestamp);	

		int SerializeUpdate(std::vector<char>& outbuf, size_t& count, double deltaThreshold, double timestamp);

	};


	/*! \class SubjectList model.h o3ds/model.h */
	//!  A collection of subjects.
	class SubjectList
	{
	public:

		SubjectList()
			: mTime(0.0)
			, mDeltaThreshold(std::numeric_limits<double>::min())
		{}

		SubjectList(const SubjectList& other) = delete;

		virtual ~SubjectList()
		{
			mItems.clear();
		}

		Subject* addSubject(std::string name, std::string uuid, void* ref=nullptr)
		{
			auto s = std::make_unique<Subject>(name, uuid, ref);
			Subject*  sptr = s.get();
			mItems.push_back(std::move(s));
			return sptr;
		}

		Subject* findSubjectByName(const std::string &name)
		{
			for (auto& i : mItems)
			{
				if (i->mName == name)
					return i.get();
			}
			return nullptr;
		}

		Subject* findSubjectByUuid(const std::string& uuid)
		{
			for (auto& i : mItems)
			{
				if (i->mUuid == uuid)
					return i.get();
			}
			return nullptr;
		}

		void update()
		{
			for (auto& i : mItems)
			{
				i->update();
			}
		}

		std::vector<std::unique_ptr<Subject>> mItems;

		struct iterator {
			using inner_it = std::vector<std::unique_ptr<Subject>>::iterator;
			inner_it it;

			Subject* operator*() const { return it->get(); }
			iterator& operator++() { ++it; return *this; }
			bool operator!=(const iterator& other) const { return it != other.it; }
		};

		size_t size() { return mItems.size(); }
		Subject* operator [] (int i) { return mItems[i].get(); }

		iterator begin() { return { mItems.begin() }; }
		iterator end() { return { mItems.end() }; }

		double mTime;
		double mDeltaThreshold;
		std::string mError;

		//! Encode all of the items in the subject list as binary data
		int Serialize(std::vector<char> &outbuf, double timestamp=0.0);

		int SerializeUpdate(std::vector<char>& outbuf, size_t& count, double timestamp=0.0);

		//! Populate or update the subject list with the binary data provided (created by Serialize)
		bool Parse(const char *data, size_t len, TransformBuilder* = nullptr, bool clearInactive = true);

		void ParseSubject(const O3DS::Data::Subject*, TransformBuilder* = nullptr);

		void ParseUpdate(const O3DS::Data::SubjectUpdate*, TransformBuilder* = nullptr);

		//! Change distance threshold below which O3DS skips transmitting a transform update.
		void SetDeltaThreshold(double newThreshold) { mDeltaThreshold = newThreshold; }

	};

	void finalize(flatbuffers::FlatBufferBuilder& builder, std::vector<char>& outbuf, std::uint32_t flags);


} // O3DS


#endif
