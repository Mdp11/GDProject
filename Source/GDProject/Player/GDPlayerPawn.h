// © 2021 Alessandro Arena, Mattia De Prisco, Nunzio Lopardo All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "GDProject/Interfaces/GDTileElement.h"

#include "GDPlayerPawn.generated.h"

class AGDTile;
class AGDUnit;

UCLASS()
class GDPROJECT_API AGDPlayerPawn : public APawn
{
	GENERATED_BODY()

public:
	AGDPlayerPawn();

protected:
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite)
	AGDTile* HoveringTile;

	UPROPERTY(BlueprintReadWrite)
	UObject* SelectedTileElement;

	TArray<AGDTile*> HighlightedPath;

	UPROPERTY(EditDefaultsOnly, Category="Widgets")
	TSubclassOf<UUserWidget> UnitActionsWidgetClass;

	UPROPERTY()
	UUserWidget* UnitActionsWidget;

	UPROPERTY()
	TSet<AGDUnit*> ActiveUnits;

	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void OnTurnBegin();

	void OnTurnEnd();

	void AddActiveUnit(AGDUnit* Unit);

	void RemoveActiveUnit(AGDUnit* Unit);

private:

	void HandleTilesHovering();

	AGDTile* GetTileUnderMouse() const;

	AGDTile* TraceForTile(const FVector& Start, const FVector& End, bool bDrawDebugHelpers) const;

	void UpdateHoveringTile(AGDTile* NewHoveringTile);

	void HighlightHoveringTile() const;

	void TriggerClick();

	void SelectTileElement();

	UFUNCTION(BlueprintCallable)
	void DeselectTileElement();

	void RequestUnitAction() const;
	
	int GetCurrentPlayerTurn() const;
};
