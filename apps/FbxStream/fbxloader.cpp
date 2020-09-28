#include "fbxloader.h"

// https://help.autodesk.com/view/FBX/2017/ENU/?guid=__files_GUID_ABFC58F1_C10D_42AE_A204_D79EB1D84B24_htm
// https://help.autodesk.com/cloudhelp/2017/ENU/FBX-Developer-Help/cpp_ref/_import_scene_2_display_animation_8cxx-example.html

//#include <fbxfilesdk/fbxio/fbxiosettings.h>

#include "o3ds/math.h"
using namespace fbxsdk;

namespace O3DS
{

	// Fbx -> O3DS assignment
	void Assign(O3DS::Matrix<double> &dst, FbxAMatrix &src)
	{
		for (int u = 0; u < 4; u++)
			for (int v = 0; v < 4; v++)
				dst.m[u][v] = src[u][v];
	}

	MobuUpdater::MobuUpdater(FbxNode *node)
		: mNode(node)
	{};

	void MobuUpdater::update(O3DS::Transform *t)
	{
		// Update the transform with the data in the FbxNode (mNode)
		O3DS::Matrix<double> oMatrix;
		O3DS::Matrix<double> oMatrixOffset = O3DS::Matrix<double>::RotateX(1.5708);

		Assign(oMatrix, mNode->EvaluateGlobalTransform(mTime));

		O3DS::Vector<double> tt = oMatrix.GetTranslation();
		O3DS::Vector<double> rr = oMatrix.GetQuaternion();

		oMatrix = oMatrix * oMatrixOffset;
			
		t->mMatrix(1, 0) = -oMatrix(1, 0);
		t->mMatrix(1, 1) =  oMatrix(1, 1);
		t->mMatrix(1, 2) = -oMatrix(1, 2);
		t->mMatrix(1, 3) = -oMatrix(1, 3);

		for (int j = 0; j < 4; ++j)
		{
			if (j == 1)
				continue;
			t->mMatrix(j, 0) =  oMatrix(j, 0);
			t->mMatrix(j, 1) = -oMatrix(j, 1);
			t->mMatrix(j, 2) =  oMatrix(j, 2);
			t->mMatrix(j, 3) =  oMatrix(j, 3);
		}

		tt = oMatrix.GetTranslation();
		rr = oMatrix.GetQuaternion();

		// const char *name = mNode->GetName();

		/*
		FbxDouble3 prerot = mNode->PreRotation.EvaluateValue(mTime);
		FbxDouble3 trans = mNode->LclTranslation.EvaluateValue(mTime);
		FbxDouble3 rot = mNode->LclRotation.EvaluateValue(mTime);


		cml::euler_order corder = cml::euler_order_xyz;
		switch (mNode->RotationOrder)
		{
		case FbxEuler::eOrderXYZ: corder = cml::euler_order_xyz; break;
		case FbxEuler::eOrderXZY: corder = cml::euler_order_xzy; break;
		case FbxEuler::eOrderYXZ: corder = cml::euler_order_yxz; break;
		case FbxEuler::eOrderYZX: corder = cml::euler_order_yzx; break;
		case FbxEuler::eOrderZXY: corder = cml::euler_order_zxy; break;
		case FbxEuler::eOrderZYX: corder = cml::euler_order_zyx; break;
		}

		cml::vector3d crot(rot.mData);
		cml::quaternion_rotation_euler(t->mOrientation, crot, corder);
			
		t->mTranslation = cml::vector3d(trans.mData);
		*/
	}

	std::string MobuUpdater::info()
	{
		std::string ret;
		ret = mNode->GetName();
		ret += " ";
		ret += mNode->GetTypeName();
		return ret;
	}

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
		int this_pid = (int)subject->size() - 1;

		for (int i = 0; i < node->GetChildCount(); i++)
		{

			fbxsdk::FbxNode *child = node->GetChild(i);

			if (strcmp(child->GetTypeName(), "Mesh") == 0)
				continue;

			if (GetNamespace(child) == ns)
				ListChildren(child, ns, subject, this_pid);
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
			if (strcmp(node->GetTypeName(), "Mesh") == 0)
				return;

			auto subject = subjects.addSubject(std::string(ns));
			ListChildren(node, ns, subject, -1);
		}
	}

	void SetTime(O3DS::SubjectList &subjects, FbxTime time)
	{
		for (auto subject : subjects)
		{
			for (auto transform : subject->mTransforms)
			{
				MobuUpdater *updater = (MobuUpdater*)transform->mVisitor;
				updater->mTime = time;
			}
		}
	}

	int Load(const char *file_path, O3DS::SubjectList& subjects, TimeInfo &time_info)
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

		time_info.Start = t1.GetFramedTime();
		time_info.End = t2.GetFramedTime();
		time_info.Inc = tInc;

		// Traverse the scene for models

		O3DS::FindSubjects(root, subjects);

		return 0;


	}

} // O3DS

