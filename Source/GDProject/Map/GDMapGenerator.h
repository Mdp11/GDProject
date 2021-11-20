// © 2021 Alessandro Arena, Mattia De Prisco, Nunzio Lopardo All Rights Reserved

#pragma once

#include "CoreMinimal.h"

#include "GDPointOfInterest.h"
#include "GameFramework/Actor.h"
#include "GDMapGenerator.generated.h"

UCLASS()
class GDPROJECT_API AGDMapGenerator : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGDMapGenerator();

	TArray<TArray<int>> MapScheme;
	
	TArray<TArray<int>> Map;

	// TArray<TArray<AGDPointOfInterest*>> Map;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Size")
	int32 Levels;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Size")
	int32 MaxNodeLevel;
	
    UPROPERTY(EditAnywhere)
    int32 NodeHorizontalDistance = 4000;

	UPROPERTY(EditAnywhere)
	int32 NodeVerticallDistance = 2300;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<AGDPointOfInterest> BasePOI;


	struct PoINode
	{
		AGDPointOfInterest* PoI;
		FVector PoILocation;
		int Level;
		PoINode(AGDPointOfInterest* node, FVector loc, int lvl) : PoI(node), PoILocation(loc), Level(lvl){}
	};

	TArray<PoINode*> PoIList;
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void GenerateMapScheme();
	
	virtual int SetPOIType();
	
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
