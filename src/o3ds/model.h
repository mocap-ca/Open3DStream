/*
Open 3D Stream

Copyright 2026 Alastair Macleod

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
#include <deque>
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

		//! Default constructor
		Transform(const std::string& name, int parentId, void *ref = nullptr);
		
		//! Copy constructor
		Transform(const Transform&);

		//! Parent only constructor
		Transform(int parentId);

		//! Empty constructor
		Transform();

		virtual ~Transform();

		//! No implementation here
		virtual void update();

		//! No implementation here
		virtual std::string info() { return std::string(); }

		//! Verify there are no nan or inf values, sets mError with the name if fails.
		bool allFinite();

		//! Check if values are all ==
		bool operator == (const Transform &other) const;

		//! Translation calue and last sent
		TransformTranslation translation;

		//! Rotation and last sent
		TransformRotation    rotation;

		//! Scale and last sent
		TransformScale       scale;

		//! Local Matrix
		Matrix      mMatrix;

		//! World Matrix
		Matrix      mWorldMatrix;

		//! World Matrix has been populated
		bool        bWorldMatrix;

		//! Any matrices as part of the transform
		std::vector<TransformMatrix> matrices;

		//! Order of calculation, e.g. T R S or T mat R mat S, etc.
		std::vector<enum ComponentType> transformOrder;

		//! Name of this Transform
		std::string mName;

		//! Parent id in subject list
		int mParentId;

		//! User value, used for updating this item
		void *mReference;

	};

	//! Platform specific builder to make a transform object
	class TransformBuilder
	{
	public:
		virtual std::unique_ptr<Transform> build(std::string name, int parentId) = 0;
	};


	/*! \class TransformList model.h o3ds/model.h */
	//! A list (std::deque) of Transform objects
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
		~TransformList();

		//! Returns the number of transforms
		size_t size() const;

		//! Delete the transform objects and clear the least.
		void clear();

		//! Calls all tranform->update()
		void update();

		//! Raw pointer interator 
		struct iterator {
			using inner_it = std::deque<std::unique_ptr<Transform>>::iterator;
			inner_it it;

			Transform* operator*() const { return it->get(); }
			iterator& operator++() { ++it; return *this; }
			bool operator!=(const iterator& other) const { return it != other.it; }
		};

		//! Raw pointer iterator begin
		iterator begin();

		//! Raw pointer iterator end
		iterator end();

		//! Raw pointer getter
		Transform* at(size_t id);

		//! Raw pointer getter
		Transform* operator[](size_t id);

		//! Find a transform by name
		Transform* find(const std::string& name);

		//! Tests equality of all transforms in the list.  Must have same order.
		bool operator ==(const TransformList& other) const;

		//! The transform items (owned)
		std::deque<std::unique_ptr<Transform>> mItems;
	};


	/*! \class Subject model.h o3ds/model.h
	 *  The subject can also have a SubjectInfo reference for implementation specific data */
	//! A collection of transforms, with a name
	class Subject
	{
	public:
		Subject(void* info = nullptr);

		Subject(std::string name, std::string uuid, void* info = nullptr);

		//! The name of the subject
		std::string   mName;

		//! Unique idenfitier for this subject
		std::string   mUuid;

		//! Optional list of joints to send.  Used to limit the sent joints from being the 
		std::vector<std::string> mJoints;

		//! Subject transforms
		TransformList mTransforms;

		//! User reference pointer
		void*         mReference;

		//! The context for this subject ( yup / zup etc)
		Context       mContext;

		//! Error string from last operation, e.g. allFinite or parsing error.
		std::string   mError;

		//! Verify all transforms are finite, sets mError with the name of the first invalid transform
		bool allFinite();

		//! Create a new (owned) transform object and return a reference
		Transform* addTransform(const std::string& name, int parentId, TransformBuilder* builder = nullptr);

		//! Add a new transform object (owned)
		void addTransform(std::unique_ptr<Transform> item);

		//! Clear all the data
		void clearAll();

		//! Clear just the transform data keep the name, id, ref, etc
		void clearTransforms();

		//! virtual update call
		void update();
		
		//! number of transforms
		size_t size();

		//! Calculate the world matrices, check mError if this fails
		bool CalcMatrices();

		//! Flatbuffer serialization
		flatbuffers::Offset<O3DS::Data::Subject> Serialize(flatbuffers::FlatBufferBuilder& builder);

		//!	Flatbuffers serialization of updates only
		flatbuffers::Offset<O3DS::Data::SubjectUpdate> SerializeUpdate(flatbuffers::FlatBufferBuilder& builder, size_t& count, double deltaThreshold);

		//! Encode the subject as binary data
		int Serialize(std::vector<char>& outbuf, double timestamp);	

		//! Encode only the changed transforms as binary data
		int SerializeUpdate(std::vector<char>& outbuf, size_t& count, double deltaThreshold, double timestamp);

		//! Set to true to tell the parse to skip this subject while encoding.
		bool mEnabled;
	};


	/*! \class SubjectList model.h o3ds/model.h */
	//!  A collection of subjects.
	class SubjectList
	{
	public:

		SubjectList();

		SubjectList(const SubjectList& other) = delete;

		virtual ~SubjectList();

		//! Check for nan or inf
		bool allFinite();

		//! Add a new subject to the list
		Subject* addSubject(std::string name, std::string uuid, void* ref = nullptr);

		//! Find a subject by name.  nullptr if not found
		Subject* findSubjectByName(const std::string& name);

		//! Find a subject by unique id.  nullptr if not found
		Subject* findSubjectByUuid(const std::string& uuid);

		//! update transforms (virtual)
		void update();

		std::deque<std::unique_ptr<Subject>> mItems;

		//! Raw pointer iterator
		struct iterator {
			using inner_it = std::deque<std::unique_ptr<Subject>>::iterator;
			inner_it it;

			Subject* operator*() const { return it->get(); }
			iterator& operator++() { ++it; return *this; }
			bool operator!=(const iterator& other) const { return it != other.it; }
		};

		//! Raw pointer begin
		iterator begin() { return { mItems.begin() }; }
		
		//! Raw pointer end
		iterator end() { return { mItems.end() }; }

		//! Number of subjecsts
		size_t size() { return mItems.size(); }

		//! Number of active Subjects
		size_t activeCount() const;

		//! Raw pointer getter
		Subject* operator [] (int i) { return mItems[i].get(); }

		//! Time stamp of last serialization
		double mTime;

		//! Threshold for delta updates
		double mDeltaThreshold;

		//! Error string from last parse
		std::string mError;

		//! Encode all of the items in the subject list as binary data
		bool Serialize(std::vector<char> &outbuf, double timestamp=0.0);

		//! Serialize changes to translation and rotation since last send
		bool SerializeUpdate(std::vector<char>& outbuf, size_t& count, double timestamp=0.0);

		//! Populate or update the subject list with the binary data provided (created by Serialize)
		bool Parse(const char *data, size_t len, TransformBuilder* = nullptr, bool clearInactive = true);

		//! Parse a subject buffer (complete hierarchy)
		void ParseSubject(const O3DS::Data::Subject*, TransformBuilder* = nullptr);
		
		//! Parse an update buffer (spare translation and rotations)
		void ParseUpdate(const O3DS::Data::SubjectUpdate*, TransformBuilder* = nullptr);

		//! Change distance threshold below which O3DS skips transmitting a transform update.
		void SetDeltaThreshold(double newThreshold) { mDeltaThreshold = newThreshold; }

	};

	// Write the header
	void finalize(flatbuffers::FlatBufferBuilder& builder, std::vector<char>& outbuf, std::uint32_t flags);


} // O3DS


#endif
