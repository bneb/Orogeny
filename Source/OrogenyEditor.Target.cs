// Copyright Orogeny. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class OrogenyEditorTarget : TargetRules
{
	public OrogenyEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.V4;
		IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_3;
		ExtraModuleNames.Add("Orogeny");
	}
}
