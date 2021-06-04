// © 2021 Alessandro Arena, Mattia De Prisco, Nunzio Lopardo All Rights Reserved

#pragma once

#include "CoreMinimal.h"

#include "GameFramework/Actor.h"

#include "GDTile.generated.h"

class AGDGrid;

UCLASS()
class GDPROJECT_API AGDTile : public AActor
{
	GENERATED_BODY()

public:
	AGDTile();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	class USceneComponent* DummyRoot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	class UStaticMeshComponent* TileMesh;

	UPROPERTY(EditDefaultsOnly, Category="Material")
	UDecalComponent* DecalComponent;

	UPROPERTY(EditDefaultsOnly, Category="Material")
	class UMaterial* HoverMaterial;

	UPROPERTY(EditDefaultsOnly, Category="Material")
	class UMaterial* SelectedMaterial;

	UPROPERTY(EditDefaultsOnly, Category="Material")
	class UMaterial* EnemyMaterial;

	UPROPERTY()
	AGDGrid* OwningGrid;

	bool bIsActive;

	UPROPERTY(EditAnywhere)
	bool bIsTraversable;

	UObject* TileElement;

	FIntPoint Coordinates;

public:
	void SetOwningGrid(AGDGrid* Grid);

	void HandleClicked();

	void Highlight(bool bOn) const;

	void HighlightDanger(bool bOn) const;

	void SetTileElement(UObject* NewTileElement);

	UObject* GetTileElement() const;

	bool IsOccupied() const;

	bool IsTraversable() const;

	void SetCoordinates(const FIntPoint& NewCoordinates);

	FIntPoint GetCoordinates() const;

	int GetDistanceFrom(AGDTile* TargetTile) const;

	TArray<AGDTile*> GetNeighbours(bool bOnlyTraversable = true) const;

	AGDGrid* GetGrid() const;
};
