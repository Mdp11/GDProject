// © 2021 Alessandro Arena, Mattia De Prisco, Nunzio Lopardo All Rights Reserved


#include "GDPlayerPawn.h"

#include "DrawDebugHelpers.h"
#include "Blueprint/UserWidget.h"
#include "GDProject/GDProjectGameModeBase.h"
#include "GDProject/Tiles/GDTile.h"
#include "GDProject/Units/GDUnit.h"

AGDPlayerPawn::AGDPlayerPawn()
{
	PrimaryActorTick.bCanEverTick = true;
	AutoPossessPlayer = EAutoReceiveInput::Player0;
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
	PlayerInputComponent->BindAction("RemoveSelection", EInputEvent::IE_Pressed, this, &AGDPlayerPawn::DeselectTile);
}

void AGDPlayerPawn::OnTurnBegin()
{
}

void AGDPlayerPawn::OnTurnEnd()
{
	DeselectTile();
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
	if (AGDProjectGameModeBase* GM = Cast<AGDProjectGameModeBase>(GetWorld()->GetAuthGameMode()))
	{
		return GM->GetCurrentPlayerTurn();
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

void AGDPlayerPawn::UpdateHoveringTile(AGDTile* NewHoveringTile)
{
	if (HoveringTile)
	{
		HoveringTile->Highlight(false);
	}

	if (NewHoveringTile)
	{
		AGDUnit* HoveringUnit = Cast<AGDUnit>(NewHoveringTile->GetTileElement());
		if (HoveringUnit)
		{
			if (HoveringUnit->IsOwnedByPlayer(GetCurrentPlayerTurn()))
			{
				NewHoveringTile->HighlighAllyTarget(true);
			}
			else
			{
				NewHoveringTile->HighlightEnemyTarget(true);
			}
		}
		else
		{
			NewHoveringTile->Highlight(true);
		}
	}

	HoveringTile = NewHoveringTile;
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

	UE_LOG(LogTemp, Warning, TEXT("Active units = %d"), ActiveUnits.Num());
	if (ActiveUnits.Num() == 0)
	{
		if (!SelectedTileElement)
		{
			UE_LOG(LogTemp, Warning, TEXT("Selecting tile"));
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

void AGDPlayerPawn::SelectTileElement()
{
	if (HoveringTile && HoveringTile->IsOccupied())
	{
		if (AGDUnit* Unit = Cast<AGDUnit>(HoveringTile->GetTileElement()))
		{
			if (Unit->IsOwnedByPlayer(GetCurrentPlayerTurn()))
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
}

void AGDPlayerPawn::DeselectTile()
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
