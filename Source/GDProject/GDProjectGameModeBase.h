// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Units/GDUnit.h"

#include "GDProjectGameModeBase.generated.h"

UCLASS()
class GDPROJECT_API AGDProjectGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	AGDProjectGameModeBase();
	
	void SetNextPlayerTurn();

	UFUNCTION(BlueprintCallable)
	void EndTurn();

	void OnTurnBegin();

	void OnTurnEnd();

	UFUNCTION(BlueprintCallable)
	void AssignUnitToPlayer(AGDUnit* Unit, int Player);

	void OnUnitDead(AGDUnit* Unit, int Player);

	void GameOver(int WinningPlayer);

protected:
	TMap<int, TSet<AGDUnit*>> PlayersUnits;
	
	TMap<int, bool> Players;

	UPROPERTY(BlueprintReadOnly)
	int CurrentPlayerTurn;

	int NumPlayers;

	UFUNCTION(BlueprintImplementableEvent)
	void OnGameOver(int WinningPlayer);

	void SetupGame();
	
	virtual void BeginPlay() override;
};
