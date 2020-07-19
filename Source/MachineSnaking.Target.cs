// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class MachineSnakingTarget : TargetRules
{
	public MachineSnakingTarget( TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
//		DefaultBuildSettings = BuildSettingsVersion.V2;
		ExtraModuleNames.AddRange( new string[] { "MachineSnaking" } );
	}
}
