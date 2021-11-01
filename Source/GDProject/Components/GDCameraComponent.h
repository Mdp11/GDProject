// © 2021 Alessandro Arena, Mattia De Prisco, Nunzio Lopardo All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GDProject/Tiles/GDGrid.h"

#include "GDCameraComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class GDPROJECT_API UGDCameraComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UGDCameraComponent();

	UPROPERTY(EditAnywhere)
	ACameraActor* Camera;

	UPROPERTY(EditAnywhere)
	float CamerasOffset = 1;

	UPROPERTY(EditAnywhere)
	float CamerasHeight = 1030;

	UPROPERTY(EditAnywhere)
	AGDGrid* GridManager;
	//EndPositionY and EndPositionX are the position of the last tiles on the respective axes
	UPROPERTY(EditAnywhere)
	FVector EndPositionY;

	UPROPERTY(EditAnywhere)
	FVector EndPositionX;

	UPROPERTY(EditAnywhere)
	int32 GridSize;

	UPROPERTY()
	APlayerController* PlayerController;

	UPROPERTY()
	int ActualCameraIndex;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

	virtual void RotateCamera(int Direction);

	virtual void SetGridManager(AGDGrid* Gm);

	FRotator TargetRotation;
	FVector TargetLocation;
	
	// void SetCamerasPositions();
};
