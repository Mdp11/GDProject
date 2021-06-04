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
		HoveringTile->HighlightDanger(false);
		HoveringTile->Highlight(false);
		HoveringTile = nullptr;
	}

	if (HighlightedPath.Num() > 0)
	{
		EmptyHighlightedPath();
	}
}

void AGDPlayerPawn::UpdateHoveringTile(AGDTile* TargetTile)
{
	EmptyHighlightedPath();

	if (HoveringTile)
	{
		HoveringTile->Highlight(false);
	}

	UE_LOG(LogTemp, Warning, TEXT("Ci arrivo?"));
	TargetTile->Highlight(true);
	HoveringTile = TargetTile;
}

void AGDPlayerPawn::UpdateUnitActionsHovering(AGDUnit* Unit)
{
	EmptyHighlightedPath();

	if (HoveringTile->IsTraversable())
	{
		if (HoveringTile->IsOccupied())
		{
			HoveringTile->HighlightDanger(true);
		}
		else
		{
			AGDGrid* Grid = HoveringTile->GetGrid();
			AGDTile* StartTile = IGDTileElement::Execute_GetTile(Unit);

			FillHighlightedPath(Grid->ComputePathBetweenTiles(StartTile, HoveringTile));
		}
	}
}

void AGDPlayerPawn::HighlightUnitMovementRange(AGDUnit* Unit)
{
	AGDTile* CurrentTile = IGDTileElement::Execute_GetTile(Unit);

	const int UnitActionPoints = Unit->GetActionPoints();

	if (UnitActionPoints > 1)
	{
		const TSet<AGDTile*> ReachableTiles = CurrentTile->GetGrid()->GetTilesAtDistance(
			CurrentTile, Unit->GetMovementRange() * UnitActionPoints);
		const TSet<AGDTile*> NearTiles = CurrentTile->GetGrid()->GetTilesAtDistance(
			CurrentTile, Unit->GetMovementRange());

		TSet<AGDTile*> FarTiles = ReachableTiles.Difference(NearTiles);

		for (const auto& Tile : NearTiles)
		{
			Tile->ApplyMovementRangeDecal(true);
		}

		for (const auto& Tile : FarTiles)
		{
			Tile->ApplyMovementRangeDecal(false);
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
}

void AGDPlayerPawn::RequestMove(AGDUnit* Unit) const
{
	Unit->RequestMoveAlongPath(HighlightedPath);
}

void AGDPlayerPawn::RequestAttack(AGDUnit* AttackingUnit, AGDUnit* EnemyUnit) const
{
	AttackingUnit->SetActorRotation(
		UKismetMathLibrary::FindLookAtRotation(AttackingUnit->GetActorLocation(),
		                                       EnemyUnit->GetActorLocation()));
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
		SelectedTile = HoveringTile;
		SelectedTile->HandleClicked();
		if (AGDUnit* Unit = Cast<AGDUnit>(SelectedTile->GetTileElement()))
		{
			HighlightUnitMovementRange(Unit);
		}
	}
}

void AGDPlayerPawn::UnselectTile()
{
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

		UnselectTile();
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

void AGDPlayerPawn::FillHighlightedPath(const TArray<AGDTile*>& Path)
{
	HighlightedPath = Path;

	HighlightedPath.RemoveAt(0); //First tile is the one unit is on, so it can be removed

	for (const auto& Tile : HighlightedPath)
	{
		Tile->Highlight(true);
	}
}

void AGDPlayerPawn::EmptyHighlightedPath()
{
	for (const auto& Tile : HighlightedPath)
	{
		Tile->Highlight(false);
	}

	HighlightedPath.Empty();
}
