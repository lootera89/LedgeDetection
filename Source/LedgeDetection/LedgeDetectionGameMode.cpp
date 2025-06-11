// Copyright Epic Games, Inc. All Rights Reserved.

#include "LedgeDetectionGameMode.h"
#include "LedgeDetectionCharacter.h"
#include "UObject/ConstructorHelpers.h"

ALedgeDetectionGameMode::ALedgeDetectionGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
