// Copyright Epic Games, Inc. All Rights Reserved.


#include "GDProjectGameModeBase.h"

#include "Controllers/GDPlayerController.h"
#include "Player/GDPlayerPawn.h"

AGDProjectGameModeBase::AGDProjectGameModeBase()
{
	DefaultPawnClass = AGDPlayerPawn::StaticClass();
	PlayerControllerClass = AGDPlayerController::StaticClass();
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
	for (auto& Unit : PlayersUnits[CurrentPlayerTurn])
	{
		Unit->OnTurnBegin();
	}
}

void AGDProjectGameModeBase::OnTurnEnd()
{
	if (AGDPlayerPawn* PlayerPawn = Cast<AGDPlayerPawn>(GetWorld()->GetFirstPlayerController()->GetPawn()))
	{
		PlayerPawn->OnTurnEnd();
	}

	for (auto& Unit : PlayersUnits[CurrentPlayerTurn])
	{
		Unit->OnTurnEnd();
	}
}

void AGDProjectGameModeBase::AssignUnitToPlayer(AGDUnit* Unit, const int Player)
{
	PlayersUnits[Player].Add(Unit);
}

void AGDProjectGameModeBase::OnUnitDead(AGDUnit* Unit, int Player)
{
	PlayersUnits[Player].Remove(Unit);

	if (PlayersUnits[Player].Num() == 0)
	{
		Players.Remove(Player);
		if (Players.Num() == 1)
		{
			GameOver(Players[0]);
		}
	}
}

void AGDProjectGameModeBase::GameOver(int WinningPlayer)
{
	if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
	{
		PC->DisableInput(nullptr);
	}

	OnGameOver(WinningPlayer);
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
	SetupGame();
}
