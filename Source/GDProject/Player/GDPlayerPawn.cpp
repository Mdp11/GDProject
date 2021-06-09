// © 2021 Alessandro Arena, Mattia De Prisco, Nunzio Lopardo All Rights Reserved


#include "GDPlayerPawn.h"

#include "DrawDebugHelpers.h"
#include "GDProject/Tiles/GDGrid.h"
#include "GDProject/Tiles/GDTile.h"
#include "GDProject/Units/GDUnit.h"

AGDPlayerPawn::AGDPlayerPawn()
{
	PrimaryActorTick.bCanEverTick = true;
	AutoPossessPlayer = EAutoReceiveInput::Player0;

	bWaitForActionCompletion = false;
}

void AGDPlayerPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if(!bWaitForActionCompletion)
	{
		HandleTilesHovering();
	}
}

void AGDPlayerPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction("TriggerClick", EInputEvent::IE_Pressed, this, &AGDPlayerPawn::TriggerClick);
	PlayerInputComponent->BindAction("RemoveSelection", EInputEvent::IE_Pressed, this, &AGDPlayerPawn::UnselectTile);
}

void AGDPlayerPawn::HandleTilesHovering()
{
	if (AGDTile* TargetTile = GetTileUnderMouse())
	{
		if (TargetTile != HoveringTile)
		{
			UpdateHoveringTile(TargetTile);

			if (SelectedTile)
			{
				if (AGDUnit* Unit = Cast<AGDUnit>(SelectedTile->GetTileElement()))
				{
					Unit->HighlightAction(HoveringTile);
				}
			}
		}
	}
	else
	{
		DisableHoveringVisuals();
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

void AGDPlayerPawn::UpdateHoveringTile(AGDTile* NewHoveringTile)
{
	if (HoveringTile)
	{
		HoveringTile->Highlight(false);
	}

	NewHoveringTile->Highlight(true);
	HoveringTile = NewHoveringTile;
}

void AGDPlayerPawn::DisableHoveringVisuals()
{
	if (HoveringTile)
	{
		HoveringTile->Highlight(false);
		HoveringTile = nullptr;
	}
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
		if (SelectedTile != HoveringTile && SelectedTile->IsOccupied())
		{
			if (AGDUnit* Unit = Cast<AGDUnit>(SelectedTile->GetTileElement()))
			{
				Unit->PerformAction(HoveringTile);
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

		IGDTileElement::Execute_Select(HoveringTile->GetTileElement());
	}
}

void AGDPlayerPawn::UnselectTile()
{
	if (SelectedTile->IsOccupied())
	{
		IGDTileElement::Execute_Deselect(SelectedTile->GetTileElement());
	}

	SelectedTile->HandleClicked();
	SelectedTile = nullptr;
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
