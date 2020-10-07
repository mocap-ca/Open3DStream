#include "fbxloader.h"

// https://help.autodesk.com/view/FBX/2017/ENU/?guid=__files_GUID_ABFC58F1_C10D_42AE_A204_D79EB1D84B24_htm
// https://help.autodesk.com/cloudhelp/2017/ENU/FBX-Developer-Help/cpp_ref/_import_scene_2_display_animation_8cxx-example.html

//#include <fbxfilesdk/fbxio/fbxiosettings.h>

#include "o3ds/math.h"
using namespace fbxsdk;

namespace O3DS
{
namespace Fb
{

	// Fbx -> O3DS assignment

	void operator >>(const FbxVector4& src, O3DS::Vector4d& dst)
	{
		dst.v[0] = src[0];
		dst.v[1] = src[1];
		dst.v[2] = src[2];
		dst.v[3] = src[3];
	}

	void operator >>(const FbxVector4& src, O3DS::Vector3d& dst)
	{
		dst.v[0] = src[0];
		dst.v[1] = src[1];
		dst.v[2] = src[2];
	}

	void operator >>(FbxDouble3& src, O3DS::Vector3d& dst)
	{
		dst.v[0] = src[0];
		dst.v[1] = src[1];
		dst.v[2] = src[2];
	}

	void operator >>(FbxAMatrix &src, O3DS::Matrix<double>& dst)
	{
		for (int u = 0; u < 4; u++)
			for (int v = 0; v < 4; v++)
				dst.m[u][v] = src[u][v];
	}

	O3DS::Matrixd rotationMatrix(fbxsdk::FbxEuler::EOrder order, FbxDouble3 rot)
	{
		double pi = 3.14159265358979323846;
		auto rx = O3DS::Matrixd::RotateX(rot[0] * pi / 180);
		auto ry = O3DS::Matrixd::RotateY(rot[1] * pi / 180);
		auto rz = O3DS::Matrixd::RotateZ(rot[2] * pi / 180);

		switch (order)
		{
		case FbxEuler::eOrderXYZ:  return  rx * ry * rz;
		case FbxEuler::eOrderXZY:  return  rx * rz * ry;
		case FbxEuler::eOrderYXZ:  return  ry * rx * rz;
		case FbxEuler::eOrderYZX:  return  ry * rz * rx;
		case FbxEuler::eOrderZXY:  return  rz * rx * ry;
		case FbxEuler::eOrderZYX:  return  rz * ry * rx;
		}

		return O3DS::Matrixd();
	}


	void FbTransform::update()
	{
		// Translation
		mNode->EvaluateLocalTranslation(mTime) >> this->translation.value;

		// Rotation
		//if(this->mName == "beta:Hips")
		//	printf("% .4f  % .4f   %.4f\n", rotation[0], rotation[1], rotation[2]);

		fbxsdk::FbxEuler::EOrder rotationOrder = mNode->RotationOrder;
		O3DS::Matrixd matRotation = rotationMatrix(rotationOrder, mNode->EvaluateLocalRotation(mTime));
		this->rotation.value = matRotation.GetQuaternion();

		// Scale
		mNode->EvaluateLocalScaling() >> this->scale.value;
	}

	FbTransform::FbTransform(fbxsdk::FbxNode *node, int parentId)
		: mNode(node)
		, O3DS::Transform(parentId)
	{

		// Update the transform with the data in the FbxNode (mNode)

		FbxTransform tform = mNode->GetTransform();

		this->mName = mNode->GetName();

		// http://help.autodesk.com/view/FBX/2020/ENU/?guid=FBX_Developer_Help_nodes_and_scene_graph_fbx_nodes_transformation_data_html
		// http://help.autodesk.com/view/FBX/2020/ENU/?guid=FBX_Developer_Help_nodes_and_scene_graph_fbx_nodes_computing_transformation_matrix_html

		enum FbxTransform::EInheritType inheritType = tform.GetInheritType();

		/*
		= ParentWorldTransform * T * [Roff * Rp * Rpre] * 
					R * [ Rpost-1 * Rp-1 * Soff * Sp ] * 
					S * [ Sp - 1 ]
		*/

		O3DS::Vector3d translation, rotation, scale,
			rotationOffset, rotationPivot, scaleOffset,
			scalePivot, preRotation, postRotation;

		fbxsdk::FbxEuler::EOrder rotationOrder = mNode->RotationOrder;
		(FbxDouble3)mNode->LclRotation    >> rotation;
		(FbxDouble3)mNode->RotationPivot  >> rotationPivot;
		(FbxDouble3)mNode->ScalingOffset  >> scaleOffset;
		(FbxDouble3)mNode->ScalingPivot   >> scalePivot;
		(FbxDouble3)mNode->RotationOffset >> rotationOffset;
		(FbxDouble3)mNode->PreRotation    >> preRotation;

		// Translation
		(FbxDouble3)(mNode->LclTranslation) >> this->translation.value;
		this->transformOrder.push_back(O3DS::ComponentType::TTranslation);

		// [Roff * Rp * Rpre]
		O3DS::Matrixd matRotOffset = O3DS::Matrixd::TranslateXYZ(rotationOffset);
		O3DS::Matrixd matRotPivot  = O3DS::Matrixd::TranslateXYZ(rotationPivot);
		O3DS::Matrixd matRotPre    = rotationMatrix(rotationOrder, mNode->PreRotation);
		O3DS::Matrixd m1 = matRotOffset * matRotPivot * matRotPre;

		//O3DS::Matrixd m1 = matRotPre * matRotPivot * matRotOffset;
		//O3DS::Matrixd m1 =  matRotPivot * matRotOffset * matRotPre;
		this->matrices.push_back(O3DS::TransformMatrix(m1));
		this->transformOrder.push_back(O3DS::ComponentType::TMatrix);

		// Rotation
		O3DS::Matrixd matRotation = rotationMatrix(rotationOrder, mNode->LclRotation);
		this->rotation.value = matRotation.GetQuaternion();
		this->transformOrder.push_back(O3DS::ComponentType::TRotation);

		//  [ Rpost-1 * Rp-1 * Soff * Sp ]
		O3DS::Matrixd matRotPost     = rotationMatrix(rotationOrder, mNode->PostRotation);
		O3DS::Matrixd matRotPivotInv = matRotPost.Inverse();
		O3DS::Matrixd matScaleOffset = O3DS::Matrixd::TranslateXYZ(scaleOffset);
		O3DS::Matrixd matScalePivot  = O3DS::Matrixd::TranslateXYZ(scalePivot);
		//O3DS::Matrixd m2 = matRotPost * matRotPivotInv * matScaleOffset * matScalePivot;
		O3DS::Matrixd m2 = matScalePivot * matScaleOffset * matRotPivotInv * matRotPost;

		this->matrices.push_back(O3DS::TransformMatrix(m2));
		this->transformOrder.push_back(O3DS::ComponentType::TMatrix);

		// Scale
		(FbxDouble3)mNode->LclScaling >> this->scale.value;
		this->transformOrder.push_back(O3DS::ComponentType::TScale);

		// [ Sp - 1 ]
		O3DS::Matrixd scalePivotInv = matScalePivot.Inverse();
		this->matrices.push_back(O3DS::TransformMatrix(scalePivotInv));
		this->transformOrder.push_back(O3DS::ComponentType::TMatrix);

		//O3DS::Matrix<double> oMatrix;
		//O3DS::Matrix<double> oMatrixOffset = O3DS::Matrix<double>::RotateX(1.5708);

		// Assign(oMatrix, mNode->EvaluateGlobalTransform(mTime));

		//O3DS::Vector<double> tt = oMatrix.GetTranslation();
		//O3DS::Vector<double> rr = oMatrix.GetQuaternion();

		/*
		for (int u = 0; u < 4; u++)
		{
			for(int v=0; v < 4; v++)
			{ 
				t->mMatrix(u, v) = oMatrix(u, v);
			}
		}*/
		
		/*
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

		*/
		//tt = oMatrix.GetTranslation();
		//rr = oMatrix.GetQuaternion();

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

	std::string FbTransform::info()
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

		subject->mTransforms.mItems.push_back(new FbTransform(node, parentId));
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
				FbTransform *t = (FbTransform*)transform;
				t->mTime = time;
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

		FindSubjects(root, subjects);

		return 0;
	}

	} // Fbx

} // O3DS

