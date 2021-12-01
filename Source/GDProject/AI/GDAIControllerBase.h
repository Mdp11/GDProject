// © 2021 Alessandro Arena, Mattia De Prisco, Nunzio Lopardo All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "GDAIControllerBase.generated.h"

class AGDTile;
class AGDUnit;

UCLASS()
class GDPROJECT_API AGDAIControllerBase : public AAIController
{
	GENERATED_BODY()

public:
	virtual void Play();

protected:
	UPROPERTY()
	AGDUnit* ControlledUnit;

	TArray<AGDTile*> AttackableTiles;

	virtual TArray<AGDUnit*> GetEnemiesSortedByDistance() const;

	virtual void ComputeActions();

	virtual bool ShouldUseSpecial();

	virtual void UseSpecial();

	virtual AGDTile* GetAttackTargetTile();

	virtual bool CanAttack();

	virtual bool ShouldAttack();

	virtual void Attack();

	virtual void Move();

	virtual void ComputeAttackableTiles();

private:
	void SetControlledUnit();
};
