#include "xsens_parser.h"
#include "binary_stream.h"

const std::vector<std::string> xsens_names = {
		"Root",
		"Pelvis",
		"L5",
		"L3",
		"T12",
		"T8",
		"Neck",
		"Head",
		"RightShoulder",
		"RightUpperArm",
		"RightForeArm",
		"RightHand",
		"LeftShoulder",
		"LeftUpperArm",
		"LeftForeArm",
		"LeftHand",
		"RightUpperLeg",
		"RightLowerLeg",
		"RightFoot",
		"RightToe",
		"LeftUpperLeg",
		"LeftLowerLeg",
		"LeftFoot",
		"LeftToe",
		"Prop1",
		"Prop2",
		"Prop3",
		"Prop4",
		"LeftCarpus",
		"LeftFirstMC",
		"LeftFirstPP",
		"LeftFirstDP",
		"LeftSecondMC",
		"LeftSecondPP",
		"LeftSecondMP",
		"LeftSecondDP",
		"LeftThirdMC",
		"LeftThirdPP",
		"LeftThirdMP",
		"LeftThirdDP",
		"LeftFourthMC",
		"LeftFourthPP",
		"LeftFourthMP",
		"LeftFourthDP",
		"LeftFifthMC",
		"LeftFifthPP",
		"LeftFifthMP",
		"LeftFifthDP",
		"RightCarpus",
		"RightFirstMC",
		"RightFirstPP",
		"RightFirstDP",
		"RightSecondMC",
		"RightSecondPP",
		"RightSecondMP",
		"RightSecondDP",
		"RightThirdMC",
		"RightThirdPP",
		"RightThirdMP",
		"RightThirdDP",
		"RightFourthMC",
		"RightFourthPP",
		"RightFourthMP",
		"RightFourthDP",
		"RightFifthMC",
		"RightFifthPP",
		"RightFifthMP",
		"RightFifthDP",
};

const std::vector<int32_t> parentIndex = { -1, 0, 1, 2, 3, 4, 5, 6, 5, 8, 9, 10, 5, 12, 13, 14, 1, 16, 17, 18, 1, 20, 21, 22, 0, 0, 0, 0,
										14, 28, 29, 30, 28, 32, 33, 34, 28, 36, 37, 38, 28, 40, 41, 42, 28, 44, 45, 46,		//left hand
										10, 48, 49, 50, 48, 52, 53, 54, 48, 56, 57, 58, 48, 60, 61, 62, 48, 64, 65, 66 };	//right hand


enum class SegmentIndexes
{
	Pelvis = 0,
	L5 = 1,
	L3 = 2,
	T12 = 3,
	T8 = 4,
	Neck = 5,
	Head = 6,
	RightShoulder = 7,
	RightUpperArm = 8,
	RightForearm = 9,
	RightHand = 10,
	LeftShoulder = 11,
	LeftUpperArm = 12,
	LeftForearm = 13,
	LeftHand = 14,
	RightUpperLeg = 15,
	RightLowerLeg = 16,
	RightFoot = 17,
	RightToe = 18,
	LeftUpperLeg = 19,
	LeftLowerLeg = 20,
	LeftFoot = 21,
	LeftToe = 22,
	Prop1 = 23,
	Prop2 = 24,
	Prop3 = 25,
	Prop4 = 26,

	XS_SEG_NUM = 27, //number of segments
	XS_SEG_NUM_FINGERS = 67//number of segments with fingers enabled
};

enum class  O3DS::XSENS::ProtocolID O3DS::XSENS::get_protocol_id(uint8_t* data)
{
	char buf[3];
	buf[0] = data[0];
	buf[1] = data[1];
	buf[2] = 0;

	switch (std::stoi(buf, nullptr, 16)) {
	case 0x01: return O3DS::XSENS::ProtocolID::PoseEuler;
	case 0x02: return O3DS::XSENS::ProtocolID::PoseQuaternion;
	case 0x03: return O3DS::XSENS::ProtocolID::PosePositions;
	case 0x04: return O3DS::XSENS::ProtocolID::JackProcessSimulate;
	case 0x05: return O3DS::XSENS::ProtocolID::PoseUnity3D;
	case 0x12: return O3DS::XSENS::ProtocolID::Meta;
	case 0x13: return O3DS::XSENS::ProtocolID::Scale;
	case 0x20: return O3DS::XSENS::ProtocolID::JointAngles;
	case 0x21: return O3DS::XSENS::ProtocolID::LinearSegmentKinematics;
	case 0x22: return O3DS::XSENS::ProtocolID::AngularSegmentKinematics;
	case 0x23: return O3DS::XSENS::ProtocolID::TrackerKinematics;
	case 0x24: return O3DS::XSENS::ProtocolID::CenterOfMass;
	case 0x25: return O3DS::XSENS::ProtocolID::TimeCode;
	}

	return O3DS::XSENS::ProtocolID::Error;
}

std::string O3DS::XSENS::protocol_name(enum class O3DS::XSENS::ProtocolID id)
{
	switch (id)
	{
	case O3DS::XSENS::ProtocolID::PoseEuler: return "Euler Pose";
	case O3DS::XSENS::ProtocolID::PoseQuaternion: return "Quaternion Pose";
	case O3DS::XSENS::ProtocolID::PosePositions: return "Position Pose";
	case O3DS::XSENS::ProtocolID::JackProcessSimulate: return "Jack Process Simulate";
	case O3DS::XSENS::ProtocolID::PoseUnity3D: return "Unity Pose";
	case O3DS::XSENS::ProtocolID::Meta: return "Meta Data";
	case O3DS::XSENS::ProtocolID::Scale: return "Scale";
	case O3DS::XSENS::ProtocolID::JointAngles: return "Joint Angles";
	case O3DS::XSENS::ProtocolID::LinearSegmentKinematics: return "Linear Segment Kinematics";
	case O3DS::XSENS::ProtocolID::AngularSegmentKinematics: return "Angular Segment Kinematics";
	case O3DS::XSENS::ProtocolID::TrackerKinematics: return "Tracker Kinematics";
	case O3DS::XSENS::ProtocolID::CenterOfMass: return "Center of Mass";
	case O3DS::XSENS::ProtocolID::TimeCode: return "Timecode";
	case O3DS::XSENS::ProtocolID::Error: return "Error";
	}

	return "INVALID";
}

int32_t O3DS::XSENS::segment_parent(int32_t id)
{
	if (id >= 0 && id < parentIndex.size())
		return parentIndex[id];

	return -1;
}

std::string O3DS::XSENS::segment_name(int32_t id)
{
	if (id >= 0 && id < xsens_names.size())
		return xsens_names[id];

	char buf[8];
	snprintf(buf, 8, "#%d", id);
	return std::string(buf);
}

void O3DS::XSENS::Parser::get_quaternion_pose(uint8_t* payload, size_t len)
{
	O3DS::Subject* subject = getSubject();

	O3DS::BinaryStream stream(payload, len);
	stream.swaporder = true;
	uint32_t segmentId;
	float x, y, z, q0, q1, q2, q3;

	subject->clear();

	O3DS::Transform* transform = nullptr;

	transform = subject->addTransform(std::string("Root"), -1);

	auto ref = this->info.find(head.characterId);
	if (ref == this->info.end())
		return;

	auto meta = ref->second;
	

	int n = 1;
	int i = 0;
	while (!stream.done())
	{
		stream.get(segmentId);
		stream.get(x);
		stream.get(y);
		stream.get(z);
		stream.get(q0);
		stream.get(q1);
		stream.get(q2);
		stream.get(q3);		

		if (n >= xsens_names.size())
			continue;
		
		std::string name;
		
		if (segmentId > 0 && segmentId - 1 < meta.names.size())
		{
			name = meta.names[segmentId - 1];
		}

		while (xsens_names[n] != name && n < xsens_names.size())
		{
			// Add missing props
			std::string nn = xsens_names[n];
			transform = subject->addTransform(nn, 0);
			n++;
		}
		
		if (n == xsens_names.size())
			return;

		int parentId = parentIndex[n];

		transform = subject->addTransform(name, parentId);

		transform->transformOrder.push_back(O3DS::ComponentType::TTranslation);
		transform->transformOrder.push_back(O3DS::ComponentType::TRotation);
		transform->transformOrder.push_back(O3DS::ComponentType::TScale);

		transform->translation.value[0] = x * 100.0;
		transform->translation.value[1] = -y * 100.0;
		transform->translation.value[2] = z * 100.0;

		transform->rotation.value[0] = -q1;
		transform->rotation.value[1] = q2;
		transform->rotation.value[2] = -q3;
		transform->rotation.value[3] = q0;

		if (n < meta.scale.size())
				transform->scale.value = meta.scale[i];

		n++;
		i++;
	}
}


O3DS::XSENS::Parser::Parser() 
{}


void O3DS::XSENS::Parser::get_scale(uint8_t* payload, size_t len)
{
	O3DS::Subject* subject = getSubject();

	O3DS::BinaryStream stream(payload, len);
	stream.swaporder = true;

	uint32_t segment_count;
	
	stream.get(segment_count);

	if (segment_count == 0)
		return;

	uint32_t name_length;
	char name[255];

	float x, y, z;

	auto ref = this->info.find(head.characterId);
	if (ref == this->info.end())
	{
		this->info[head.characterId] = SubjectInfo();
	}

	SubjectInfo& meta = this->info[head.characterId];
	meta.names.clear();
	meta.scale.clear();

	for (uint32_t i = 0; i < segment_count && !stream.done(); i++)
	{
		stream.get(name_length);
		if (name_length >= 254) return;

		stream.get(name, name_length);
		name[name_length] = 0;

		stream.get(x);
		stream.get(y);
		stream.get(z);

		meta.names.push_back(name);
		meta.scale.push_back(O3DS::Vector3d(x * 100.0, y * -100.0, z * 100.0));
	}
}


bool O3DS::XSENS::Parser::get_header(uint8_t* payload, size_t len)
{
	O3DS::BinaryStream stream(payload, len);
	stream.swaporder = true;

	stream.get(head.sampleCounter);
	stream.get(head.datagramCounter);
	stream.get(head.numberOfItems);
	stream.get((uint8_t*)&head.timecode, 4);
	stream.get(head.characterId);
	stream.get(head.bodySegments);
	stream.get(head.props);
	stream.get(head.fingers);
	stream.get(head.reserved);
	stream.get(head.payloadSize);

	return true;
}

void O3DS::XSENS::Parser::print_header()
{
	printf("Sample Counter  : %d\n", head.sampleCounter);
	printf("Datagram Counter: %d\n", head.datagramCounter);
	printf("Items           : %d\n", head.numberOfItems);
	printf("Timecode        : %d %d %d %d\n", head.timecode[0], head.timecode[1], head.timecode[2], head.timecode[3]);
	printf("Character Id    : %d\n", head.characterId);
	printf("Body Segments   : %d\n", head.bodySegments);
	printf("Props           : %d\n", head.props);
	printf("Fingers         : %d\n", head.fingers);
	printf("payloadSize     : %d\n", head.payloadSize);
}

void O3DS::XSENS::Parser::get_meta(uint8_t* payload, size_t len, std::string &name)
{
	O3DS::BinaryStream stream(payload, len);
	stream.swaporder = true;

	uint32_t sz;

	stream.get(sz);

	if (sz > 254) return;

	char buf[255];

	stream.get(buf, sz);
	buf[sz] = 0;

	printf("Meta: %s\n", buf);

	std::stringstream ss(buf);
	std::string line;
	size_t pos;

	while (std::getline(ss, line, '\n'))
	{
		pos = line.find(':');
		if (pos != std::string::npos)
		{
			std::string key = line.substr(0, pos);
			std::string value = line.substr(pos+1);

			if (key == "name") {
				getSubject()->mName = value;
			}
		}
	}
}


O3DS::Subject* O3DS::XSENS::Parser::getSubject()
{
	while (subjects.mItems.size() <= head.characterId)
	{
		subjects.addSubject("");
	}

	return subjects.mItems[head.characterId];
}