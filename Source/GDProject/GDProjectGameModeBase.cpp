// Copyright Epic Games, Inc. All Rights Reserved.


#include "GDProjectGameModeBase.h"

#include "Controllers/GDPlayerController.h"
#include "Player/GDPlayerPawn.h"
#include "GDProject/Units/GDUnit.h"
#include "AI/GDAIControllerBase.h"

AGDProjectGameModeBase::AGDProjectGameModeBase()
{
	DefaultPawnClass = AGDPlayerPawn::StaticClass();
	PlayerControllerClass = AGDPlayerController::StaticClass();

	PrimaryActorTick.bStartWithTickEnabled = true;
	PrimaryActorTick.bCanEverTick = true;
}

void AGDProjectGameModeBase::SetNextPlayerTurn()
{
	do
	{
		if (++CurrentPlayerTurn >= NumPlayers)
		{
			CurrentPlayerTurn = 0;
		}
	}
	while (!Players.Contains(CurrentPlayerTurn) || Players[CurrentPlayerTurn] == false);
}

void AGDProjectGameModeBase::EndTurn()
{
	OnTurnEnd();

	SetNextPlayerTurn();

	OnTurnBegin();
}

void AGDProjectGameModeBase::OnTurnBegin()
{
	for (const auto& Unit : PlayersUnits[CurrentPlayerTurn])
	{
		Unit->OnTurnBegin();
	}

	if (CurrentPlayerTurn != 0)
	{
		AIUnits = PlayersUnits[CurrentPlayerTurn];
	}
}

void AGDProjectGameModeBase::OnTurnEnd()
{
	if (AGDPlayerPawn* PlayerPawn = Cast<AGDPlayerPawn>(GetWorld()->GetFirstPlayerController()->GetPawn()))
	{
		PlayerPawn->OnTurnEnd();
	}

	for (const auto& Unit : PlayersUnits[CurrentPlayerTurn])
	{
		Unit->OnTurnEnd();
	}
}

void AGDProjectGameModeBase::AssignUnitToPlayer(AGDUnit* Unit, const int Player)
{
	PlayersUnits[Player].Add(Unit);
}

void AGDProjectGameModeBase::OnUnitDead(const AGDUnit* Unit, const int Player)
{
	PlayersUnits[Player].Remove(Unit);

	if (PlayersUnits[Player].Num() == 0)
	{
		Players.Remove(Player);
		if (Players.Num() == 1)
		{
			GameOver(Players.begin().Key());
		}
	}
}

void AGDProjectGameModeBase::GameOver(int WinningPlayer)
{
	if (APawn* PlayerPawn = GetWorld()->GetFirstPlayerController()->GetPawn())
	{
		PlayerPawn->DisableInput(nullptr);
	}
	OnGameOver(WinningPlayer);
}

int AGDProjectGameModeBase::GetCurrentPlayerTurn() const
{
	return CurrentPlayerTurn;
}

void AGDProjectGameModeBase::SetupGame()
{
	CurrentPlayerTurn = 0;
	NumPlayers = 2;

	for (int i = 0; i < NumPlayers; ++i)
	{
		Players.Add(i, true);
		PlayersUnits.Add(i, TSet<AGDUnit*>{});
	}
}

void AGDProjectGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	SetupGame();
}

void AGDProjectGameModeBase::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (AIUnits.Num() > 0 && !Cast<AGDPlayerPawn>(GetWorld()->GetFirstPlayerController()->GetPawn())->
		IsAnyEntityActive())
	{
		AGDAIControllerBase* AIController = Cast<AGDAIControllerBase>((*AIUnits.begin())->GetController());
		AIController->Play();
	
		AIUnits.Remove(*AIUnits.begin());
	
		if (AIUnits.Num() == 0)
		{
			EndTurn();
		}
	}
}
