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

namespace O3DS
{

	class Transform;

	/*! \class Updater model.h o3ds/model.h */
	//! Abstract visitor class to update a transform.
	class Updater
	{
	public:
		virtual void update(Transform*) = 0;
		virtual std::string info() = 0;
	};

	/*! \class Transform model.h o3ds/model.h */
	//! Defines a single transform with name and parent id reference
	class Transform
	{
	public:
		Transform(std::string& name, int parentId, Updater *updater = nullptr)
			: mName(name)
			, mParentId(parentId)
			, mVisitor(updater)
		{}

		Transform(int parentId)
			: mName()
			, mParentId(parentId)
			, mVisitor(nullptr)
		{}

		Transform()
			: mName()
			, mParentId(-1)
			, mVisitor(nullptr)
		{}

		virtual ~Transform()
		{
			if (mVisitor) delete mVisitor;
		};

		void update() { if(mVisitor) mVisitor->update(this); }

		O3DS::Matrix<double> mMatrix;
		O3DS::Matrix<double> mParentInverseMatrix;

		O3DS::Vector<double>  mTranslation;
		O3DS::Vector<double>  mOrientation;

		std::string mName;
		int mParentId;

		Updater *mVisitor;

	};

	/*! \class TransformList model.h o3ds/model.h */
	//! A list (std::vector) of Transform objects
	class TransformList
	{
	public:

		virtual ~TransformList()
		{
			for (auto i : mItems)
				delete i;
		}

		size_t size() { return mItems.size(); }
		void clear() { mItems.clear();  }
		std::vector <Transform*>::iterator begin() { return mItems.begin(); }
		std::vector <Transform*>::iterator end() { return mItems.end(); }

		std::vector<Transform*> mItems;

		Transform* find(const std::string &name)
		{
			for (auto i = 0; i < mItems.size(); i++)
			{
				if (mItems[i]->mName == name)
					return mItems[i];
				return nullptr;
			}

		}
	};

	/*! \class SubjectInfo model.h o3ds/model.h
	 *  This data may be used by the update visitor and is optional. */
	//! Wrapper to hold implementation specific data for a subject. 
	class SubjectInfo
	{
	public:
		virtual ~SubjectInfo() {};
	};

	/*! \class Subject model.h o3ds/model.h
	 *  The subject can also have a SubjectInfo reference for implementation specific data */
	//! A collection of transforms, with a name
	class Subject
	{
	public:
		Subject(SubjectInfo *info = nullptr) : mInfo(info) {}

		Subject(std::string name, SubjectInfo *info = nullptr)
			: mName(name)
			, mInfo(info) 
		{}

		virtual ~Subject() { if (mInfo) delete mInfo; }

		std::string mName;
		TransformList mTransforms;
		SubjectInfo *mInfo;

		Transform* addTransform(std::string &name, int parentId, Updater *updater = nullptr)
		{
			auto ret = new Transform(name, parentId, updater);
			mTransforms.mItems.push_back(ret);
			return ret;
		}

		void addTransform(Transform* item)
		{
			mTransforms.mItems.push_back(item);
		}

		void update(bool useWorldMatrix);

		void clear()
		{
			mTransforms.clear();
		}
		
		size_t size()
		{
			return mTransforms.mItems.size();
		}

	};

	/*! \class SubjectList model.h o3ds/model.h */
	//!  A collection of subjects.
	class SubjectList
	{
	public:

		SubjectList()
		{}

		SubjectList(const SubjectList &other)
		{}

		virtual ~SubjectList()
		{
			for (auto i : mItems)
			{
				delete i;
			}
		}

		Subject* addSubject(std::string name, SubjectInfo* info=nullptr)
		{
			auto s = new Subject(name, info);
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

		void update(bool useMatrix)
		{
			for (auto i : mItems)
			{
				i->update(useMatrix);
			}
		}
		std::vector<Subject*> mItems;

		Context mContext;

		size_t size() { return mItems.size(); }
		std::vector <Subject*>::iterator begin() { return mItems.begin(); }
		std::vector <Subject*>::iterator end() { return mItems.end(); }
		Subject* operator [] (int ref) { return mItems.operator[](ref); }
	};

	//! Encode all of the items in the subject list as binary data
	int Serialize(SubjectList &data, uint8_t *outbuf, int buflen, bool names);

	//! Populate the subject list with the binary data provided (created by Serialize)
	void Parse(SubjectList &subjects, const char *data, size_t len);
	


} // FBStream


#endif
