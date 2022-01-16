// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.IO;

public class Open3DStream : ModuleRules
{
	public Open3DStream(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicIncludePaths.AddRange( new string[] {} );
		
		string LibDir = Path.GetFullPath(Path.Combine(ModuleDirectory, "../../lib/"));
	
		PrivateIncludePaths.AddRange( new string[] 
        { LibDir + "include" } );
		
		PublicDependencyModuleNames.AddRange( new string[] { "Core" } );

        PublicAdditionalLibraries.Add(LibDir + "nng.lib");
        PublicAdditionalLibraries.Add(LibDir + "flatbuffers.lib");
        PublicAdditionalLibraries.Add(LibDir + "open3dstreamstatic.lib");

        PublicDefinitions.Add("NNG_STATIC_LIB");

        PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore",
                "LiveLinkInterface",
                "Networking",
                "Sockets",
                "InputCore",
                "Networking",
                "Sockets",
				// ... add private dependencies that you statically link with here ...	
			}
			);
				
		DynamicallyLoadedModuleNames.AddRange( new string[] {} );
	}
}
