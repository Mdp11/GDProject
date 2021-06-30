// © 2021 Alessandro Arena, Mattia De Prisco, Nunzio Lopardo All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "GDItemBase.h"
#include "GDHealthPotion.generated.h"

UCLASS()
class GDPROJECT_API UGDHealthPotion : public UGDItemBase
{
	GENERATED_BODY()

public:
	UGDHealthPotion();

	virtual void Use(AGDTile* TargetTile) override;

protected:

	UPROPERTY(EditDefaultsOnly)
	float HealValue;

	UPROPERTY()
	UParticleSystem* HealEffect;

	virtual bool CanBeUsed(AGDTile* TargetTile) override;
};
