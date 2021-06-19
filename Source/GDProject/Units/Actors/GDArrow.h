// © 2021 Alessandro Arena, Mattia De Prisco, Nunzio Lopardo All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameFramework/ProjectileMovementComponent.h"

#include "GDArrow.generated.h"

class AGDUnit;

UCLASS()
class GDPROJECT_API AGDArrow : public AActor
{
	GENERATED_BODY()

public:
	AGDArrow();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* StaticMeshComponent;

	UPROPERTY(VisibleAnywhere)
	UProjectileMovementComponent* ProjectileMovementComponent;

	UPROPERTY()
	AGDUnit* OwnerUnit;

	UFUNCTION(BlueprintImplementableEvent)
	void Fire(const FVector& Target);

	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	virtual void FireInDirection(const FVector& TargetLocation);

	UFUNCTION()
	void OnComponentHit(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	                    int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};
