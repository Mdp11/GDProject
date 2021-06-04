// © 2021 Alessandro Arena, Mattia De Prisco, Nunzio Lopardo All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "GDProject/Interfaces/GDTileElement.h"
#include "GDProject/Units/GDUnit.h"

#include "GDPlayerPawn.generated.h"

class AGDTile;

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

	TArray<AGDTile*> HighlightedPath;

	TSet<AGDTile*> HighlightedTilesInMovementRange;

	void SelectTile();

	void UnselectTile();

	void TriggerClick();

	AGDTile* TraceForTile(const FVector& Start, const FVector& End, bool bDrawDebugHelpers) const;

	void FillHighlightedPath(const TArray<AGDTile*>& Path);

	void EmptyHighlightedPath();

	void EmptyTilesInMovementRange();

	void DisableHoveringVisuals();

	void UpdateHoveringTile(AGDTile* TargetTile);

	void UpdateUnitActionsHovering(AGDUnit* Unit);

	void HighlightUnitMovementRange(AGDUnit* Unit);

public:
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void RequestMove(AGDUnit* Unit) const;

	void RequestAttack(AGDUnit* AttackingUnit, AGDUnit* EnemyUnit) const;

	void RequestUnitAction(AGDUnit* Unit) const;
};
