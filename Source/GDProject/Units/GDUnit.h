// © 2021 Alessandro Arena, Mattia De Prisco, Nunzio Lopardo All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GDProject/Interfaces/GDTileElement.h"

#include "GDUnit.generated.h"

class UGDHealthComponent;

UCLASS()
class GDPROJECT_API AGDUnit : public ACharacter, public IGDTileElement
{
	GENERATED_BODY()

public:
	AGDUnit();

protected:

	UPROPERTY(VisibleAnywhere, Category = "Health")
	UGDHealthComponent* HealthComponent;

	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	int WalkableTiles;

	UPROPERTY(EditDefaultsOnly, Category = "Action")
	int ActionPoints;

	bool bIsDead;
	
	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void OnHealthChanged(UGDHealthComponent* HealthComp, float Health,
	                             float HealthDelta, const class UDamageType* DamageType,
	                             class AController* InstigatedBy, AActor* DamageCauser);
	
	virtual void Die();

public:
	virtual UObject* GetTile_Implementation() override;
	
	virtual bool CanBeSelected_Implementation() override;
	
	virtual bool CanMove_Implementation() override;

	virtual void RequestMoveTo(UObject* TargetTile);
};
