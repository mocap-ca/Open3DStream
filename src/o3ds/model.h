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
#include <cstdint>

#include "context.h"
#include "transform_component.h"
#include "o3ds_generated.h"
#include "math.h"

namespace O3DS
{

	/*! \class TimeEnvelope model.h o3ds/model.h */
	//! The context a bare timestamp loses: which clock counted it, how far it
	//! can be trusted, and - for a timecode - at what rate and whether the
	//! frame field is drop-frame.
	//!
	//! SubjectList::mTime is a double of seconds and SubjectList::mTimecode a
	//! bare "HH:MM:SS:FF". A sender that has an embedded timecode on one frame
	//! and only an arrival time on the next puts two unrelated epochs in mTime
	//! under one name, and a receiver cannot tell them apart, nor rebuild the
	//! timecode, because a frame field means nothing without its rate. This
	//! travels beside them and says what they are.
	//!
	//! A sender fills it in and leaves mTime/mTimecode populated as well, so a
	//! receiver built before this existed is unaffected. A receiver reads it
	//! only when version != 0; a sender that predates it leaves version 0.
	//!
	//! Field-for-field the appended tail of SubjectList in o3ds.fbs. Append
	//! only - other products persist and exchange this.
	struct TimeEnvelope
	{
		//! Envelope version. 0 means "the sender set none of this".
		std::uint8_t  version = 0;

		//! Which clock counted ns, and the count. Same instant as
		//! SubjectList::mTime, exact rather than rounded through a double.
		//! domain: 0 app-steady, 1 host-system, 2 device, 3 SMPTE, 4 PTP,
		//! 5 NTP, 6 synthetic. instance separates two clocks of one kind.
		std::uint8_t  clockDomain   = 0;
		std::uint16_t clockInstance = 0;
		std::uint64_t ns            = 0;

		//! 0 locked (frame-accurate, genlocked device), 1 counted (derived by
		//! counting frames from an anchor - one dropped frame and it is
		//! permanently wrong), 2 estimated (accurate to arrival jitter).
		std::uint8_t  quality = 2;

		//! Which of the sender's stamps ns came from: 0 arrival, 1 device,
		//! 2 timecode, 3 restamped. When it is 3 the sender deliberately
		//! replaced the time its source gave, and restampReason says why:
		//! 1 clock conversion, 2 resample, 3 interpolation, 4 retime,
		//! 5 repair.
		std::uint8_t  source        = 0;
		std::uint8_t  restampReason = 0;

		//! The device's own clock, when it reports one - kept beside ns rather
		//! than folded into it.
		bool          hasDeviceTime  = false;
		std::uint16_t deviceInstance = 0;
		std::uint64_t deviceNs       = 0;

		//! Externally synchronised timecode, when the sample carried one.
		//! kind: 0 SMPTE, 1 NTP, 2 PTP, 3 custom.
		bool          hasTimecode      = false;
		std::uint8_t  timecodeKind     = 0;
		std::uint16_t timecodeInstance = 0;
		std::uint64_t timecodeNs       = 0;

		//! SMPTE fields, valid when hasSmpte. frames is 16-bit because SMPTE
		//! is only defined to 60 fps and a mocap rig streaming at 500 Hz needs
		//! three digits. rateCode indexes the sender's own rate table and is
		//! only meaningful to a receiver that shares it; rateNum/rateDen are
		//! the same rate as an exact rational that anyone can read, and
		//! dropFrame is carried on its own so a receiver with neither can
		//! still tell "01:00:00;00" from "01:00:00:00".
		bool          hasSmpte  = false;
		std::uint8_t  hours     = 0;
		std::uint8_t  minutes   = 0;
		std::uint8_t  seconds   = 0;
		std::uint16_t frames    = 0;
		std::uint16_t subframe  = 0;
		std::uint16_t subframes = 0;
		std::uint8_t  rateCode  = 0;
		bool          dropFrame = false;
		std::int32_t  rateNum   = 0;
		std::int32_t  rateDen   = 0;

		//! True when a sender actually filled this in.
		bool valid() const { return version != 0; }
	};

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

		//! Set Transform order to trs
		void setOrderTRS();

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

	//! Platform specific builder to make a performer joint object
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

		//! Translation, Rotation Scale order
		void setOrderTRS();

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
		bool calcMatrices(O3DS::ConversionContext *conv);

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

		//! Calculate the world matrices, with conversions applied
		bool calcMatrices();

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

		//! What mTime and mTimecode actually are - clock, quality, rate,
		//! drop-frame. A sender sets this before serialize()/serializeUpdate()
		//! the same way it sets mContext; parse() fills it from the packet, or
		//! leaves version 0 when the sender predates it. See TimeEnvelope.
		TimeEnvelope mTimeEnvelope;

		//! Threshold for delta updates
		double mDeltaThreshold;

		//! Error string from last parse
		std::string mError;

		//! Encode all of the items in the subject list as binary data
		[[nodiscard]] bool serialize(std::vector<char> &outbuf, double timestamp=0.0, const std::string& timcode = std::string());

		//! Serialize changes to translation and rotation since last send
		[[nodiscard]] bool serializeUpdate(std::vector<char>& outbuf, size_t& count, double timestamp=0.0, const std::string& timcode = std::string());

		//! Populate or update the subject list with the binary data provided (created by Serialize)
		[[nodiscard]] bool parse(const char *data,
			size_t len, 
			BuilderSet* builders = nullptr,
			bool clearInactive = false);

		//! Change distance threshold below which O3DS skips transmitting a transform update.
		void setDeltaThreshold(double newThreshold) { mDeltaThreshold = newThreshold; }

		//! The context for this stream ( yup / zup etc)
		Context       mContext;

		std::unique_ptr<ConversionContext> conversion;


	};

	//! Write the header, uint32 flags, uint32 checksum and data blob 
	void finalize(flatbuffers::FlatBufferBuilder& builder, std::vector<char>& outbuf, std::uint32_t flags);


} // O3DS


#endif
