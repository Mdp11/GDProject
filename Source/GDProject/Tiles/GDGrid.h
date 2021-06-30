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

	static TArray<AGDTile*> ReconstructPath(const TMap<AGDTile*, AGDTile*> CameFrom, AGDTile* Tile);

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	class USceneComponent* DummyRoot;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Size")
	int32 Width;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Size")
	int32 Height;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float BlockSpacing;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<AGDTile> BaseTileClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<AGDTile> ForestTileClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<AGDTile> RiverTileClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<AGDUnit> TileUnitClassDummy;

	bool bMapGenerated;

	virtual void BeginPlay() override;

	void BuildMap(const TArray<TArray<int>>& TileScheme);

	void CleanMap();

public:
	UPROPERTY(EditDefaultsOnly, Category="Material")
	class UMaterial* HoverDecalMaterial;

	UPROPERTY(EditDefaultsOnly, Category="Material")
	class UMaterial* SelectedDecalMaterial;

	UPROPERTY(EditDefaultsOnly, Category="Material")
	class UMaterial* TargetedEnemyDecalMaterial;

	UPROPERTY(EditDefaultsOnly, Category="Material")
	class UMaterial* ShortMovementDecalMaterial;

	UPROPERTY(EditDefaultsOnly, Category="Material")
	class UMaterial* LongMovementDecalMaterial;

	UPROPERTY(EditDefaultsOnly, Category="Material")
	class UMaterial* EnemyDecalMaterial;

	TArray<AGDTile*> ComputePathBetweenTiles(AGDTile* StartTile, AGDTile* TargetTile);

	TSet<AGDTile*> GetTilesAtDistance(AGDTile* StartTile, int Distance) const;

	AGDTile* GetTile(const FIntPoint& Coordinates) const;

	int32 GetSize() const;

	const TArray<TArray<AGDTile*>>& GetTilesGrid() const;
};
