// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "Open3DStream/Public/Open3DStreamData.h"
#ifdef _MSC_VER
#pragma warning (push)
#pragma warning (disable : 4883)
#endif
PRAGMA_DISABLE_DEPRECATION_WARNINGS
void EmptyLinkFunctionForGeneratedCodeOpen3DStreamData() {}
// Cross Module References
	OPEN3DSTREAM_API UScriptStruct* Z_Construct_UScriptStruct_FOpen3DStreamData();
	UPackage* Z_Construct_UPackage__Script_Open3DStream();
// End Cross Module References
class UScriptStruct* FOpen3DStreamData::StaticStruct()
{
	static class UScriptStruct* Singleton = NULL;
	if (!Singleton)
	{
		extern OPEN3DSTREAM_API uint32 Get_Z_Construct_UScriptStruct_FOpen3DStreamData_Hash();
		Singleton = GetStaticStruct(Z_Construct_UScriptStruct_FOpen3DStreamData, Z_Construct_UPackage__Script_Open3DStream(), TEXT("Open3DStreamData"), sizeof(FOpen3DStreamData), Get_Z_Construct_UScriptStruct_FOpen3DStreamData_Hash());
	}
	return Singleton;
}
template<> OPEN3DSTREAM_API UScriptStruct* StaticStruct<FOpen3DStreamData>()
{
	return FOpen3DStreamData::StaticStruct();
}
static FCompiledInDeferStruct Z_CompiledInDeferStruct_UScriptStruct_FOpen3DStreamData(FOpen3DStreamData::StaticStruct, TEXT("/Script/Open3DStream"), TEXT("Open3DStreamData"), false, nullptr, nullptr);
static struct FScriptStruct_Open3DStream_StaticRegisterNativesFOpen3DStreamData
{
	FScriptStruct_Open3DStream_StaticRegisterNativesFOpen3DStreamData()
	{
		UScriptStruct::DeferCppStructOps(FName(TEXT("Open3DStreamData")),new UScriptStruct::TCppStructOps<FOpen3DStreamData>);
	}
} ScriptStruct_Open3DStream_StaticRegisterNativesFOpen3DStreamData;
	struct Z_Construct_UScriptStruct_FOpen3DStreamData_Statics
	{
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam Struct_MetaDataParams[];
#endif
		static void* NewStructOps();
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_TimeOffset_MetaData[];
#endif
		static const UE4CodeGen_Private::FDoublePropertyParams NewProp_TimeOffset;
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_Port_MetaData[];
#endif
		static const UE4CodeGen_Private::FUnsizedIntPropertyParams NewProp_Port;
		static const UE4CodeGen_Private::FPropertyParamsBase* const PropPointers[];
		static const UE4CodeGen_Private::FStructParams ReturnStructParams;
	};
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UScriptStruct_FOpen3DStreamData_Statics::Struct_MetaDataParams[] = {
		{ "BlueprintType", "true" },
		{ "Comment", "// E:\\Unreal\\UE_4.25\\Engine\\Plugins\\Animation\\LiveLink\\Source\\LiveLink\\Public\n//    LiveLinkMessageBusFinder.h\n" },
		{ "ModuleRelativePath", "Public/Open3DStreamData.h" },
		{ "ToolTip", "E:\\Unreal\\UE_4.25\\Engine\\Plugins\\Animation\\LiveLink\\Source\\LiveLink\\Public\n   LiveLinkMessageBusFinder.h" },
	};
#endif
	void* Z_Construct_UScriptStruct_FOpen3DStreamData_Statics::NewStructOps()
	{
		return (UScriptStruct::ICppStructOps*)new UScriptStruct::TCppStructOps<FOpen3DStreamData>();
	}
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UScriptStruct_FOpen3DStreamData_Statics::NewProp_TimeOffset_MetaData[] = {
		{ "ModuleRelativePath", "Public/Open3DStreamData.h" },
	};
#endif
	const UE4CodeGen_Private::FDoublePropertyParams Z_Construct_UScriptStruct_FOpen3DStreamData_Statics::NewProp_TimeOffset = { "TimeOffset", nullptr, (EPropertyFlags)0x0010000000000000, UE4CodeGen_Private::EPropertyGenFlags::Double, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(FOpen3DStreamData, TimeOffset), METADATA_PARAMS(Z_Construct_UScriptStruct_FOpen3DStreamData_Statics::NewProp_TimeOffset_MetaData, UE_ARRAY_COUNT(Z_Construct_UScriptStruct_FOpen3DStreamData_Statics::NewProp_TimeOffset_MetaData)) };
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UScriptStruct_FOpen3DStreamData_Statics::NewProp_Port_MetaData[] = {
		{ "Category", "Open3DStream" },
		{ "ModuleRelativePath", "Public/Open3DStreamData.h" },
	};
#endif
	const UE4CodeGen_Private::FUnsizedIntPropertyParams Z_Construct_UScriptStruct_FOpen3DStreamData_Statics::NewProp_Port = { "Port", nullptr, (EPropertyFlags)0x0010000000000015, UE4CodeGen_Private::EPropertyGenFlags::Int, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(FOpen3DStreamData, Port), METADATA_PARAMS(Z_Construct_UScriptStruct_FOpen3DStreamData_Statics::NewProp_Port_MetaData, UE_ARRAY_COUNT(Z_Construct_UScriptStruct_FOpen3DStreamData_Statics::NewProp_Port_MetaData)) };
	const UE4CodeGen_Private::FPropertyParamsBase* const Z_Construct_UScriptStruct_FOpen3DStreamData_Statics::PropPointers[] = {
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FOpen3DStreamData_Statics::NewProp_TimeOffset,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FOpen3DStreamData_Statics::NewProp_Port,
	};
	const UE4CodeGen_Private::FStructParams Z_Construct_UScriptStruct_FOpen3DStreamData_Statics::ReturnStructParams = {
		(UObject* (*)())Z_Construct_UPackage__Script_Open3DStream,
		nullptr,
		&NewStructOps,
		"Open3DStreamData",
		sizeof(FOpen3DStreamData),
		alignof(FOpen3DStreamData),
		Z_Construct_UScriptStruct_FOpen3DStreamData_Statics::PropPointers,
		UE_ARRAY_COUNT(Z_Construct_UScriptStruct_FOpen3DStreamData_Statics::PropPointers),
		RF_Public|RF_Transient|RF_MarkAsNative,
		EStructFlags(0x00000201),
		METADATA_PARAMS(Z_Construct_UScriptStruct_FOpen3DStreamData_Statics::Struct_MetaDataParams, UE_ARRAY_COUNT(Z_Construct_UScriptStruct_FOpen3DStreamData_Statics::Struct_MetaDataParams))
	};
	UScriptStruct* Z_Construct_UScriptStruct_FOpen3DStreamData()
	{
#if WITH_HOT_RELOAD
		extern uint32 Get_Z_Construct_UScriptStruct_FOpen3DStreamData_Hash();
		UPackage* Outer = Z_Construct_UPackage__Script_Open3DStream();
		static UScriptStruct* ReturnStruct = FindExistingStructIfHotReloadOrDynamic(Outer, TEXT("Open3DStreamData"), sizeof(FOpen3DStreamData), Get_Z_Construct_UScriptStruct_FOpen3DStreamData_Hash(), false);
#else
		static UScriptStruct* ReturnStruct = nullptr;
#endif
		if (!ReturnStruct)
		{
			UE4CodeGen_Private::ConstructUScriptStruct(ReturnStruct, Z_Construct_UScriptStruct_FOpen3DStreamData_Statics::ReturnStructParams);
		}
		return ReturnStruct;
	}
	uint32 Get_Z_Construct_UScriptStruct_FOpen3DStreamData_Hash() { return 1249410499U; }
PRAGMA_ENABLE_DEPRECATION_WARNINGS
#ifdef _MSC_VER
#pragma warning (pop)
#endif
