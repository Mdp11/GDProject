// © 2021 Alessandro Arena, Mattia De Prisco, Nunzio Lopardo All Rights Reserved


#include "GDPlayerPawn.h"

#include "DrawDebugHelpers.h"
#include "GDProject/Tiles/GDGrid.h"
#include "GDProject/Tiles/GDTile.h"
#include "GDProject/Units/GDUnit.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

AGDPlayerPawn::AGDPlayerPawn()
{
	PrimaryActorTick.bCanEverTick = true;
	AutoPossessPlayer = EAutoReceiveInput::Player0;
}


void AGDPlayerPawn::DisableHoveringVisuals()
{
	if (HoveringTile)
	{
		HoveringTile->HighlightTargetEnemy(false);
		HoveringTile->Highlight(false);
		HoveringTile = nullptr;
	}
}

void AGDPlayerPawn::UpdateHoveringTile(AGDTile* TargetTile)
{
	if (HoveringTile)
	{
		HoveringTile->Highlight(false);
	}

	TargetTile->Highlight(true);
	HoveringTile = TargetTile;
}

void AGDPlayerPawn::UpdateUnitActionsHovering(AGDUnit* Unit)
{
	if (HoveringTile->IsTraversable())
	{
		if (HoveringTile->IsOccupied())
		{
			HoveringTile->HighlightTargetEnemy(true);
		}
		else
		{
			AGDGrid* Grid = HoveringTile->GetGrid();
			AGDTile* StartTile = IGDTileElement::Execute_GetTile(Unit);

			TArray<AGDTile*> MovementPath = Grid->ComputePathBetweenTiles(StartTile, HoveringTile);
			if (MovementPath.Num() > 0)
			{
				MovementPath.RemoveAt(0); //First tile is the one unit is on, so it can be removed
				if (MovementPath.Num() <= Unit->GetMovementRange() * Unit->GetActionPoints())
				{
					FillHighlightedPath(MovementPath);
				}
			}
		}
	}
}

void AGDPlayerPawn::HandleTilesHovering()
{
	APlayerController* PC = Cast<APlayerController>(GetController());
	if (!PC)
	{
		return;
	}

	FVector Start, Dir;
	PC->DeprojectMousePositionToWorld(Start, Dir);
	const FVector End = Start + Dir * 8000.f;

	if (AGDTile* TargetTile = TraceForTile(Start, End, false))
	{
		if (TargetTile != HoveringTile)
		{
			UpdateHoveringTile(TargetTile);

			if (SelectedTile)
			{
				if (AGDUnit* Unit = Cast<AGDUnit>(SelectedTile->GetTileElement()))
				{
					UpdateUnitActionsHovering(Unit);
				}
			}
		}
	}
	else
	{
		DisableHoveringVisuals();
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
	PlayerInputComponent->BindAction("RemoveSelection", EInputEvent::IE_Pressed, this, &AGDPlayerPawn::UnselectTile);
}

void AGDPlayerPawn::RequestMove(AGDUnit* Unit) const
{
	Unit->RequestMoveAlongPath(HighlightedPath);
}

void AGDPlayerPawn::RequestAttack(AGDUnit* AttackingUnit, AGDUnit* EnemyUnit) const
{
	AttackingUnit->Attack(EnemyUnit);
}

void AGDPlayerPawn::RequestUnitAction(AGDUnit* Unit) const
{
	if (HoveringTile)
	{
		if (HoveringTile->IsOccupied())
		{
			if (AGDUnit* OtherUnit = Cast<AGDUnit>(HoveringTile->GetTileElement()) /* && OtherUnit is enemy */)
			{
				UE_LOG(LogTemp, Warning, TEXT("Requesting attack"));
				RequestAttack(Unit, OtherUnit);
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Requesting move"));
			RequestMove(Unit);
		}
	}
}

void AGDPlayerPawn::SelectTile()
{
	if (HoveringTile && HoveringTile->IsOccupied())
	{
		AGDUnit* Unit = Cast<AGDUnit>(HoveringTile->GetTileElement());
		if (Unit && Unit->GetActionPoints() > 0)
		{
			Unit->Select();

			SelectedTile = HoveringTile;
			SelectedTile->HandleClicked();
		}
	}
}

void AGDPlayerPawn::UnselectTile()
{
	if(SelectedTile->IsOccupied())
	{
		if(AGDUnit* Unit = Cast<AGDUnit>(SelectedTile->GetTileElement()))
		{
			Unit->Deselect();
		}
	}
	
	SelectedTile->HandleClicked();
	SelectedTile = nullptr;
}

void AGDPlayerPawn::TriggerClick()
{
	if (!SelectedTile)
	{
		UE_LOG(LogTemp, Warning, TEXT("Selecting tile"));
		SelectTile();
	}
	else
	{
		if (SelectedTile != HoveringTile)
		{
			if (AGDUnit* Unit = Cast<AGDUnit>(SelectedTile->GetTileElement()))
			{
				RequestUnitAction(Unit);
			}
		}
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