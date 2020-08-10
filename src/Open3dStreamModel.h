#ifndef OPEN3D_STREAM_MODEL_H
#define OPEN3D_STREAM_MODEL_H

#include <vector>

#include "cml/cml.h"

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

		cml::matrix44d mMatrix;
		cml::matrix44d mParentInverseMatrix;

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

	class Subject
	{
	public:
		Subject() {}
		Subject(std::string name) : mName(name) {}
		std::string mName;
		TransformList mTransforms;

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

		void update()
		{
			for (auto i : mTransforms)
			{
				i->update();
			}
		}

		size_t size() { return mTransforms.items.size(); }

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

		Subject* addSubject(std::string name)
		{
			auto s = new Subject(name);
			items.push_back(s);
			return s;
		}

		void update()
		{
			for (auto i : items)
			{
				i->update();
			}
		}
		std::vector<Subject*> items;

		size_t size() { return items.size(); }
		std::vector <Subject*>::iterator begin() { return items.begin(); }
		std::vector <Subject*>::iterator end() { return items.end(); }
	};

	int Serialize(SubjectList &data, uint8_t *outbuf, int buflen, bool names);
	


} // FBStream


#endif
