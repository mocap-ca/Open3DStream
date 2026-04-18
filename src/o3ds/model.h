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

		virtual ~Transform() = default;
		
		//! Convert to flatbuffers using the given builder
		flatbuffers::Offset<O3DS::Data::Transform> serialize(flatbuffers::FlatBufferBuilder& builder);

		//! Parse the given flatbuffer data to populate this transform
		void parse(const O3DS::Data::Transform* data);

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
		std::vector<TransformMatrix> mMatrices;

		//! Order of calculation, e.g. T R S or T mat R mat S, etc.
		std::vector<enum ComponentType> mTransformOrder;

		//! Name of this Transform
		std::string mName;

		//! Parent id in subject list
		int mParentId;

		//! User value, used for updating this item
		void *mReference;

	};

	class TransformBuilder
	{
		public:
		virtual std::unique_ptr<Transform> build() = 0;
	};

	//! Platform specific builder to make a rigibody object
	class RigidbodyBuilder : public TransformBuilder
	{
	};

	//! Platform specific builder to make a rigibody object
	class JointBuilder : public TransformBuilder
	{
	};

	//! Platform specific builder to make a camera object (optional)
	class CameraBuilder : public TransformBuilder
	{
	public:
	};

	struct BuilderSet {
		JointBuilder* joint = nullptr;
		RigidbodyBuilder* rigidbody = nullptr;
		CameraBuilder* camera = nullptr;
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

		//! Raw pointer getter, bounds checked
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
		//! Create a subject with an optional reference pointer.
		Subject(void* ref = nullptr);

		//! Create a subject with the name and unique id, and optional user reference pointer
		Subject(const std::string& name, const std::string& uuid, void* ref = nullptr);

		virtual ~Subject() = default;

		//! Parse the incoming flatbuffer data to populate this subject, no null checking.
		void parse(const O3DS::Data::SubjectData* data, TransformBuilder* builder);

		//! Parse the incoming flatbuffer update data to update the transforms in this subject, no null checking.
		void parseUpdate(const O3DS::Data::SubjectUpdate* inUpdate);

		//! Flatbuffer serialization
		flatbuffers::Offset<O3DS::Data::SubjectData> serialize(flatbuffers::FlatBufferBuilder& builder);

		//!	Flatbuffers serialization of updates only
		flatbuffers::Offset<O3DS::Data::SubjectUpdate> serializeUpdate(flatbuffers::FlatBufferBuilder& builder, size_t& count, double deltaThreshold);

		//! The name of the subject
		std::string   mName;

		//! Unique idenfitier for this subject
		std::string   mUuid;

		//! Subject transforms
		TransformList mTransforms;

		//! User reference pointer
		void*         mReference;

		//! Error string from last operation, e.g. allFinite or parsing error.
		std::string   mError;

		//! Verify all transforms are finite, sets mError with the name of the first invalid transform
		bool allFinite();

		//! Create a new (owned) transform object and return a reference
		Transform* addTransform(TransformBuilder* builder = nullptr);

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
		bool calcMatrices();

		//! Set to false to tell the parse to skip this subject while encoding.
		bool mEnabled;
	};


	/*! \class RigidbodySubject model.h o3ds/model.h
	 * A rigidbody, will usually only have one transform but > 1 is supported */
    //! A rigidbody - e.g. prop.

	class RigidbodySubject : public Subject
	{
	public:

		RigidbodySubject(void* ref = nullptr);

		RigidbodySubject(const std::string& name, const std::string& uuid, void* ref = nullptr);

		void parse(const O3DS::Data::Rigidbody* data, RigidbodyBuilder* builder = nullptr);

		void parseUpdate(const O3DS::Data::RigidbodyUpdate* inUpdate);

		//! Flatbuffer serialization
		flatbuffers::Offset<O3DS::Data::Rigidbody> serialize(flatbuffers::FlatBufferBuilder& builder);

		//!	Flatbuffers serialization of updates only
		flatbuffers::Offset<O3DS::Data::RigidbodyUpdate> serializeUpdate(flatbuffers::FlatBufferBuilder& builder, size_t& count, double deltaThreshold);
	};


	/*! \class PerformerSubject model.h o3ds/model.h 
 	 * A subject that has a hierarchy of joints (transforms)  */
	//! A skeleton subject.

	class PerformerSubject : public Subject
	{
	public:

		PerformerSubject(void* ref = nullptr);

		PerformerSubject(const std::string& name, const std::string& uuid, void* ref = nullptr);

		void parse(const O3DS::Data::Performer* data, JointBuilder* builder = nullptr);

		void parseUpdate(const O3DS::Data::PerformerUpdate* inUpdate);

		//! Flatbuffer serialization
		flatbuffers::Offset<O3DS::Data::Performer> serialize(flatbuffers::FlatBufferBuilder& builder);

		//!	Flatbuffers serialization of updates only
		flatbuffers::Offset<O3DS::Data::PerformerUpdate> serializeUpdate(flatbuffers::FlatBufferBuilder& builder, size_t& count, double deltaThreshold);

		//! Optional list of joints to send.  Mobu uses this to limit the joints that are to be sent
		std::vector<std::string> mJoints;
	};



	/*! \class CameraSubject model.h o3ds/model.h
	 * Representation of a 3d camera including some lens and filmback information
	 * Likely only has one transform but more than one are supported
	 */
	 //! Moving Camera, e.g. VCam.
	class CameraSubject : public Subject
	{
	public:

		EIGEN_MAKE_ALIGNED_OPERATOR_NEW

		CameraSubject(void* ref = nullptr);

		CameraSubject(const std::string& name, const std::string& uuid, void* ref = nullptr);

		//! Convert to flatbuffers using the given builder
		flatbuffers::Offset<O3DS::Data::Camera> serialize(flatbuffers::FlatBufferBuilder& builder);

		//! Parse the given flatbuffer data to populate this camera, no null checking.
		void parse(const O3DS::Data::Camera* data, CameraBuilder* camera = nullptr);

		//! Parse the given flatbuffer data to populate this camera, no null checking.
		void parseUpdate(const O3DS::Data::CameraUpdate*);

		//! Flatbuffers serialization of updates only
		flatbuffers::Offset<O3DS::Data::CameraUpdate> serializeUpdate(flatbuffers::FlatBufferBuilder& builder, size_t& count, double deltaThreshold);

		//! Film back width in mm  
		float filmBackWidth;

		//! Film back height in mm
		float filmBackHeight;

		//! Lens focal length in mm
		float focalLength;

		//! Aspect Ratio (crop?)
		float aspect;

		//! Focus distance in mm
		float focusDistance;

		//! Aperture in f-stops
		float aperture;
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
		template<typename T>
		T* findOrAddSubject(const std::string& uuid)
		{
			T* subject = findSubjectByUuid<T>(uuid);
			if (subject) { return subject; }
			auto s = std::make_unique<T>();
			s->mUuid = uuid;
			T* sptr = s.get();
			mItems.push_back(std::move(s));
			return sptr;
		}

		//! Find a subject by name.  nullptr if not found
		template<typename T>
		T* findSubjectByName(const std::string& name)
		{
			for (auto& i : mItems)
			{
				if (i->mName == name) {
					return dynamic_cast<T*>(i.get());
				}
			}
			return nullptr;
		}

		//! Find a subject by unique id.  nullptr if not found
		template<typename T>
		T* findSubjectByUuid(const std::string& uuid)
		{
			for (auto& i : mItems) {
				if (i->mUuid == uuid) {
					return dynamic_cast<T*>(i.get());
				}
			}
			return nullptr;
		}

		//! update transforms (virtual)
		void update();

		//! Subject item list (owned)
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

		//! Number of subjects
		size_t size() { return mItems.size(); }

		//! Number of active Subjects
		size_t activeCount() const;

		//! Raw pointer getter
		Subject* operator [] (int i) { return mItems[i].get(); }

		//! Time stamp of last serialization
		double mTime;

		//! Timecode of the frame/update
		std::string mTimecode;

		//! Threshold for delta updates
		double mDeltaThreshold;

		//! Error string from last parse
		std::string mError;

		//! Encode all of the items in the subject list as binary data
		bool serialize(std::vector<char> &outbuf, double timestamp=0.0, const std::string& timcode = std::string());

		//! Serialize changes to translation and rotation since last send
		bool serializeUpdate(std::vector<char>& outbuf, size_t& count, double timestamp=0.0, const std::string& timcode = std::string());

		//! Populate or update the subject list with the binary data provided (created by Serialize)
		bool parse(const char *data, 
			size_t len, 
			BuilderSet* builders = nullptr,
			bool clearInactive = false);

		//! Change distance threshold below which O3DS skips transmitting a transform update.
		void setDeltaThreshold(double newThreshold) { mDeltaThreshold = newThreshold; }

		//! The context for this subject ( yup / zup etc)
		Context       mContext;

	};

	//! Write the header, uint32 flags, uint32 checksum and data blob 
	void finalize(flatbuffers::FlatBufferBuilder& builder, std::vector<char>& outbuf, std::uint32_t flags);


} // O3DS


#endif
