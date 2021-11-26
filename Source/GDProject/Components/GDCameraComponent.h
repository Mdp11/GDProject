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

	//This value indicate the distance between the camera and the grid
	UPROPERTY(EditAnywhere)
	float CamerasOffset = 530;

	UPROPERTY(EditAnywhere)
	float CamerasHeight = 750;

	UPROPERTY(EditAnywhere)
	float CameraFOV = 105.f;

	UPROPERTY(EditAnywhere)
	float CameraAngle = 40;
	
	UPROPERTY(EditAnywhere)
	AGDGrid* GridManager;
	//EndPositionY and EndPositionX are the position of the last tiles on the respective axes
	UPROPERTY(EditAnywhere)
	float EndPositionY;

	UPROPERTY(EditAnywhere)
	float EndPositionX;
	
	UPROPERTY(EditAnywhere)
	FVector Camera0Pos;

	UPROPERTY(EditAnywhere)
	FVector Camera1Pos;

	UPROPERTY(EditAnywhere)
	FVector Camera2Pos;

	UPROPERTY(EditAnywhere)
	FVector Camera3Pos;

	UPROPERTY(EditAnywhere)
	FRotator Camera0Rot;
	
	UPROPERTY(EditAnywhere)
	FRotator Camera1Rot;
	
	UPROPERTY(EditAnywhere)
	FRotator Camera2Rot;
	
	UPROPERTY(EditAnywhere)
	FRotator Camera3Rot;
	
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

	FRotator TargetRotation;
	
	FVector TargetLocation;
	
};
