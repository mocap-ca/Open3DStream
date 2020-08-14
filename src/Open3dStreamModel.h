#ifndef OPEN3D_STREAM_MODEL_H
#define OPEN3D_STREAM_MODEL_H

#include <vector>

#include "o3ds/math.h"
#include <string>

namespace O3DS
{
	class Transform;

	class Updater
	{
	public:
		virtual void update(Transform*) = 0;
		virtual std::string info() = 0;
	};

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

		O3DS::Vector<double>  mTranslation;
		O3DS::Vector<double>  mOrientation;

		std::string mName;
		int mParentId;

		Updater *mVisitor;

	};

	class TransformList
	{
	public:

		virtual ~TransformList()
		{
			for (auto i : items)
				delete i;
		}

		size_t size() { return items.size(); }
		std::vector <Transform*>::iterator begin() { return items.begin(); }
		std::vector <Transform*>::iterator end() { return items.end(); }

		std::vector<Transform*> items;
	};

	class SubjectInfo
	{
	public:
		virtual ~SubjectInfo() {};
	};

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
			mTransforms.items.push_back(ret);
			return ret;
		}

		void addTransform(Transform* item)
		{
			mTransforms.items.push_back(item);
		}

		void update(bool useWorldMatrix);
		
		size_t size()
		{
			return mTransforms.items.size();
		}

	};

	class SubjectList
	{
	public:

		virtual ~SubjectList()
		{
			for (auto i : items)
			{
				delete i;
			}
		}

		Subject* addSubject(std::string name, SubjectInfo* info=nullptr)
		{
			auto s = new Subject(name, info);
			items.push_back(s);
			return s;
		}

		void update(bool useMatrix)
		{
			for (auto i : items)
			{
				i->update(useMatrix);
			}
		}
		std::vector<Subject*> items;

		size_t size() { return items.size(); }
		std::vector <Subject*>::iterator begin() { return items.begin(); }
		std::vector <Subject*>::iterator end() { return items.end(); }
		Subject* operator [] (int ref) { return items.operator[](ref); }
	};

	int Serialize(SubjectList &data, uint8_t *outbuf, int buflen, bool names);
	


} // FBStream


#endif
