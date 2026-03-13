// Copyright Orogeny. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class OrogenyTarget : TargetRules
{
	public OrogenyTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.V5;
		IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
		ExtraModuleNames.Add("Orogeny");
	}
}
