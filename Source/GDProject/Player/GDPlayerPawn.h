// © 2021 Alessandro Arena, Mattia De Prisco, Nunzio Lopardo All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"

#include "GDPlayerPawn.generated.h"

class AGDTile;

UCLASS()
class GDPROJECT_API AGDPlayerPawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AGDPlayerPawn();

protected:
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite)
	AGDTile* CurrentTileFocus;

	void TriggerClick();

	void TraceForBlock(const FVector& Start, const FVector& End, bool bDrawDebugHelpers);

public:
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
};
