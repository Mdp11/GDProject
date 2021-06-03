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


void AGDPlayerPawn::HandleTilesHovering()
{
	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		FVector Start, Dir;
		PC->DeprojectMousePositionToWorld(Start, Dir);

		const FVector End = Start + Dir * 8000.f;

		if (AGDTile* TargetTile = TraceForTile(Start, End, false))
		{
			if (TargetTile != HoveringTile)
			{
				EmptyHighlightedPath();

				if (HoveringTile)
				{
					HoveringTile->Highlight(false);
				}

				TargetTile->Highlight(true);
				HoveringTile = TargetTile;
				if (SelectedTile)
				{
					if (AGDUnit* Unit = Cast<AGDUnit>(SelectedTile->GetTileElement()))
					{
						EmptyHighlightedPath();

						if (TargetTile->IsTraversable())
						{
							if (TargetTile->IsOccupied())
							{
								// ???
							}
							else
							{
								AGDGrid* Grid = TargetTile->GetGrid();
								AGDTile* StartTile = IGDTileElement::Execute_GetTile(Unit);

								FillHighlightedPath(Grid->ComputePathBetweenTiles(StartTile, TargetTile));
							}
						}
					}
				}
			}
		}
		else
		{
			if (HoveringTile)
			{
				HoveringTile->Highlight(false);
				HoveringTile = nullptr;
			}

			if (HighlightedPath.Num() > 0)
			{
				EmptyHighlightedPath();
			}
		}
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
		//Do something based on selected tile type or content
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
