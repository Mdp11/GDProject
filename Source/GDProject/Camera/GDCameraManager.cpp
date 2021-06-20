// © 2021 Alessandro Arena, Mattia De Prisco, Nunzio Lopardo All Rights Reserved


#include "GDCameraManager.h"

#include "Camera/CameraActor.h"
#include "Kismet/GameplayStatics.h"


#define LEFT 1
#define RIGHT -1

// Sets default values
AGDCameraManager::AGDCameraManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AGDCameraManager::BeginPlay()
{
	Super::BeginPlay();
	PlayerController = UGameplayStatics::GetPlayerController(this, 0);
	ActualCameraIndex = 0;
	/*
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AGDCameraManager::StaticClass(), FoundActors);
	UE_LOG(LogTemp, Warning, TEXT("Number of Camera Fouded, %i"), FoundActors.Num());
	if (FoundActors.Num() > 0)
	{
		for (auto FoundActor : FoundActors)
        {
        	Cameras.Add(Cast<ACameraActor>(FoundActor));
        }
	}
	*/
	Cameras.Add(Camera0);
	Cameras.Add(Camera1);
	Cameras.Add(Camera2);
	Cameras.Add(Camera3);	
}

// Called every frame
void AGDCameraManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AGDCameraManager::RotateCamera(int Direction)
{
	UE_LOG(LogTemp, Error, TEXT("Index, %i"),ActualCameraIndex);

	ActualCameraIndex+=Direction;
	if (ActualCameraIndex < 0)
	{
		ActualCameraIndex = Cameras.Num() - 1;
	} else if (ActualCameraIndex >= Cameras.Num())
	{
		ActualCameraIndex = 0;
	}
	UE_LOG(LogTemp, Error, TEXT("Index, %i"),ActualCameraIndex);
	PlayerController->SetViewTarget(Cameras[ActualCameraIndex]);
	
}


