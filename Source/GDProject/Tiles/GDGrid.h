// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "GDGrid.generated.h"

class AGDTile;

UCLASS()
class GDPROJECT_API AGDGrid : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGDGrid();


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Size")
		int32 Width;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Size")
		int32 Hight;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		float BlockSpacing;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
		class USceneComponent* DummyRoot;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		TSubclassOf<AGDTile> TileClass;

	UFUNCTION(CallInEditor)
		void BuildMap();	
	
	UFUNCTION(CallInEditor)
		void CleanMap();


	TArray<TArray<AGDTile*>> GetTilesGrid();



protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:

	TArray<TArray<AGDTile*>> TilesGrid;

	bool mapGenerated;

};
