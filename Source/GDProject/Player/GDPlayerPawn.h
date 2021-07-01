// © 2021 Alessandro Arena, Mattia De Prisco, Nunzio Lopardo All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "GDProject/Camera/GDCameraManager.h"
#include "GDProject/Tiles/GDGrid.h"

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
	UPROPERTY(EditAnywhere)
	AGDCameraManager* CameraManger;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UGDInventoryComponent* Inventory;

	UPROPERTY(EditAnywhere)
	AGDGrid* GridManger;

	UPROPERTY(EditInstanceOnly, BlueprintReadWrite)
	AGDTile* HoveringTile;

	UPROPERTY(BlueprintReadWrite)
	UObject* SelectedTileElement;

	TArray<AGDTile*> HighlightedPath;

	UPROPERTY(EditDefaultsOnly, Category="Widgets")
	TSubclassOf<UUserWidget> UnitActionsWidgetClass;

	UPROPERTY()
	UUserWidget* UnitActionsWidget;

	UPROPERTY(BlueprintReadOnly)
	TSet<UObject*> ActiveEntities;

	UPROPERTY()
	class UGDItemBase* SelectedItem;

	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void OnTurnBegin();

	void OnTurnEnd();

	void AddActiveEntity(UObject* Entity);

	void RemoveActiveEntity(UObject* Entity);

	UFUNCTION(BlueprintCallable)
	void DeselectTileElement(bool bReset = true);

	void OnUnitDead(AGDUnit* Unit, int OwningPlayer);

	UFUNCTION(BlueprintCallable)
	void OnItemSelected(class UGDItemBase* Item);

	void OnItemDeselected();

private:
	void HandleTilesHovering();

	AGDTile* GetTileUnderMouse() const;

	AGDTile* TraceForTile(const FVector& Start, const FVector& End, bool bDrawDebugHelpers) const;

	void UpdateHoveringTile(AGDTile* NewHoveringTile);

	void HighlightHoveringTile() const;

	void TriggerClick();

	void RotateCameraLeft();

	void RotateCameraRight();

	void SelectTileElement();

	void RequestUnitAction() const;

	int GetCurrentPlayerTurn() const;
};
