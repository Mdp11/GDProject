// © 2021 Alessandro Arena, Mattia De Prisco, Nunzio Lopardo All Rights Reserved

#pragma once

#include "CoreMinimal.h"

#include "GDTileForest.h"
#include "GDTileLowland.h"
#include "GDTileRocks.h"
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
	TArray<TArray<int>> TileScheme;
	
	TArray<TArray<AGDTile*>> Tiles;

	static TArray<AGDTile*> ReconstructPath(const TMap<AGDTile*, AGDTile*> CameFrom, AGDTile* Tile);

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	class USceneComponent* DummyRoot;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Size")
	int32 Width;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Size")
	int32 Height;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Property")
	float Lowland_Percentage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Property")
	float Forest_Percentage;

	// UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Property")
	// float River_Percentage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Property")
	float Rock_Percentage;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float BlockSpacing;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<AGDTile> BaseTileClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<AGDTileLowland> LowlandTileClass;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<AGDTileForest> ForestTileClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<AGDTile> RiverTileClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<AGDTileRocks> RockTileClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<AGDUnit> TileUnitClassDummy;

	bool bMapGenerated;

	virtual void BeginPlay() override;

	void GenerateGrid();

	int32 ComputeTileType(TPair<int, int> Tile);

	TArray<int> SurroundCheck(TPair<int, int> Tile);
	
	void BuildMap();

	void CleanMap();

	void ShufflePositions(TArray<TPair<int, int>>& Undefined_Tiles);

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

	TArray<AGDTile*> ComputePathBetweenTiles(AGDTile* StartTile, AGDTile* TargetTile, AGDUnit* MovingUnit);

	TSet<AGDTile*> GetTilesAtDistance(AGDTile* StartTile, int Distance) const;

	AGDTile* GetTile(const FIntPoint& Coordinates) const;

	int32 GetSize() const;

	const TArray<TArray<AGDTile*>>& GetTilesGrid() const;
};
