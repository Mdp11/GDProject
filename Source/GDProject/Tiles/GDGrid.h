// © 2021 Alessandro Arena, Mattia De Prisco, Nunzio Lopardo All Rights Reserved

#pragma once

#include "CoreMinimal.h"

#include "GameFramework/Actor.h"

#include "GDGrid.generated.h"

class AGDTile;
class AGDUnit;

UCLASS()
class GDPROJECT_API AGDGrid : public AActor
{
	GENERATED_BODY()

public:
	AGDGrid();

private:
	TArray<TArray<AGDTile*>> Tiles;

	TArray<AGDTile*> ReconstructPath(const TMap<AGDTile*, AGDTile*> CameFrom, AGDTile* Tile);

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	class USceneComponent* DummyRoot;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 Size;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float BlockSpacing;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<AGDTile> TileClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<AGDUnit> TileUnitClassDummy;

	virtual void BeginPlay() override;

public:
	TArray<AGDTile*> ComputePathBetweenTiles(AGDTile* StartTile, AGDTile* TargetTile);


	AGDTile* GetTile(const FIntPoint& Coordinates) const;

	int32 GetSize() const;
};
