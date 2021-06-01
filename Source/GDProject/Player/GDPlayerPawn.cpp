// © 2021 Alessandro Arena, Mattia De Prisco, Nunzio Lopardo All Rights Reserved


#include "GDPlayerPawn.h"

#include "DrawDebugHelpers.h"
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

	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		FVector Start, Dir;
		PC->DeprojectMousePositionToWorld(Start, Dir);

		const FVector End = Start + (Dir * 8000.0f);
		TraceForTileElement(Start, End, true);
		// TraceForTile(Start, End, true);
	}
}

void AGDPlayerPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction("TriggerClick", EInputEvent::IE_Pressed, this, &AGDPlayerPawn::TriggerClick);
}

void AGDPlayerPawn::TriggerClick()
{
	if(SelectedTileElement)
	{
		AGDTile* TileSelectedElementIsOn = IGDTileElement::Execute_GetTile(SelectedTileElement);
		if(SelectedTileElement == HoveringTileElement)
		{
			TileSelectedElementIsOn->HandleClicked();
			SelectedTileElement = nullptr;
		}
		else
		{
			if (APlayerController* PC = Cast<APlayerController>(GetController()))
			{
				FVector Start, Dir;
				PC->DeprojectMousePositionToWorld(Start, Dir);

				const FVector End = Start + (Dir * 8000.0f);
				TraceForTile(Start, End, true);
			}

			if(HoveringTile != TileSelectedElementIsOn)
			{
				AGDUnit* Unit = Cast<AGDUnit>(SelectedTileElement);
				if(Unit)
				{
					UE_LOG(LogTemp, Warning, TEXT("MOVING"));
					Unit->RequestMoveTo(HoveringTile);
				}
			}
		}
	}
	else
	{
		if (HoveringTileElement)
		{
			SelectedTileElement = HoveringTileElement;
			IGDTileElement::Execute_GetTile(SelectedTileElement)->HandleClicked();
		}
	}

}

void AGDPlayerPawn::TraceForTile(const FVector& Start, const FVector& End, bool bDrawDebugHelpers)
{
	FHitResult HitResult;
	GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility);
	if (bDrawDebugHelpers)
	{
		DrawDebugLine(GetWorld(), Start, HitResult.Location, FColor::Red);
		DrawDebugSolidBox(GetWorld(), HitResult.Location, FVector(20.0f), FColor::Red);
	}
	if (HitResult.Actor.IsValid())
	{
		AGDTile* HitTile = Cast<AGDTile>(HitResult.Actor.Get());
		if (HoveringTile != HitTile)
		{
			if (HoveringTile)
			{
				HoveringTile->Highlight(false);
			}
			if (HitTile)
			{
				HitTile->Highlight(true);
			}
			HoveringTile = HitTile;
		}
	}
	else if (HoveringTile)
	{
		HoveringTile->Highlight(false);
		HoveringTile = nullptr;
	}
}

void AGDPlayerPawn::TraceForTileElement(const FVector& Start, const FVector& End, bool bDrawDebugHelpers)
{
	FHitResult HitResult;
	GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Camera);
	if (bDrawDebugHelpers)
	{
		DrawDebugLine(GetWorld(), Start, HitResult.Location, FColor::Red);
		DrawDebugSolidBox(GetWorld(), HitResult.Location, FVector(20.0f), FColor::Red);
	}
	if (HitResult.Actor.IsValid())
	{
		if (HoveringTileElement != HitResult.Actor.Get())
		{
			if (HitResult.Actor->GetClass()->ImplementsInterface(UGDTileElement::StaticClass()))
			{
				UpdateHoveringTileElement(HitResult.Actor.Get());
			}
			else if (HoveringTileElement)
			{
				UpdateHoveringTileElement(nullptr);
			}
		}
	}
	else if (HoveringTileElement)
	{
		UpdateHoveringTileElement(nullptr);
	}
}

void AGDPlayerPawn::UpdateHoveringTileElement(UObject* NewTileElement)
{
	if (HoveringTileElement && HoveringTileElement != SelectedTileElement)
	{
		IGDTileElement::Execute_GetTile(HoveringTileElement)->Highlight(false);
	}

	if (NewTileElement)
	{
		IGDTileElement::Execute_GetTile(NewTileElement)->Highlight(true);
	}

	HoveringTileElement = NewTileElement;
}
