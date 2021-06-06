// Copyright Epic Games, Inc. All Rights Reserved.


#include "GDProjectGameModeBase.h"

#include "Controllers/GDPlayerController.h"
#include "Player/GDPlayerPawn.h"

AGDProjectGameModeBase::AGDProjectGameModeBase()
{
	DefaultPawnClass = AGDPlayerPawn::StaticClass();
	PlayerControllerClass = AGDPlayerController::StaticClass();
}
