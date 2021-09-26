// © 2021 Alessandro Arena, Mattia De Prisco, Nunzio Lopardo All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "GDItemBase.h"
#include "GDFireball.generated.h"

/**
 * 
 */
UCLASS()
class GDPROJECT_API UGDFireball : public UGDItemBase
{
	GENERATED_BODY()

public:
	UGDFireball();

	virtual void Use(AGDTile* TargetTile) override;

protected:
	UPROPERTY(EditDefaultsOnly)
	float Damage;

	TSet<AGDTile*> TargetTiles;

	void ResetTargetTiles();

	virtual void HighlightAffectedTiles(AGDTile* TargetTile, bool NonUsable) override;

	virtual void OnDeselect() override;
};
