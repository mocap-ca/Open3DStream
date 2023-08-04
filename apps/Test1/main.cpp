#pragma once
 
 
#include "o3ds/async_publisher.h"
#include "o3ds/model.h"
#include "o3ds/o3ds.h"
#include <chrono>
#include <iostream>

void verify(std::vector<char> buffer, double rotate, O3DS::SubjectList &subjectList)
{

	

	if (!subjectList.Parse(buffer.data(), buffer.size()))
	{
		std::cout << "Could not parse data" << std::endl;
		return;
	}

	O3DS::Subject* subjectCopy = subjectList.findSubject("Test1");
	if (subjectCopy == nullptr)
	{
		std::cout << "Verify failed on find subject" << std::endl;
		return;
	}

	if (subjectCopy->mTransforms.size() != 2)
	{
		std::cout << "Verify failed on transform count" << std::endl;
		return;
	}

	auto rot_mat = O3DS::Matrix<double>::RotateX(rotate);
	auto value = rot_mat.GetQuaternion();

	O3DS::Transform* bone = subjectCopy->mTransforms.find("Root");
	if (bone == nullptr)
	{
		std::cout << "Verify failed on finding bone" << std::endl;
		return;
	}

	for (int i = 0; i < 4; i++)
	{
		// Do a float precition test
		float a = bone->rotation.value.v[i];
		float b = value.v[i];
		if (a != b)
		{
			std::cout << "Verify failed on bone channel " << i << " " << a - b << std::endl;
			return;
		}
	}
	

}
 
int main(int arc, char** argv)
{
	O3DS::AsyncPublisher publisher;

	if (publisher.start("INVALIDTEXT")) {
		std::cout << "Able to start invalid openStream server" << std::endl;
		return 1;
	}


	if (!publisher.start("tcp://127.0.0.1:3001")) {
		std::cout << "Unable to start OpenStream Publish Server: " << publisher.getError() << std::endl;
		return 1;
	}

	O3DS::SubjectList subjectList;

	O3DS::SubjectList subjectListCopy;
	

	O3DS::Subject *subject = subjectList.addSubject("Test1");

	O3DS::Transform* root = subject->addTransform(std::string("Root"), -1);
	O3DS::Transform* bone = subject->addTransform(std::string("Bone"), 0);

	root->transformOrder.push_back(O3DS::ComponentType::TTranslation);
	root->transformOrder.push_back(O3DS::ComponentType::TRotation);
	root->transformOrder.push_back(O3DS::ComponentType::TScale);

	bone->transformOrder.push_back(O3DS::ComponentType::TTranslation);
	bone->transformOrder.push_back(O3DS::ComponentType::TRotation);
	bone->transformOrder.push_back(O3DS::ComponentType::TScale);

	bone->translation.value.v[0] = 10;

	double rotate = 1;
	size_t frame = 0;

	while (1)
	{
		auto rot_mat = O3DS::Matrix<double>::RotateX(rotate);
		root->rotation.value = rot_mat.GetQuaternion();

		std::vector<char> buffer;
		std::chrono::time_point< std::chrono::system_clock > now = std::chrono::system_clock::now();
		auto duration = now.time_since_epoch();

		int ret;
		if (frame % 100 != 0) {
			size_t count = 0;
			ret = subjectList.SerializeUpdate(buffer, count, (double)std::chrono::duration_cast<std::chrono::nanoseconds>(duration).count());
		}
		else {
			ret = subjectList.Serialize(buffer, (double)std::chrono::duration_cast<std::chrono::nanoseconds>(duration).count());
		}

		if (ret > 0) {
			auto ptr = (const char*)&buffer[0];
			if (!publisher.write(ptr, ret)) {
				std::cout << "Could not write frame" << std::endl;
			}
		}
		else
		{
			std::cout <<  "No data created" << std::endl;
		}

		verify(buffer, rotate, subjectListCopy);

		std::this_thread::sleep_for(std::chrono::milliseconds(100));

		frame++;
		rotate += 0.01;

	}
 
 
 
	return 0;
}