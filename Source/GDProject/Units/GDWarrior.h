// © 2021 Alessandro Arena, Mattia De Prisco, Nunzio Lopardo All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "GDUnit.h"
#include "GDWarrior.generated.h"

/**
 * 
 */
UCLASS()
class GDPROJECT_API AGDWarrior : public AGDUnit
{
	GENERATED_BODY()

public:
	AGDWarrior();

protected:
	UPROPERTY(EditDefaultsOnly, Category="Special")
	float DefenceBonus;

	UPROPERTY(BlueprintReadOnly)
	bool bIsInGuard;

	UFUNCTION(BlueprintCallable)
	virtual void UseSpecial() override;

	virtual void RemoveGuard();
	
	virtual void BeginPlay() override;

	virtual void OnHealthChanged(UGDHealthComponent* HealthComp, float Health, float HealthDelta, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser) override;
};
