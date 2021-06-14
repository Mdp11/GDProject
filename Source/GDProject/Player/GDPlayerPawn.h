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

	void HandleTilesHovering();

protected:
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite)
	AGDTile* HoveringTile;

	UPROPERTY(EditInstanceOnly, BlueprintReadWrite)
	AGDTile* SelectedTile;

	UPROPERTY()
	AGDUnit* SelectedUnit;

	TArray<AGDTile*> HighlightedPath;

	UPROPERTY(EditDefaultsOnly, Category="Widgets")
	TSubclassOf<UUserWidget> UnitActionsWidgetClass;

	UPROPERTY()
	UUserWidget* UnitActionsWidget;

	UPROPERTY()
	TSet<AGDUnit*> ActiveUnits;

	void SelectTile(AGDTile* TargetTile = nullptr);

	UFUNCTION(BlueprintCallable)
	void DeselectTile();

	void TriggerClick();

	AGDTile* TraceForTile(const FVector& Start, const FVector& End, bool bDrawDebugHelpers) const;

	void UpdateHoveringTile(AGDTile* NewHoveringTile);

	void RequestUnitAction() const;

	AGDTile* GetTileUnderMouse() const;

	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void OnTurnBegin();

	void OnTurnEnd();

	void AddActiveUnit(AGDUnit* Unit);

	void RemoveActiveUnit(AGDUnit* Unit);
};
