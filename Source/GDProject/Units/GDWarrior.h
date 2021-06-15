// © 2021 Alessandro Arena, Mattia De Prisco, Nunzio Lopardo All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "GDUnit.h"
#include "GDWarrior.generated.h"

UCLASS()
class GDPROJECT_API AGDWarrior : public AGDUnit
{
	GENERATED_BODY()

	friend class UHitNotify;

public:
	AGDWarrior();

protected:
	UPROPERTY(EditDefaultsOnly, Category="Special")
	float DefenceBonus;

	float BaseDefence;

	UPROPERTY(BlueprintReadOnly)
	bool bIsInGuard;

	UPROPERTY(EditDefaultsOnly, Category="Animation")
	UAnimMontage* GuardImpactAnimation;

	virtual void UseSpecial() override;

	virtual void RemoveSpecial() override;

	virtual void BeginPlay() override;

	virtual void OnHealthChanged(UGDHealthComponent* HealthComp, float Health, float HealthDelta,
	                             const UDamageType* DamageType, AController* InstigatedBy,
	                             AActor* DamageCauser) override;
};
