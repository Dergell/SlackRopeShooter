// Copyright Epic Games, Inc. All Rights Reserved.

#include "SlackRopeShooterGameMode.h"
#include "SlackRopeShooterCharacter.h"
#include "UObject/ConstructorHelpers.h"

ASlackRopeShooterGameMode::ASlackRopeShooterGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPerson/Blueprints/BP_FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

}
