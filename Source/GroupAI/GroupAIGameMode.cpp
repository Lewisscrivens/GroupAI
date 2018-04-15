// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "GroupAIGameMode.h"
#include "GroupAIHUD.h"
#include "GroupAICharacter.h"
#include "UObject/ConstructorHelpers.h"

AGroupAIGameMode::AGroupAIGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/Assets/Player/FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	// use our custom HUD class
	HUDClass = AGroupAIHUD::StaticClass();
}
