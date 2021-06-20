// © 2021 Alessandro Arena, Mattia De Prisco, Nunzio Lopardo All Rights Reserved


#include "GDPlayerPawn.h"

#include "DrawDebugHelpers.h"
#include "Blueprint/UserWidget.h"
#include "Camera/CameraActor.h"
#include "GDProject/GDProjectGameModeBase.h"
#include "GDProject/Tiles/GDTile.h"
#include "GDProject/Units/GDUnit.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"

AGDPlayerPawn::AGDPlayerPawn()
{
	PrimaryActorTick.bCanEverTick = true;
	AutoPossessPlayer = EAutoReceiveInput::Player0;
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AGDCameraManager::StaticClass(), FoundActors);
	UE_LOG(LogTemp, Warning, TEXT("Founded CameraManager, %i"), FoundActors.Num())
	if (FoundActors.Num() > 0)
	{
		CameraManger = Cast<AGDCameraManager>(FoundActors[0]);
		if (CameraManger) UE_LOG(LogTemp, Error, TEXT("Founded CameraManager"));
	}
}

void AGDPlayerPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	HandleTilesHovering();
}

void AGDPlayerPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction("TriggerClick", EInputEvent::IE_Pressed, this, &AGDPlayerPawn::TriggerClick);
	PlayerInputComponent->BindAction("RemoveSelection", EInputEvent::IE_Pressed, this,
	                                 &AGDPlayerPawn::DeselectTileElement);
	PlayerInputComponent->BindAction("RotateCamLeft", IE_Pressed, this, &AGDPlayerPawn::RotateCameraLeft);
	PlayerInputComponent->BindAction("RotateCamRight", IE_Pressed, this, &AGDPlayerPawn::RotateCameraRight);
}

void AGDPlayerPawn::OnTurnBegin()
{
}

void AGDPlayerPawn::OnTurnEnd()
{
	DeselectTileElement();
}

void AGDPlayerPawn::AddActiveUnit(AGDUnit* Unit)
{
	ActiveUnits.Add(Unit);
}

void AGDPlayerPawn::RemoveActiveUnit(AGDUnit* Unit)
{
	ActiveUnits.Remove(Unit);
}

int AGDPlayerPawn::GetCurrentPlayerTurn() const
{
	if (AGDProjectGameModeBase* GameMode = Cast<AGDProjectGameModeBase>(GetWorld()->GetAuthGameMode()))
	{
		return GameMode->GetCurrentPlayerTurn();
	}

	UE_LOG(LogTemp, Error, TEXT("Game mode not found."))
	return -1;
}

void AGDPlayerPawn::HandleTilesHovering()
{
	AGDTile* TargetTile = GetTileUnderMouse();

	if (TargetTile != HoveringTile)
	{
		UpdateHoveringTile(TargetTile);

		if (SelectedTileElement && ActiveUnits.Num() == 0)
		{
			if (AGDUnit* Unit = Cast<AGDUnit>(SelectedTileElement))
			{
				Unit->HighlightActions(HoveringTile);
			}
		}
	}
}

AGDTile* AGDPlayerPawn::GetTileUnderMouse() const
{
	APlayerController* PC = Cast<APlayerController>(GetController());
	if (!PC)
	{
		return nullptr;
	}

	FVector Start, Dir;
	PC->DeprojectMousePositionToWorld(Start, Dir);
	const FVector End = Start + Dir * 8000.f;

	return TraceForTile(Start, End, false);
}

void AGDPlayerPawn::BeginPlay()
{
	Super::BeginPlay();

	if (UnitActionsWidgetClass)
	{
		UnitActionsWidget = CreateWidget(GetWorld(), UnitActionsWidgetClass);
	}
}

void AGDPlayerPawn::HighlightHoveringTile() const
{
	if (HoveringTile)
	{
		EHighlightInfo HighlightInfo = EHighlightInfo::Default;

		AGDUnit* HoveringUnit = Cast<AGDUnit>(HoveringTile->GetTileElement());
		if (HoveringUnit)
		{
			if (HoveringUnit->IsOwnedByPlayer(GetCurrentPlayerTurn()))
			{
				HighlightInfo = EHighlightInfo::Ally;
			}
			else
			{
				HighlightInfo = EHighlightInfo::Enemy;
			}
		}
		HoveringTile->Highlight(HighlightInfo);
	}
}

void AGDPlayerPawn::UpdateHoveringTile(AGDTile* NewHoveringTile)
{
	if (HoveringTile)
	{
		HoveringTile->RemoveHighlight();
	}

	HoveringTile = NewHoveringTile;

	HighlightHoveringTile();
}

void AGDPlayerPawn::RequestUnitAction() const
{
	if (AGDUnit* Unit = Cast<AGDUnit>(SelectedTileElement))
	{
		Unit->RequestAction(HoveringTile);
	}
}

void AGDPlayerPawn::TriggerClick()
{
	AGDUnit* SelectedUnit = Cast<AGDUnit>(SelectedTileElement);
	if (SelectedUnit && SelectedUnit->IsUnitRotating())
	{
		UE_LOG(LogTemp, Warning, TEXT("Trying to stop rotation"));
		SelectedUnit->Rotate();
	}
	else if (ActiveUnits.Num() == 0)
	{
		if (!SelectedTileElement)
		{
			UE_LOG(LogTemp, Warning, TEXT("Selecting tile element"));
			SelectTileElement();
		}
		else
		{
			if (IGDTileElement::Execute_GetTile(SelectedTileElement) != HoveringTile)
			{
				RequestUnitAction();
			}
		}
	}
}

void AGDPlayerPawn::RotateCameraLeft()
{
	UE_LOG(LogTemp, Error, TEXT("Rotating Left"));
	if (CameraManger) CameraManger->RotateCamera(1);
}

void AGDPlayerPawn::RotateCameraRight()
{
	UE_LOG(LogTemp, Error, TEXT("Rotating Right"));
	if (CameraManger) CameraManger->RotateCamera(-1);
}

void AGDPlayerPawn::SelectTileElement()
{
	if (HoveringTile && HoveringTile->IsOccupied())
	{
		AGDUnit* Unit = Cast<AGDUnit>(HoveringTile->GetTileElement());
		if (Unit && Unit->IsOwnedByPlayer(GetCurrentPlayerTurn()))
		{
			HoveringTile->Select();

			IGDTileElement::Execute_Select(Unit);
			SelectedTileElement = Unit;
			if (UnitActionsWidget && !UnitActionsWidget->IsInViewport())
			{
				UnitActionsWidget->AddToViewport();
			}
		}
	}
}

void AGDPlayerPawn::DeselectTileElement()
{
	AGDUnit* SelectedUnit = Cast<AGDUnit>(SelectedTileElement);
	if (SelectedUnit && SelectedUnit->IsUnitRotating())
	{
		if (SelectedUnit->IsUnitRotating())
		{
			UE_LOG(LogTemp, Warning, TEXT("DESELECTION STOP ROTATION"));
			SelectedUnit->Rotate();
		}
	}

	if (ActiveUnits.Num() == 0 && SelectedTileElement)
	{
		IGDTileElement::Execute_Deselect(SelectedTileElement);
		IGDTileElement::Execute_GetTile(SelectedTileElement)->Deselect();

		if (UnitActionsWidget)
		{
			UnitActionsWidget->RemoveFromViewport();
		}

		SelectedTileElement = nullptr;
	}
}

void AGDPlayerPawn::OnUnitDead(AGDUnit* Unit, const int OwningPlayer)
{
	if (SelectedTileElement == Unit)
	{
		DeselectTileElement();
	}

	if (AGDProjectGameModeBase* GM = Cast<AGDProjectGameModeBase>(GetWorld()->GetAuthGameMode()))
	{
		GM->OnUnitDead(Unit, OwningPlayer);
	}
}

AGDTile* AGDPlayerPawn::TraceForTile(const FVector& Start, const FVector& End, bool bDrawDebugHelpers) const
{
	FHitResult HitResult;
	GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility);
	if (bDrawDebugHelpers)
	{
		DrawDebugLine(GetWorld(), Start, HitResult.Location, FColor::Red);
		DrawDebugSolidBox(GetWorld(), HitResult.Location, FVector(20.0f), FColor::Red);
	}

	AGDTile* HitTile = nullptr;

	if (HitResult.Actor.IsValid())
	{
		HitTile = Cast<AGDTile>(HitResult.Actor.Get());
	}

	return HitTile;
}
