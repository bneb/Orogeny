// Copyright Orogeny. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class OrogenyEditorTarget : TargetRules
{
	public OrogenyEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.V5;
		IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
		ExtraModuleNames.Add("Orogeny");
		bOverrideBuildEnvironment = true;
	}
}
