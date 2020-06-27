// Copyright Epic Games, Inc. All Rights Reserved.

#include "Open3DStream.h"

#define LOCTEXT_NAMESPACE "FOpen3DStreamModule"

void FOpen3DStreamModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
}

void FOpen3DStreamModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FOpen3DStreamModule, Open3DStream)