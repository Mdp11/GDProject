// © 2021 Alessandro Arena, Mattia De Prisco, Nunzio Lopardo All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "GDAIControllerBase.generated.h"

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
	
	virtual TArray<AGDUnit*> GetEnemiesSortedByDistance() const;

	virtual void ComputeActions();

	virtual bool ShouldUseSpecial();

	virtual void UseSpecial();

	virtual void Attack();

	virtual void Move();
	
private:
	void SetControlledUnit();
};
