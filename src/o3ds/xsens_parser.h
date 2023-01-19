/*
Open 3D Stream

Copyright 2022 Alastair Macleod

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

#ifndef O3DS_XSENS_PARSER_H
#define O3DS_XSENS_PARSER_H

#include <stdint.h>
#include <string>
#include <map>

#include "o3ds/model.h"

namespace O3DS
{
	namespace XSENS
	{
		enum class ProtocolID {
			PoseEuler                = 0x01,
			PoseQuaternion           = 0x02,
			PosePositions            = 0x03,
			JackProcessSimulate      = 0x04,
			PoseUnity3D              = 0x05,
			Meta                     = 0x12,
			Scale                    = 0x13,
			JointAngles              = 0x20,
			LinearSegmentKinematics  = 0x21,
			AngularSegmentKinematics = 0x22,
			TrackerKinematics        = 0x23,
			CenterOfMass             = 0x24,
			TimeCode                 = 0x25,
			Error                    = 0xff,
		};

		struct header 
		{
			uint32_t  sampleCounter;
			uint8_t   datagramCounter;
			uint8_t   numberOfItems;
			char      timecode[4];
			uint8_t   characterId;
			uint8_t   bodySegments;
			uint8_t   props;
			uint8_t   fingers;
			uint16_t  reserved;
			uint16_t  payloadSize;
		};

		enum class ProtocolID get_protocol_id(uint8_t*);

		int32_t segment_parent(int32_t id);

		std::string protocol_name(enum class ProtocolID);

		std::string segment_name(int32_t id);

		struct SubjectInfo
		{
			std::vector<std::string> names;
			std::vector<O3DS::Vector3d> scale;
		};

		class Parser
		{
		public:
			Parser();

			bool parse(const char* data, size_t sz);

			O3DS::Subject* getSubject();

			O3DS::SubjectList subjects;

			bool get_header(uint8_t* buf, size_t sz);

			void print_header();

			void get_scale(uint8_t* payload, size_t len);

			void get_meta(uint8_t* payload, size_t len, std::string& name);

			void get_quaternion_pose(uint8_t* payload, size_t len);

			struct header head;

			std::map<int, SubjectInfo > info;

			std::string system_name;


		};


	}
}



#endif