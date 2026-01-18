//#include "o3ds/async_publisher.h"
#include "o3ds/model.h"
#include "o3ds/o3ds.h"
#include "o3ds/o3ds_version.h"
#include <chrono>
#include <iostream>
#include <thread>

void verify(std::vector<char> buffer, O3DS::SubjectList &subjectList)
{
	

	

}
 
int main(int arc, char** argv)
{
	std::cout << O3DS::getVersion();

	O3DS::SubjectList subjectList;


	O3DS::Subject *subject = subjectList.addSubject("Test1");

	O3DS::Transform* root = subject->addTransform(std::string("Root"), -1);
	O3DS::Transform* bone = subject->addTransform(std::string("Bone"), 0);

	root->transformOrder.push_back(O3DS::ComponentType::TTranslation);
	root->transformOrder.push_back(O3DS::ComponentType::TRotation);
	root->transformOrder.push_back(O3DS::ComponentType::TScale);

	bone->transformOrder.push_back(O3DS::ComponentType::TTranslation);
	bone->transformOrder.push_back(O3DS::ComponentType::TRotation);
	bone->transformOrder.push_back(O3DS::ComponentType::TScale);

	root->translation.value = Eigen::Vector3d(10.0, 11.0, 12.0);
	root->rotation.value = O3DS::toQuaternion(O3DS::rotateX(2.0));

	bone->translation.value = Eigen::Vector3d(1.0, 2.0, 3.0);
	bone->rotation.value = O3DS::toQuaternion(O3DS::rotateY(1.0));

	std::vector<char> buffer;
	std::chrono::time_point< std::chrono::system_clock > now = std::chrono::system_clock::now();
	auto duration = now.time_since_epoch();
	double timeval = (double)std::chrono::duration_cast<std::chrono::nanoseconds>(duration).count();

	int ret = subjectList.Serialize(buffer, timeval);

	// ret = subjectList.SerializeUpdate(buffer, count, (double)std::chrono::duration_cast<std::chrono::nanoseconds>(duration).count());

	// Verify a copy

	O3DS::SubjectList subjectListCopy;

	if (!subjectListCopy.Parse(buffer.data(), buffer.size()))
	{
		std::cout << "Could not parse data" << std::endl;
		return 1;
	}

	O3DS::Subject* subjectCopy = subjectListCopy.findSubject("Test1");
	if (subjectCopy == nullptr)
	{
		std::cout << "Verify failed on find subject" << std::endl;
		return 1;
	}

	if(subjectCopy->mTransforms.operator==(subject->mTransforms) == false)
	{
		std::cout << "Verify failed on transform compare" << std::endl;
		return 1;
	}


 
 
 
	return 0;
}