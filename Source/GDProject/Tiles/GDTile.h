// © 2021 Alessandro Arena, Mattia De Prisco, Nunzio Lopardo All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GDProject/Interfaces/GDTileElement.h"

#include "GDTile.generated.h"

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
	class UMaterial* BaseMaterial;

	UPROPERTY(EditDefaultsOnly, Category="Material")
	class UMaterial* HoverMaterial;

	UPROPERTY(EditDefaultsOnly, Category="Material")
	class UMaterial* SelectedMaterial;

	UPROPERTY()
	class AGDGrid* OwningGrid;

	bool bIsActive;

	UObject* TileElement;

public:
	void SetOwningGrid(AGDGrid* Grid);

	void HandleClicked();

	void Highlight(bool bOn) const;

	void SetTileElement(AActor* NewTileElement);
	
	UObject* GetTileElement () const;

	bool IsOccupied() const;
};
