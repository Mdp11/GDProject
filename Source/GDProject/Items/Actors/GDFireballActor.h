// © 2021 Alessandro Arena, Mattia De Prisco, Nunzio Lopardo All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GDFireballActor.generated.h"

UCLASS()
class GDPROJECT_API AGDFireballActor : public AActor
{
	GENERATED_BODY()

public:
	AGDFireballActor();

	void Initialize(const FVector& Target, const TSet<class AGDUnit*>& Units);
protected:
	UPROPERTY()
	USceneComponent* SceneComponent;

	UPROPERTY()
	UParticleSystemComponent* FireballEffect;

	UPROPERTY()
	UParticleSystem* ExplosionEffect;

	UPROPERTY()
	FVector TargetLocation;

	UPROPERTY()
	class AGDPlayerPawn* PlayerPawn;

	UPROPERTY()
	TSet<class AGDUnit*> UnitsToDamage;

	virtual void BeginPlay() override;

	float Damage;

public:
	virtual void Tick(float DeltaTime) override;
};
