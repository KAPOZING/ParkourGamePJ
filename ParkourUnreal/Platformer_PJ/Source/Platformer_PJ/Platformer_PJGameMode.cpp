// Copyright Epic Games, Inc. All Rights Reserved.

#include "Platformer_PJGameMode.h"
#include "CharaBase.h"
#include "UObject/ConstructorHelpers.h"

APlatformer_PJGameMode::APlatformer_PJGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPersonCPP/Blueprints/ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
