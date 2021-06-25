// © 2021 Alessandro Arena, Mattia De Prisco, Nunzio Lopardo All Rights Reserved

#pragma once

#include "CoreMinimal.h"

#include "Chaos/AABB.h"
#include "Chaos/AABB.h"
#include "GameFramework/Actor.h"

#include "GDTile.generated.h"

class AGDGrid;
class AGDUnit;
enum class EDirection : uint8;

enum class EHighlightInfo
{
	Default,
	Enemy,
	Ally
};

UCLASS()
class GDPROJECT_API AGDTile : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AGDTile();


protected:
	// Called when the game starts or when spawned

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class USceneComponent* DummyRoot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UStaticMeshComponent* TileMesh;

	UPROPERTY(EditDefaultsOnly, Category="Material")
	UDecalComponent* SelectionDecalComponent;

	UPROPERTY(EditDefaultsOnly, Category="Material")
	UDecalComponent* InfoDecalComponent;

	UPROPERTY()
	AGDGrid* OwningGrid;

	TSet<AGDUnit*> GuardingUnits;

	bool bIsActive;

	UPROPERTY(EditAnywhere)
	bool bIsTraversable;

	UPROPERTY(EditAnywhere)
	bool bObstructVisual;

	UObject* TileElement;

	FIntPoint Coordinates;

	//Tile Modifiers
	UPROPERTY(EditDefaultsOnly, Category = "Fight Modifiers")
	float AttackModifier;

	UPROPERTY(EditDefaultsOnly, Category = "Fight Modifiers")
	float DefenceModifier;

	UPROPERTY(EditDefaultsOnly, Category = "Fight Modifiers")
	float HitChanceModifier;

	UPROPERTY(EditDefaultsOnly, Category = "Fight Modifiers")
	float CriticalModifier;

	UPROPERTY(EditDefaultsOnly, Category = "Fight Modifiers")
	float CriticalChanceModifier;


public:
	void SetOwningGrid(AGDGrid* Grid);

	void Select();

	void Deselect();

	void Highlight(const EHighlightInfo& HighlightInfo) const;

	void RemoveHighlight() const;

	void SetTileElement(UObject* NewTileElement);

	UFUNCTION(CallInEditor, Category = "Tile Type")
	void ChangeInForest();

	UFUNCTION(CallInEditor, Category = "Tile Type")
	void ChangeInRiver();

	void SetCoordinates(const FIntPoint& NewCoordinates);

	UFUNCTION(BlueprintCallable)
	UObject* GetTileElement() const;

	bool IsOccupied() const;

	bool IsOccupiedByEnemy(AGDUnit* OtherUnit) const;

	bool IsTraversable() const;

	UFUNCTION(BlueprintCallable)
	FIntPoint GetCoordinates() const;

	int GetDistanceFrom(AGDTile* TargetTile) const;

	TArray<AGDTile*> GetTraversableNeighbours() const;

	UFUNCTION(BlueprintCallable)
	AGDGrid* GetGrid() const;

	void ApplyMovementRangeInfoDecal(bool bShort) const;

	void ApplyEnemyInfoDecal() const;

	void RemoveInfoDecal() const;

	bool IsPathClearTowardsTile(AGDTile* Tile) const;

	TArray<AGDTile*> GetTilesInDirection(const EDirection Direction, int Num) const;

	float GetAttackModifier() const
	{
		return AttackModifier;
	}

	float GetDefenceModifier() const
	{
		return DefenceModifier;
	}

	float GetHitChanceModifier() const
	{
		return HitChanceModifier;
	}

	float GetCriticalModifier() const
	{
		return CriticalModifier;
	}

	float GetCriticalChanceModifier() const
	{
		return CriticalChanceModifier;
	}

	TSet<AGDUnit*> GetGuardingUnits() const;

	void AddGuardingUnit(AGDUnit* Unit);

	void RemoveGuardingUnit(AGDUnit* Unit);

	bool HasGuardingUnits() const;
};
