// Copyright Epic Games, Inc. All Rights Reserved.

#include "Palworld_PortfolioGameMode.h"
#include "Palworld_PortfolioCharacter.h"
#include "UObject/ConstructorHelpers.h"

APalworld_PortfolioGameMode::APalworld_PortfolioGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/Player/Blueprint/BP_Player.BP_Player_C"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("BAd"));
	}
}
