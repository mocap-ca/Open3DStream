#include <fbxsdk.h>
//#include <fbxfilesdk/fbxio/fbxiosettings.h>

// https://help.autodesk.com/view/FBX/2017/ENU/?guid=__files_GUID_ABFC58F1_C10D_42AE_A204_D79EB1D84B24_htm
// https://help.autodesk.com/cloudhelp/2017/ENU/FBX-Developer-Help/cpp_ref/_import_scene_2_display_animation_8cxx-example.html

using namespace fbxsdk;

#include "windows.h"

void ListChildren(fbxsdk::FbxNode *node)
{
	FbxDouble3 trans = node->LclTranslation.EvaluateValue(FbxTime(0));
	FbxDouble3 rot = node->LclRotation.EvaluateValue(FbxTime(0));

	printf("% 20s  % qws    %- 8.1f %- 8.1f %- 8.1f  %- 8.1f %- 8.1f %- 8.1f\n",
		node->GetName(), node->GetTypeName(),
		trans[0], trans[1], trans[2], rot[0], rot[1], rot[2]);

	for (int i = 0; i < node->GetChildCount(); i++)
	{
		fbxsdk::FbxNode *child = node->GetChild(i);
		ListChildren(child);
	}

}

int main(int argc, char *argv[])
{
	FbxManager* manager = FbxManager::Create();
    FbxScene* lScene = NULL;
	
	const char *file_path = "c:\\users\\al\\Downloads\\shadow_run_jump.fbx";

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

	fbxsdk::FbxAnimStack *stack = lScene->GetCurrentAnimationStack();
	printf("Stack: %s\n", stack->GetName());

	fbxsdk::FbxNode *root = lScene->GetRootNode();
	printf("Root: %s\n", root->GetName());

	FbxTime t1 = stack->LocalStart;
	FbxTime t2 = stack->LocalStop;
	
	FbxTime tInc = FbxTime();
	tInc.SetFrame(1);

	printf("Start: %s\n", t1.GetTimeString());
	printf("End:   %s\n", t2.GetTimeString());
	printf("Rate:  %f\n", frameRate);

	LARGE_INTEGER freq;
	LARGE_INTEGER count, zero;
	QueryPerformanceFrequency(&freq);
	QueryPerformanceCounter(&zero);

	double zerof = (double)zero.QuadPart * 1000. / freq.QuadPart;

	for (FbxTime t = t1.GetFramedTime(); t < t2.GetFramedTime(); t = t + tInc)
	{
		QueryPerformanceCounter(&count);
		double tick = ((double)count.QuadPart * 1000. / freq.QuadPart) - zerof;
		double delay = t.GetSecondDouble() * 1000 - tick;
		printf("%f   %s   %f\n", tick, t.GetTimeString(), delay);

		if (delay > 0)
			Sleep(delay);
	}

	ListChildren(root);

	return 0;

}

