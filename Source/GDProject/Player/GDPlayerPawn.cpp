// © 2021 Alessandro Arena, Mattia De Prisco, Nunzio Lopardo All Rights Reserved


#include "GDPlayerPawn.h"

#include "DrawDebugHelpers.h"
#include "Blueprint/UserWidget.h"
#include "Camera/CameraActor.h"
#include "GDProject/GDProjectGameModeBase.h"
#include "GDProject/Tiles/GDTile.h"
#include "GDProject/Units/GDUnit.h"
#include "GDProject/Items/GDItemBase.h"
#include "GDProject/Components/GDInventoryComponent.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"

void AGDPlayerPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	HandleTilesHovering();
}

AGDPlayerPawn::AGDPlayerPawn()
{
	PrimaryActorTick.bCanEverTick = true;
	AutoPossessPlayer = EAutoReceiveInput::Player0;
	// TArray<AActor*> FoundGridManagerActors;
	// UGameplayStatics::GetAllActorsOfClass(GetWorld(), AGDGrid::StaticClass(), FoundGridManagerActors);
	// UE_LOG(LogTemp, Warning, TEXT("Founded GridManager, %i"), FoundGridManagerActors.Num())
	// if (FoundGridManagerActors.Num() > 0)
	// {
	// 	GridManger = Cast<AGDGrid>(FoundGridManagerActors[0]);
	// 	if (GridManger) UE_LOG(LogTemp, Error, TEXT("Founded GridManager"));
	// }
	// TArray<AActor*> FoundCameraManagerActors;
	// UGameplayStatics::GetAllActorsOfClass(GetWorld(), AGDCameraManager::StaticClass(), FoundCameraManagerActors);
	// UE_LOG(LogTemp, Warning, TEXT("Founded CameraManager, %i"), FoundCameraManagerActors.Num())
	// if (FoundCameraManagerActors.Num() > 0)
	// {
	// 	CameraManger = Cast<AGDCameraManager>(FoundCameraManagerActors[0]);
	// 	if (CameraManger) UE_LOG(LogTemp, Error, TEXT("Founded CameraManager"));
	// 	CameraManger->SetGridManager(GridManger);
	// }

	Inventory = CreateDefaultSubobject<UGDInventoryComponent>(TEXT("Inventory"));
}

void AGDPlayerPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction("TriggerClick", EInputEvent::IE_Pressed, this, &AGDPlayerPawn::TriggerClick);

	DECLARE_DELEGATE_OneParam(FDelegate_RemoveSelection, bool);
	PlayerInputComponent->BindAction<FDelegate_RemoveSelection>("RemoveSelection", EInputEvent::IE_Pressed, this,
	                                                            &AGDPlayerPawn::DeselectTileElement, true);
	// PlayerInputComponent->BindAction("RotateCamLeft", IE_Pressed, this, &AGDPlayerPawn::RotateCameraLeft);
	// PlayerInputComponent->BindAction("RotateCamRight", IE_Pressed, this, &AGDPlayerPawn::RotateCameraRight);
}

void AGDPlayerPawn::OnTurnBegin()
{
}

void AGDPlayerPawn::OnTurnEnd()
{
	DeselectTileElement();
}

void AGDPlayerPawn::AddActiveEntity(UObject* Entity)
{
	ActiveEntities.Add(Entity);

	if (ActiveEntities.Num() - 1 == 0)
	{
		HoveringTile = nullptr;
		HandleTilesHovering();
	}
}

void AGDPlayerPawn::RemoveActiveEntity(UObject* Entity)
{
	ActiveEntities.Remove(Entity);

	if (ActiveEntities.Num() == 0)
	{
		HoveringTile = nullptr;
		HandleTilesHovering();
	}
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

		if (!SelectedItem && SelectedTileElement && ActiveEntities.Num() == 0)
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
	if (SelectedItem)
	{
		const bool bNonUsable = !(ActiveEntities.Num() == 0);
		SelectedItem->HighlightAffectedTiles(HoveringTile, bNonUsable);
	}
	else if (HoveringTile)
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
	if (SelectedItem && HoveringTile)
	{
		if (ActiveEntities.Num() == 0 && SelectedItem->RequestUse(HoveringTile))
		{
			Inventory->RemoveBattleItem(SelectedItem);
			OnItemDeselected();
		}
	}
	else
	{
		AGDUnit* SelectedUnit = Cast<AGDUnit>(SelectedTileElement);
		if (SelectedUnit && SelectedUnit->IsUnitRotating())
		{
			UE_LOG(LogTemp, Warning, TEXT("Trying to stop rotation"));
			SelectedUnit->Rotate();
		}
		else if (ActiveEntities.Num() == 0)
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

void AGDPlayerPawn::DeselectTileElement(const bool bReset)
{
	if (SelectedItem)
	{
		OnItemDeselected();
		if (HoveringTile)
		{
			HoveringTile->RemoveHighlight();
		}
	}
	else
	{
		AGDUnit* SelectedUnit = Cast<AGDUnit>(SelectedTileElement);
		if (SelectedUnit && SelectedUnit->IsUnitRotating())
		{
			if (SelectedUnit->IsUnitRotating())
			{
				SelectedUnit->Rotate();
			}
		}

		if (ActiveEntities.Num() == 0 && SelectedTileElement)
		{
			IGDTileElement::Execute_Deselect(SelectedTileElement);
			IGDTileElement::Execute_GetTile(SelectedTileElement)->Deselect();

			if (UnitActionsWidget)
			{
				UnitActionsWidget->RemoveFromViewport();
			}

			if (bReset)
			{
				SelectedTileElement = nullptr;
			}
		}
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

void AGDPlayerPawn::OnItemSelected(UGDItemBase* Item)
{
	if (!SelectedItem && SelectedTileElement)
	{
		DeselectTileElement(false);
	}

	SelectedItem = Item;
	SelectedItem->OnSelect();
}

void AGDPlayerPawn::OnItemDeselected()
{
	SelectedItem->OnDeselect();
	SelectedItem = nullptr;

	if (SelectedTileElement)
	{
		HoveringTile = IGDTileElement::Execute_GetTile(SelectedTileElement);
		SelectTileElement();
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
