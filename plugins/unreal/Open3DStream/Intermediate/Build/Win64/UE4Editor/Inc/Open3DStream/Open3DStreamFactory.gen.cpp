// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "Open3DStream/Public/Open3DStreamFactory.h"
#ifdef _MSC_VER
#pragma warning (push)
#pragma warning (disable : 4883)
#endif
PRAGMA_DISABLE_DEPRECATION_WARNINGS
void EmptyLinkFunctionForGeneratedCodeOpen3DStreamFactory() {}
// Cross Module References
	OPEN3DSTREAM_API UClass* Z_Construct_UClass_UOpen3DStreamFactory_NoRegister();
	OPEN3DSTREAM_API UClass* Z_Construct_UClass_UOpen3DStreamFactory();
	LIVELINKINTERFACE_API UClass* Z_Construct_UClass_ULiveLinkSourceFactory();
	UPackage* Z_Construct_UPackage__Script_Open3DStream();
// End Cross Module References
	void UOpen3DStreamFactory::StaticRegisterNativesUOpen3DStreamFactory()
	{
	}
	UClass* Z_Construct_UClass_UOpen3DStreamFactory_NoRegister()
	{
		return UOpen3DStreamFactory::StaticClass();
	}
	struct Z_Construct_UClass_UOpen3DStreamFactory_Statics
	{
		static UObject* (*const DependentSingletons[])();
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam Class_MetaDataParams[];
#endif
		static const FCppClassTypeInfoStatic StaticCppClassTypeInfo;
		static const UE4CodeGen_Private::FClassParams ClassParams;
	};
	UObject* (*const Z_Construct_UClass_UOpen3DStreamFactory_Statics::DependentSingletons[])() = {
		(UObject* (*)())Z_Construct_UClass_ULiveLinkSourceFactory,
		(UObject* (*)())Z_Construct_UPackage__Script_Open3DStream,
	};
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UOpen3DStreamFactory_Statics::Class_MetaDataParams[] = {
		{ "Comment", "/* \nhttps://docs.unrealengine.com/en-US/Engine/Animation/LiveLinkPlugin/LiveLinkPluginDevelopment/index.html\nThis is how Live Link knows about the sources it can use. The source factory must be a UObject and derive\nfrom ULiveLinkSourceFactory. The factory provides functions to create sources and create custom editor UI\nfor source creation. Source factories are automatically picked up by the client (there is no manual\nregistration process).\n*/" },
		{ "IncludePath", "Open3DStreamFactory.h" },
		{ "ModuleRelativePath", "Public/Open3DStreamFactory.h" },
		{ "ToolTip", "https://docs.unrealengine.com/en-US/Engine/Animation/LiveLinkPlugin/LiveLinkPluginDevelopment/index.html\nThis is how Live Link knows about the sources it can use. The source factory must be a UObject and derive\nfrom ULiveLinkSourceFactory. The factory provides functions to create sources and create custom editor UI\nfor source creation. Source factories are automatically picked up by the client (there is no manual\nregistration process)." },
	};
#endif
	const FCppClassTypeInfoStatic Z_Construct_UClass_UOpen3DStreamFactory_Statics::StaticCppClassTypeInfo = {
		TCppClassTypeTraits<UOpen3DStreamFactory>::IsAbstract,
	};
	const UE4CodeGen_Private::FClassParams Z_Construct_UClass_UOpen3DStreamFactory_Statics::ClassParams = {
		&UOpen3DStreamFactory::StaticClass,
		nullptr,
		&StaticCppClassTypeInfo,
		DependentSingletons,
		nullptr,
		nullptr,
		nullptr,
		UE_ARRAY_COUNT(DependentSingletons),
		0,
		0,
		0,
		0x001000A0u,
		METADATA_PARAMS(Z_Construct_UClass_UOpen3DStreamFactory_Statics::Class_MetaDataParams, UE_ARRAY_COUNT(Z_Construct_UClass_UOpen3DStreamFactory_Statics::Class_MetaDataParams))
	};
	UClass* Z_Construct_UClass_UOpen3DStreamFactory()
	{
		static UClass* OuterClass = nullptr;
		if (!OuterClass)
		{
			UE4CodeGen_Private::ConstructUClass(OuterClass, Z_Construct_UClass_UOpen3DStreamFactory_Statics::ClassParams);
		}
		return OuterClass;
	}
	IMPLEMENT_CLASS(UOpen3DStreamFactory, 4239487311);
	template<> OPEN3DSTREAM_API UClass* StaticClass<UOpen3DStreamFactory>()
	{
		return UOpen3DStreamFactory::StaticClass();
	}
	static FCompiledInDefer Z_CompiledInDefer_UClass_UOpen3DStreamFactory(Z_Construct_UClass_UOpen3DStreamFactory, &UOpen3DStreamFactory::StaticClass, TEXT("/Script/Open3DStream"), TEXT("UOpen3DStreamFactory"), false, nullptr, nullptr, nullptr);
	DEFINE_VTABLE_PTR_HELPER_CTOR(UOpen3DStreamFactory);
PRAGMA_ENABLE_DEPRECATION_WARNINGS
#ifdef _MSC_VER
#pragma warning (pop)
#endif
