#include <fbxsdk.h>
//#include <fbxfilesdk/fbxio/fbxiosettings.h>

// https://help.autodesk.com/view/FBX/2017/ENU/?guid=__files_GUID_ABFC58F1_C10D_42AE_A204_D79EB1D84B24_htm
// https://help.autodesk.com/cloudhelp/2017/ENU/FBX-Developer-Help/cpp_ref/_import_scene_2_display_animation_8cxx-example.html

using namespace fbxsdk;

#include "schema_generated.h"
using namespace MyGame::Sample;

#include "windows.h"

#include "get_time.h"
#include "cml/cml.h"

#include "Open3dStreamModel.h"

namespace O3DS
{

	class MobuUpdater : public O3DS::Updater
	{
	public:
		MobuUpdater(FbxNode *node) : mNode(node) {};

		void update(O3DS::Transform *t)
		{
			FbxDouble3 trans = mNode->LclTranslation.EvaluateValue(FbxTime(0));
			FbxDouble3 rot = mNode->LclRotation.EvaluateValue(FbxTime(0));

			cml::euler_order corder = cml::euler_order_xyz;
			switch (mNode->RotationOrder)
			{
			case FbxEuler::eOrderXYZ: corder = cml::euler_order_xyz;
			case FbxEuler::eOrderXZY: corder = cml::euler_order_xzy;
			case FbxEuler::eOrderYXZ: corder = cml::euler_order_yxz;
			case FbxEuler::eOrderYZX: corder = cml::euler_order_yzx;
			case FbxEuler::eOrderZXY: corder = cml::euler_order_zxy;
			case FbxEuler::eOrderZYX: corder = cml::euler_order_zyx;
			}

			cml::vector3d ctrans(trans.mData);
			cml::vector3d crot(rot.mData);

			cml::matrix_set_translation(t->mMatrix, ctrans);
			cml::matrix_rotation_euler(t->mMatrix, crot, corder);
		}

		std::string info()
		{
			std::string ret;
			ret = mNode->GetName();
			ret += " ";
			ret += mNode->GetTypeName();
			return ret;
		}

		FbxNode *mNode;
	};

	std::string GetNamespace(fbxsdk::FbxNode *node)
	{
		std::string name(node->GetName());
		size_t split = name.find(':');
		if (split == -1) return std::string();
		return name.substr(0, split);
	}

	void ListChildren(fbxsdk::FbxNode *node, std::string ns, O3DS::Subject *subject, int parentId = -1)
	{
		// Add node and follow down the tree for nodes with the same namespace
		std::string name(node->GetName());
		name = name.substr(ns.size() + 1, name.size());

		auto t = subject->addTransform(name, parentId, new MobuUpdater(node));

		for (int i = 0; i < node->GetChildCount(); i++)
		{

			fbxsdk::FbxNode *child = node->GetChild(i);

			if (strcmp(child->GetTypeName(), "Mesh") == 0)
				continue;

			if(GetNamespace(child) == ns)
				ListChildren(child, ns, subject, subject->size() - 1);
		}
	}

	void FindSubjects(fbxsdk::FbxNode *node, O3DS::SubjectList &subjects)
	{
		// Search node looking for objects that have a namespace
		std::string ns = GetNamespace(node);
		if (ns.size() == 0)
		{
			for (int i = 0; i < node->GetChildCount(); i++)
			{
				FindSubjects(node->GetChild(i), subjects);
			}
		}
		else
		{
			auto subject = subjects.addSubject(std::string(ns));
			ListChildren(node, ns, subject, -1);
		}
	}
} // O3DS





int main(int argc, char *argv[])
{
	// Init - Create the manager
	FbxManager* manager = FbxManager::Create();
    FbxScene* lScene = NULL;
	
	FbxIOSettings *settings = FbxIOSettings::Create(manager, IOSROOT);

	settings->SetBoolProp(IMP_FBX_MATERIAL, true);
	settings->SetBoolProp(IMP_FBX_TEXTURE, true);
	settings->SetBoolProp(IMP_FBX_LINK, false);
	settings->SetBoolProp(IMP_FBX_SHAPE, false);
	settings->SetBoolProp(IMP_FBX_GOBO, false);
	settings->SetBoolProp(IMP_FBX_ANIMATION, true);
	settings->SetBoolProp(IMP_FBX_GLOBAL_SETTINGS, true);

	manager->SetIOSettings(settings);

	FbxImporter* lImporter = FbxImporter::Create(manager, "");

	// Load the fbx file
	const char *file_path = "c:\\users\\al\\Desktop\\test.fbx";

	bool lImportStatus = lImporter->Initialize(file_path, -1, manager->GetIOSettings());

	if (!lImportStatus) {
		printf("Call to FbxImporter::Initialize() failed.\n");
		printf("Error returned: %s\n\n", lImporter->GetStatus().GetErrorString());
		exit(-1);
	}

	// Create a new scene so it can be populated by the imported file.
	lScene = FbxScene::Create(manager, "myScene");

	// Import the contents of the file into the scene.
	if (!lImporter->Import(lScene))
	{
		printf("Could not import file\n");
		return 1;
	}
	lImporter->Destroy();

	int lFileMajor = 0, lFileMinor = 0, lFileRevision = 0;
	lImporter->GetFileVersion(lFileMajor, lFileMinor, lFileRevision);

	printf("Version: %d %d %d\n", lFileMajor, lFileMinor, lFileRevision);

	// Get the scene time information

	FbxGlobalSettings& globalSettings = lScene->GetGlobalSettings();

	FbxTime::EMode timeMode = globalSettings.GetTimeMode();

	switch (timeMode)
	{
	case FbxTime::eDefaultMode: printf("default\n"); break;
	case FbxTime::eFrames120: printf("eFrames120 \n"); break;
	case FbxTime::eFrames100: printf("eFrames100\n"); break;
	case FbxTime::eFrames60: printf("eFrames60\n"); break;
	case FbxTime::eFrames50: printf("eFrames50 \n"); break;
	case FbxTime::eFrames48: printf("eFrames48 \n"); break;
	case FbxTime::eFrames30: printf("eFrames30 \n"); break;
	case FbxTime::eFrames30Drop: printf("eFrames30Drop \n"); break;
	case FbxTime::eNTSCDropFrame: printf("eNTSCDropFrame \n"); break;
	case FbxTime::eNTSCFullFrame: printf("eNTSCFullFrame \n"); break;
	case FbxTime::ePAL: printf("ePAL\n"); break;
	case FbxTime::eFrames24: printf("eFrames24 \n"); break;
	case FbxTime::eFrames1000: printf("eFrames1000 \n"); break;
	case FbxTime::eFilmFullFrame: printf("eFilmFullFrame \n"); break;
	case FbxTime::eCustom: printf("eCustom \n"); break;
	case FbxTime::eFrames96: printf("eFrames96 \n"); break;
	case FbxTime::eFrames72: printf("eFrames72 \n"); break;
	case FbxTime::eFrames59dot94: printf("eFrames59dot94  \n"); break;
	case FbxTime::eModesCount: printf("eModesCount  \n"); break;
	}

	double frameRate;

	if (timeMode == FbxTime::eCustom)
	{
		frameRate = globalSettings.GetCustomFrameRate();
	}
	else
	{
		frameRate = FbxTime::GetFrameRate(timeMode);
	}

	FbxTime tInc = FbxTime();
	tInc.SetFrame(1);

	double zerof = GetTime() * 1000.;


	// Get the anim stacks and default stack start/end

	fbxsdk::FbxAnimStack *stack = lScene->GetCurrentAnimationStack();
	printf("Stack: %s\n", stack->GetName());

	fbxsdk::FbxNode *root = lScene->GetRootNode();
	printf("Root: %s\n", root->GetName());

	FbxTime t1 = stack->LocalStart;
	FbxTime t2 = stack->LocalStop;

	printf("Start: %s\n", t1.GetTimeString().operator const char *());
	printf("End:   %s\n", t2.GetTimeString().operator const char *());
	printf("Rate:  %f\n", frameRate);

	
	// Traverse the scene for models

	O3DS::SubjectList subjects;

	O3DS::FindSubjects(root, subjects);

	for (auto s : subjects)
	{
		printf("Subject: %s\n", s->mName.c_str());
		for (auto i : s->mTransforms)
		{
			printf("  %s\n", i->mVisitor->info().c_str());
		}
	}

	// Serialize

	uint8_t buffer[1024 * 16];

	bool first = true;

	for (FbxTime t = t1.GetFramedTime(); t < t2.GetFramedTime(); t = t + tInc)
	{
		double tick = GetTime() * 1000. - zerof;
		double delay = t.GetSecondDouble() * 1000 - tick;
		printf("%f   %s   %f\n", tick, t.GetTimeString().operator const char *(), delay);

		subjects.update();

		int ret = O3DS::Serialize(subjects, buffer, 1024 * 16, first);
		first = false;

		printf("%d bytes\n", ret);

		if (delay > 0)
			Sleep(delay);
	}


	return 0;

}

