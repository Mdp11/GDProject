// © 2021 Alessandro Arena, Mattia De Prisco, Nunzio Lopardo All Rights Reserved


#include "GDPlayerPawn.h"

#include "DrawDebugHelpers.h"
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

		if (AGDTile* Tile = TraceForTile(Start, End, false))
		{
			if (Tile != HoveringTile)
			{
				if (HoveringTile)
				{
					HoveringTile->Highlight(false);
				}
				Tile->Highlight(true);
				HoveringTile = Tile;
			}
		}
		else if (HoveringTile)
		{
			HoveringTile->Highlight(false);
			HoveringTile = nullptr;
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

void AGDPlayerPawn::RequestMove(AGDUnit* Unit, AGDTile* TargetTile) const
{
	Unit->RequestMoveTo(TargetTile);
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
	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		FVector Start, Dir;
		PC->DeprojectMousePositionToWorld(Start, Dir);
		const FVector End = Start + Dir * 8000.0f;
		AGDTile* TargetTile = TraceForTile(Start, End, true);

		if (TargetTile)
		{
			if (TargetTile->IsOccupied())
			{
				if (AGDUnit* OtherUnit = Cast<AGDUnit>(TargetTile->GetTileElement()) /* && OtherUnit is enemy */)
				{
					RequestAttack(Unit, OtherUnit);
				}
			}
			else
			{
				RequestMove(Unit, TargetTile);
			}
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
