// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"

#include "GDProjectGameModeBase.generated.h"

class AGDUnit;

UCLASS()
class GDPROJECT_API AGDProjectGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	AGDProjectGameModeBase();

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

public:
	void SetNextPlayerTurn();

	UFUNCTION(BlueprintCallable)
	void EndTurn();

	void OnTurnBegin();

	void OnTurnEnd();

	UFUNCTION(BlueprintCallable)
	void AssignUnitToPlayer(AGDUnit* Unit, int Player);

	void OnUnitDead(AGDUnit* Unit, int Player);

	void GameOver(int WinningPlayer);

	int GetCurrentPlayerTurn() const;
};
