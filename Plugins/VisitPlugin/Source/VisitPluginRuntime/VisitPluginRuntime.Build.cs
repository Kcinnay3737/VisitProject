using UnrealBuildTool;

public class VisitPluginRuntime : ModuleRules
{
	public VisitPluginRuntime(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicIncludePaths.AddRange(
			new string[] {

			}
			);
				
		
		PrivateIncludePaths.AddRange(
			new string[] {

			}
            );
			
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
                "Core",
				"CoreUObject",
				"Engine",
				"InputCore",
				"EnhancedInput",
				"UMG",
                "Paper2D",
                "Slate",
                "SlateCore",
            }
			);

        if (Target.bBuildEditor)
        {
            PublicDependencyModuleNames.AddRange(
				new string[] 
				{ 
					"UnrealEd" 
				}
				);
        }


        PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				
			}
			);
		
		
		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{

			}
			);
	}
}
