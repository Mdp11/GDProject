// © 2021 Alessandro Arena, Mattia De Prisco, Nunzio Lopardo All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GDProject/Tiles/GDGrid.h"

#include "GDCameraManager.generated.h"

UCLASS()
class GDPROJECT_API AGDCameraManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGDCameraManager();

	UPROPERTY(EditAnywhere)
	TArray<ACameraActor*> Cameras;
	
	UPROPERTY(EditAnywhere)
	AGDGrid* GridManager;
	
	UPROPERTY(EditAnywhere)
	ACameraActor* Camera0;

	UPROPERTY(EditAnywhere)
	ACameraActor* Camera1;

	UPROPERTY(EditAnywhere)
	ACameraActor* Camera2;

	UPROPERTY(EditAnywhere)
	ACameraActor* Camera3;

	UPROPERTY(EditAnywhere)
	float CamerasOffset;

	UPROPERTY(EditAnywhere)
	float CamerasHeight = 1020;

	UPROPERTY()
	APlayerController* PlayerController;

	UPROPERTY()
	int ActualCameraIndex;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	virtual void RotateCamera(int Direction);

	virtual void SetGridManager(AGDGrid* Gm);
	void SetCamerasPositions();
};
