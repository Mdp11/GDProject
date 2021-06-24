// © 2021 Alessandro Arena, Mattia De Prisco, Nunzio Lopardo All Rights Reserved


#include "GDCameraManager.h"

#include "Camera/CameraActor.h"
#include "GDProject/Tiles/GDTile.h"
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
	SetCamerasPositions();
	Cameras.Add(Camera0);
	Cameras.Add(Camera1);
	Cameras.Add(Camera2);
	Cameras.Add(Camera3);
	CamerasOffset = 600;
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
	PlayerController->SetViewTargetWithBlend(Cameras[ActualCameraIndex], 0.5);
	
}

void AGDCameraManager::SetGridManager(AGDGrid* Gm)
{
	GridManager = Gm;
}

void AGDCameraManager::SetCamerasPositions()
{
	int GridSize = GridManager->GetSize() - 1;
	TArray<TArray<AGDTile*>> TilesGrid =  GridManager->GetTilesGrid();
	UE_LOG(LogTemp, Error, TEXT("GridSize, %i"),GridSize);
	FVector EndPositionY = TilesGrid[0].Last()->GetActorLocation();
	UE_LOG(LogTemp, Error, TEXT("GridSize, %i"),GridSize);
	FVector EndPositionX = TilesGrid.Last()[0]->GetActorLocation();
	UE_LOG(LogTemp, Error, TEXT("EndPosX, %f"),EndPositionX.X);
	UE_LOG(LogTemp, Error, TEXT("EndPosY, %f"),EndPositionY.Y);
	//CamerasOffset = round((600*EndPositionY.Y)/2090);
	CamerasOffset = 600;
	FVector Camera0Pos(-CamerasOffset, EndPositionY.Y/2, CamerasHeight);
	FVector Camera1Pos(EndPositionX.X/2,-CamerasOffset, CamerasHeight);
	FVector Camera2Pos(EndPositionX.X+ CamerasOffset, EndPositionY.Y/2, CamerasHeight);
	FVector Camera3Pos(EndPositionX.X/2,EndPositionY.Y+CamerasOffset, CamerasHeight);
	Camera0->SetActorLocation(Camera0Pos);
	Camera1->SetActorLocation(Camera1Pos);
	Camera2->SetActorLocation(Camera2Pos);
	Camera3->SetActorLocation(Camera3Pos);
}

