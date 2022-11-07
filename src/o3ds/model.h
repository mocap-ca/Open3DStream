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

#include "math.h"
#include "context.h"
#include "transform_component.h"
#include "schema_generated.h"


namespace O3DS
{


	/*! \class Transform model.h o3ds/model.h */
	//! Defines a single transform with name and parent id reference
	class Transform
	{
	public:
		Transform(std::string& name, int parentId, void *ref = nullptr);

		Transform(int parentId);

		Transform();

		virtual ~Transform();

		virtual void update() {}
		virtual std::string info() { return std::string(); }

		TransformTranslation translation;
		TransformRotation    rotation;
		TransformScale       scale;

		Matrixd       mMatrix;
		Matrixd       mWorldMatrix;
		bool          bWorldMatrix;

		std::vector<TransformMatrix> matrices;
		std::vector<enum ComponentType> transformOrder;

		std::string mName;
		int mParentId;

		void *mReference;

	};

	class TransformBuilder
	{
	public:
		virtual Transform* build(std::string name, int parentId) = 0;
	};


	/*! \class TransformList model.h o3ds/model.h */
	//! A list (std::vector) of Transform objects
	class TransformList
	{
	private:
		TransformList(const TransformList &other)
		{}

	public:
		TransformList() {}

		//! deletes the transform objects in the list
		virtual ~TransformList()
		{
			for (auto i : mItems)
				delete i;
		}

		//! Returns the number of transforms
		size_t size() { return mItems.size(); }

		//! Delete the transform objects and clear the least.
		void clear()
		{
			for (auto i : mItems)
				delete i;
			mItems.clear();
		}

		void update()
		{
			for (auto i : mItems)
				i->update();
		}

		std::vector <Transform*>::iterator begin() { return mItems.begin(); }
		std::vector <Transform*>::iterator end()   { return mItems.end(); }
		Transform *operator[](size_t id)           { return mItems[id]; }

		std::vector<Transform*> mItems;

		Transform* find(const std::string &name)
		{
			for (auto i = 0; i < mItems.size(); i++)
			{
				if (mItems[i]->mName == name)
					return mItems[i];
			}
			return nullptr;
		}
	};


	/*! \class Subject model.h o3ds/model.h
	 *  The subject can also have a SubjectInfo reference for implementation specific data */
	//! A collection of transforms, with a name
	class Subject
	{
	public:
		Subject(void *info = nullptr) : mReference(info) {}

		Subject(std::string name, void *info = nullptr)
			: mName(name)
			, mReference(info) 
		{}

		std::string   mName;
		std::vector<std::string> mJoints;

		TransformList mTransforms;
		void*         mReference;
		Context       mContext;

		Transform* addTransform(std::string &name, int parentId, TransformBuilder *builder = nullptr)
		{
			Transform *ret;
			if (builder) ret = builder->build(name, parentId);
			else         ret = new Transform(name, parentId);
			mTransforms.mItems.push_back(ret);
			return ret;
		}

		void addTransform(Transform* item)
		{
			mTransforms.mItems.push_back(item);
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

		void CalcMatrices();

	};

	/*! \class SubjectList model.h o3ds/model.h */
	//!  A collection of subjects.
	class SubjectList
	{
	public:

		SubjectList()
			: mTime(0.0)
			, mDeltaThreshold(0.0001)
		{}

		SubjectList(const SubjectList &other)
			: mTime(0.0)
			, mDeltaThreshold(0.0001)
		{}

		virtual ~SubjectList()
		{
			for (auto i : mItems)
			{
				delete i;
			}
		}

		Subject* addSubject(std::string name, void* ref=nullptr)
		{
			auto s = new Subject(name, ref);
			mItems.push_back(s);
			return s;
		}

		Subject* findSubject(const std::string &name)
		{
			for (auto i : mItems)
			{
				if (i->mName == name)
					return i;
			}
			return nullptr;
		}

		void update()
		{
			for (auto i : mItems)
			{
				i->update();
			}
		}

		std::vector<Subject*> mItems;

		size_t size() { return mItems.size(); }
		std::vector <Subject*>::iterator begin() { return mItems.begin(); }
		std::vector <Subject*>::iterator end() { return mItems.end(); }
		Subject* operator [] (int ref) { return mItems.operator[](ref); }

		double mTime;
		double mDeltaThreshold;


		//! Encode all of the items in the subject list as binary data
		int Serialize(std::vector<char> &outbuf, double timestamp=0.0);

		int SerializeUpdate(std::vector<char>& outbuf, double timestamp = 0.0);

		//! Populate or update the subject list with the binary data provided (created by Serialize)
		bool Parse(const char *data, size_t len, TransformBuilder* = nullptr);

		void ParseSubject(const O3DS::Data::Subject*, TransformBuilder* = nullptr);

		void ParseUpdate(const O3DS::Data::SubjectUpdate*, TransformBuilder* = nullptr);


	};


} // O3DS


#endif
