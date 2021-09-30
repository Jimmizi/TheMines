// Copyright Epic Games, Inc. All Rights Reserved.

#include "LD49GameMode.h"

#include "GameService.h"
#include "LD49HUD.h"
#include "LD49Character.h"
#include "UObject/ConstructorHelpers.h"

ALD49GameMode::ALD49GameMode()
	: Super()
{
	GameService::ResetAll();
	
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPersonCPP/Blueprints/FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	// use our custom HUD class
	HUDClass = ALD49HUD::StaticClass();
}
