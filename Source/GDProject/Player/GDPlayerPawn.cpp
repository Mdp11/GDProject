// © 2021 Alessandro Arena, Mattia De Prisco, Nunzio Lopardo All Rights Reserved


#include "GDPlayerPawn.h"

#include "DrawDebugHelpers.h"
#include "GDProject/Tiles/GDTile.h"

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
		TraceForBlock(Start, End, false);
	}
}

void AGDPlayerPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction("TriggerClick", EInputEvent::IE_Pressed, this, &AGDPlayerPawn::TriggerClick);
}

void AGDPlayerPawn::TriggerClick()
{
	if (CurrentTileFocus)
	{
		CurrentTileFocus->HandleClicked();
	}
}

void AGDPlayerPawn::TraceForBlock(const FVector& Start, const FVector& End, bool bDrawDebugHelpers)
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
		if (CurrentTileFocus != HitTile)
		{
			if (CurrentTileFocus)
			{
				CurrentTileFocus->Highlight(false);
			}
			if (HitTile)
			{
				HitTile->Highlight(true);
			}
			CurrentTileFocus = HitTile;
		}
	}
	else if (CurrentTileFocus)
	{
		CurrentTileFocus->Highlight(false);
		CurrentTileFocus = nullptr;
	}
}
