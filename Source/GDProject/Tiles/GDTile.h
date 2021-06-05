// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GDGrid.h"

#include "GDTile.generated.h"

class AGDTilesManager;

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

	UPROPERTY(EditDefaultsOnly, Category = "Material")
		class UMaterial* BaseMaterial;

	UPROPERTY(EditDefaultsOnly, Category = "Material")
		class UMaterial* HoverMaterial;

	UPROPERTY(EditDefaultsOnly, Category = "Material")
		class UMaterial* SelectedMaterial;


	UPROPERTY()
		AGDGrid* OwningGrid;

	bool bIsActive;

	UPROPERTY(EditAnywhere)
		bool bIsTraversable;

	UObject* TileElement;

	FIntPoint Coordinates;

	//Tile Modifiers

	UPROPERTY(EditDefaultsOnly, Category = "Fight Modifiers")
		float AttackModifier;

	UPROPERTY(EditDefaultsOnly, Category = "Fight Modifiers")
		float DefenceModifier;

	UPROPERTY(EditDefaultsOnly, Category = "Fight Modifiers")
		float CritcalModifier;


public:	
	
	void SetOwningGrid(AGDTilesManager* Grid);

	void HandleClicked();

	void Highlight(bool bOn) const;

	void AddElement(AActor* NewTileElement);

	UFUNCTION(CallInEditor, Category = "Tile Type")
		void ChangeInForest();

	UFUNCTION(CallInEditor, Category = "Tile Type")
		void ChangeInRiver();

	void SetOwningGrid(AGDGrid* Grid);

	void SetCoordinates(const FIntPoint& NewCoordinates);

};
