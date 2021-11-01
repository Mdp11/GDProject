// © 2021 Alessandro Arena, Mattia De Prisco, Nunzio Lopardo All Rights Reserved


#include "GDCameraComponent.h"

#include "Camera/CameraActor.h"
#include "GDProject/Tiles/GDTile.h"
#include "Kismet/GameplayStatics.h"

// Sets default values for this component's properties
UGDCameraComponent::UGDCameraComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

// Called when the game starts
void UGDCameraComponent::BeginPlay()
{
	Super::BeginPlay();
	PlayerController = UGameplayStatics::GetPlayerController(this, 0);
	//This value indicate the distance between the camera and the grid
	CamerasOffset = 680;
	//SetCamerasPositions();
	const FVector CameraPos(-CamerasOffset,1050.000000,CamerasHeight);
	const FRotator CameraRot(-43,0,0);
	TargetLocation = CameraPos;
	TargetRotation = CameraRot;
	Camera = GetWorld()->SpawnActor<ACameraActor>(ACameraActor::StaticClass(), CameraPos, CameraRot);
	PlayerController->SetViewTarget(Camera);
}

// void UGDCameraComponent::SetCamerasPositions()
// {
// 	TArray<TArray<AGDTile*>> TilesGrid;
// 	int32 GridSize;
// 	if (GridManager)
// 	{
// 		UE_LOG(LogTemp, Warning, TEXT("grid size, %i"), GridManager->GetSize());
// 		GridSize = GridManager->GetSize() - 1;
// 		TilesGrid = GridManager->GetTilesGrid();
// 	}
// 	UE_LOG(LogTemp, Warning, TEXT("grid size, %i"), TilesGrid.Num());
//
// 	if (TilesGrid.Num() > 0)
// 	{
// 		UE_LOG(LogTemp, Error, TEXT("GridSize, %i"), GridSize);
// 		FVector EndPositionY = TilesGrid[0].Last()->GetActorLocation();
// 		FVector EndPositionX = TilesGrid.Last()[0]->GetActorLocation();
// 		UE_LOG(LogTemp, Error, TEXT("EndPosX, %f"), EndPositionX.X);
// 		UE_LOG(LogTemp, Error, TEXT("EndPosY, %f"), EndPositionY.Y);
// 		//CamerasOffset = round((600*EndPositionY.Y)/2090);
// 		CamerasOffset = 600;
// 		FVector Camera0Pos(-CamerasOffset, EndPositionY.Y / 2, CamerasHeight);
// 		FVector Camera1Pos(EndPositionX.X / 2, -CamerasOffset, CamerasHeight);
// 		FVector Camera2Pos(EndPositionX.X + CamerasOffset, EndPositionY.Y / 2, CamerasHeight);
// 		FVector Camera3Pos(EndPositionX.X / 2, EndPositionY.Y + CamerasOffset, CamerasHeight);
// 		Camera->SetActorLocation(Camera0Pos);
// 	}
// }

// Called every frame
void UGDCameraComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                       FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if(!Camera->GetActorLocation().Equals(TargetLocation, 0.1))
	{
		FVector StepLocation = FMath::VInterpTo(Camera->GetActorLocation(), TargetLocation, 1.f, 0.95f);
		Camera->SetActorLocation(StepLocation);
	}
	
	if(!Camera->GetActorRotation().Equals(TargetRotation, 0.1))
	{
		FRotator StepRotation = FMath::RInterpTo(Camera->GetActorRotation(), TargetRotation, 1.f, 0.95f);
		Camera->SetActorRotation(StepRotation);
	}

}

void UGDCameraComponent::RotateCamera(int Direction)
{
	if (GridSize > 0)
	{
		UE_LOG(LogTemp, Display, TEXT("EndPosX, %f"), EndPositionX.X);
		UE_LOG(LogTemp, Display, TEXT("EndPosY, %f"), EndPositionY.Y);
		FVector Camera0Pos(-CamerasOffset,1050,1030);
		FVector Camera1Pos(1050, -CamerasOffset, 1030);
		FVector Camera2Pos((1050*2)+CamerasOffset, 1050, 1030);
		FVector Camera3Pos(1050, (1050*2)+CamerasOffset, 1030);
		
		// FVector Camera1Pos(-CamerasOffset, EndPositionY.Y / 2, CamerasHeight);
		// FVector Camera3Pos(EndPositionX.X + CamerasOffset, EndPositionY.Y / 2, CamerasHeight);
		// FVector Camera2Pos(EndPositionX.X / 2, EndPositionY.Y + CamerasOffset, CamerasHeight);
		// FVector Camera0Pos(EndPositionX.X / 2, -CamerasOffset, CamerasHeight);
		// FRotator Camera0Rot, Camera1Rot, Camera2Rot, Camera3Rot = Camera->GetActorRotation();
		// Camera1Rot.Yaw -= 90.f;
		// Camera2Rot.Yaw -= 180.f;
		// Camera3Rot.Yaw += 90.f;

		FRotator Camera0Rot(-43,0,0);
		FRotator Camera1Rot(-43,+90,0);
		FRotator Camera2Rot(-43,+180,0);
		FRotator Camera3Rot(-43,-90,0);
		
		TArray<TPair<FVector, FRotator>> CameraPosition;		
		
		FRotator CameraRotation;
		UE_LOG(LogTemp, Display, TEXT("Direction, %i"), Direction);
		ActualCameraIndex += Direction;
		if (ActualCameraIndex < 0) ActualCameraIndex = 3;
		if (ActualCameraIndex > 3) ActualCameraIndex = 0;
		UE_LOG(LogTemp, Display, TEXT("Index, %i"), ActualCameraIndex);
		switch (ActualCameraIndex)
		{
		case 0:
			TargetLocation = Camera0Pos;
			TargetRotation = Camera0Rot;
			UE_LOG(LogTemp, Display, TEXT("Camera Rot %f, %f, %f"), TargetRotation.Pitch, TargetRotation.Yaw, TargetRotation.Roll);
			break;
		case 1:
			TargetLocation = Camera1Pos;
			TargetRotation = Camera1Rot;
			UE_LOG(LogTemp, Display, TEXT("Camera Rot %f, %f, %f"), TargetRotation.Pitch, TargetRotation.Yaw, TargetRotation.Roll);
			break;
		case 2:
			TargetLocation = Camera2Pos;
			TargetRotation = Camera2Rot;
			UE_LOG(LogTemp, Display, TEXT("Camera Rot %f, %f, %f"), TargetRotation.Pitch, TargetRotation.Yaw, TargetRotation.Roll);
			break;
		case 3:
			TargetLocation = Camera3Pos;
			TargetRotation = Camera3Rot;
			UE_LOG(LogTemp, Display, TEXT("Camera Rot %f, %f, %f"), TargetRotation.Pitch, TargetRotation.Yaw, TargetRotation.Roll);
			break;
		}
	}
}

void UGDCameraComponent::SetGridManager(AGDGrid* Gm)
{
	GridManager = Gm;
	if (GridManager)
	{
		TArray<TArray<AGDTile*>> TilesGrid;
		UE_LOG(LogTemp, Display, TEXT("grid size, %i"), GridManager->GetSize());
		GridSize = GridManager->GetSize() - 1;
		TilesGrid = GridManager->GetTilesGrid();
		EndPositionY = TilesGrid[0].Last()->GetActorLocation();
		EndPositionX = TilesGrid.Last()[0]->GetActorLocation();
	}
}
