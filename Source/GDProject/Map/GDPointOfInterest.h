// © 2021 Alessandro Arena, Mattia De Prisco, Nunzio Lopardo All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GDPointOfInterest.generated.h"

UCLASS()
class GDPROJECT_API AGDPointOfInterest : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGDPointOfInterest();

	TArray<AGDPointOfInterest*> AdjList;

	int Level;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Difficulty")
	int Difficulty;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	bool AddNode(AGDPointOfInterest* NewNode);
};
