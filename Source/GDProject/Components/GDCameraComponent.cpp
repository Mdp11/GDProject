// © 2021 Alessandro Arena, Mattia De Prisco, Nunzio Lopardo All Rights Reserved


#include "GDCameraComponent.h"

#include "Camera/CameraActor.h"
#include "Camera/CameraComponent.h"
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
	TArray<AActor*> FoundGridManagerActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AGDGrid::StaticClass(), FoundGridManagerActors);
	int GridManagerX = 0;
	int GridManagerY = 0;
	int GridManagerZ = 0;
	if (FoundGridManagerActors.Num() > 0)
	{
		GridManager = Cast<AGDGrid>(FoundGridManagerActors[0]);
		if (GridManager)
		{
			GridManagerX = GridManager->GetActorLocation().X;
			GridManagerY = GridManager->GetActorLocation().Y;
			GridManagerZ = GridManager->GetActorLocation().Z;
			TArray<TArray<AGDTile*>> TilesGrid;
			UE_LOG(LogTemp, Display, TEXT("grid size, %i"), GridManager->GetSize());
			GridSize = GridManager->GetSize() - 1;
			TilesGrid = GridManager->GetTilesGrid();
			EndPositionX = (TilesGrid.Last()[0]->GetActorLocation().X);
			EndPositionY = (TilesGrid[0].Last()->GetActorLocation().Y);
			CamerasHeight += GridManagerZ;
		}
	}


	
	//Setting camera positions base on grid size
	Camera0Pos.Set(GridManagerX-CamerasOffset,((EndPositionY+GridManagerY)/2),CamerasHeight);
	Camera1Pos.Set(((EndPositionX+GridManagerX)/2), GridManagerY - CamerasOffset, CamerasHeight);
	Camera2Pos.Set(EndPositionX + CamerasOffset, ((EndPositionY+GridManagerY) / 2), CamerasHeight);
	Camera3Pos.Set(((EndPositionX+GridManagerX)/2), EndPositionY+CamerasOffset, CamerasHeight);

	Camera0Rot.Pitch = -CameraAngle;
	Camera0Rot.Yaw = 0;
	Camera0Rot.Roll = 0;

	Camera1Rot.Pitch = -CameraAngle;
	Camera1Rot.Yaw = Camera0Rot.Yaw + 90;
	Camera1Rot.Roll = 0;
	
	Camera2Rot.Pitch = -CameraAngle;
	Camera2Rot.Yaw = Camera0Rot.Yaw + 180;
	Camera2Rot.Roll = 0;
	
	Camera3Rot.Pitch = -CameraAngle;
	Camera3Rot.Yaw = Camera0Rot.Yaw - 90;
	Camera3Rot.Roll = 0;
	
	PlayerController = UGameplayStatics::GetPlayerController(this, 0);
	TargetLocation = Camera0Pos;
	TargetRotation = Camera0Rot;
	Camera = GetWorld()->SpawnActor<ACameraActor>(ACameraActor::StaticClass(), Camera0Pos, Camera0Rot);
	Camera->GetCameraComponent()->SetFieldOfView(CameraFOV);
	Camera->GetCameraComponent()->SetConstraintAspectRatio(false);	
	PlayerController->SetViewTarget(Camera);
}

// Called every frame
void UGDCameraComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                       FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if(!Camera->GetActorLocation().Equals(TargetLocation, 0.1))
	{
		FVector StepLocation = FMath::VInterpTo(Camera->GetActorLocation(), TargetLocation, 1.f, 0.1f);
		Camera->SetActorLocation(StepLocation);
	}
	
	if(!Camera->GetActorRotation().Equals(TargetRotation, 0.1))
	{
		FRotator StepRotation = FMath::RInterpTo(Camera->GetActorRotation(), TargetRotation, 1.f, 0.1f);
		Camera->SetActorRotation(StepRotation);
	}

}

void UGDCameraComponent::RotateCamera(int Direction)
{
	if (GridSize > 0)
	{
		ActualCameraIndex += Direction;
		if (ActualCameraIndex < 0) ActualCameraIndex = 3;
		if (ActualCameraIndex > 3) ActualCameraIndex = 0;
		switch (ActualCameraIndex)
		{
		case 0:
			TargetLocation = Camera0Pos;
			TargetRotation = Camera0Rot;
			break;
		case 1:
			TargetLocation = Camera1Pos;
			TargetRotation = Camera1Rot;
			break;
		case 2:
			TargetLocation = Camera2Pos;
			TargetRotation = Camera2Rot;
			break;
		case 3:
			TargetLocation = Camera3Pos;
			TargetRotation = Camera3Rot;
			break;
		}
	}
}