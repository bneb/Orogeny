// Copyright Orogeny. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class OrogenyTarget : TargetRules
{
	public OrogenyTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.V4;
		IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_3;
		ExtraModuleNames.Add("Orogeny");
	}
}
