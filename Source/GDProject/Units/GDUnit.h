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

private:
	float CriticalChanceAdjuster;

protected:
	UPROPERTY(VisibleAnywhere, Category = "Health")
	UGDHealthComponent* HealthComponent;

	UPROPERTY(EditDefaultsOnly, Category = "Action")
	int ActionPoints;

	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	int MovementRange;

	UPROPERTY(EditDefaultsOnly, Category = "Attack")
	int AttackRange;

	UPROPERTY(EditDefaultsOnly, Category = "Attack")
	float BaseDamage;

	UPROPERTY(EditDefaultsOnly, Category = "Attack")
	float HitChance;

	UPROPERTY(EditDefaultsOnly, Category = "Attack")
	float CriticalChance;

	UPROPERTY(EditDefaultsOnly, Category = "Attack")
	float CriticalDamageMultiplier;

	UPROPERTY()
	AGDTile* CurrentTile;

	bool bIsDead;

	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void OnHealthChanged(UGDHealthComponent* HealthComp, float Health,
	                             float HealthDelta, const class UDamageType* DamageType,
	                             class AController* InstigatedBy, AActor* DamageCauser);

	virtual void Die();

public:
	virtual AGDTile* GetTile_Implementation() override;

	virtual void SetTile_Implementation(AGDTile* Tile) override;

	virtual bool CanBeSelected_Implementation() override;

	virtual bool CanMove_Implementation() override;

	virtual void RequestMoveToTile(AGDTile* TargetTile);

	virtual void RequestMoveAlongPath(const TArray<AGDTile*>& Path);

	virtual void Attack(AGDUnit* Enemy);
};
