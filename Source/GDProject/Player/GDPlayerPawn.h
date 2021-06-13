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

	UPROPERTY()
	AGDUnit* SelectedUnit;

	TArray<AGDTile*> HighlightedPath;

	UPROPERTY(BlueprintReadWrite)
	bool bWaitingForActionCompletion;

	UPROPERTY(EditDefaultsOnly, Category="Widgets")
	TSubclassOf<UUserWidget> UnitActionsWidgetClass;

	UPROPERTY()
	UUserWidget* UnitActionsWidget;

	void SelectTile(AGDTile* TargetTile = nullptr);

	UFUNCTION(BlueprintCallable)
	void DeselectTile();

	void TriggerClick();

	AGDTile* TraceForTile(const FVector& Start, const FVector& End, bool bDrawDebugHelpers) const;

	void UpdateHoveringTile(AGDTile* NewHoveringTile);

	void RequestUnitAction();

	AGDTile* GetTileUnderMouse() const;

	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void ActionFinished(AGDTile* NewCurrentTile);

	void OnTurnBegin();

	void OnTurnEnd();
};
