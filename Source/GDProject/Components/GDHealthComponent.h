// © 2021 Alessandro Arena, Mattia De Prisco, Nunzio Lopardo All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GDHealthComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_SixParams(FOnHealthChangedSignature, UGDHealthComponent*, HealthComp, float, Health,
                                             float, HealthDelta, const class UDamageType*, DamageType,
                                             class AController*, InstigatedBy, AActor*, DamageCauser);


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class GDPROJECT_API UGDHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UGDHealthComponent();

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Health")
	float DefaultHealth;

	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void OnTakeAnyDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType,
	                             class AController* InstigatedBy, AActor* DamageCauser);
	
private:
	float CurrentHealth;


public:
	FOnHealthChangedSignature OnHealthChanged;
};
