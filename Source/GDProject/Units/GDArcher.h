// © 2021 Alessandro Arena, Mattia De Prisco, Nunzio Lopardo All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "GDUnit.h"
#include "GDArcher.generated.h"

class AGDBow;
class AGDArrow;

UCLASS()
class GDPROJECT_API AGDArcher : public AGDUnit
{
	GENERATED_BODY()

	friend class UArrowNotify;
	friend class UGDBowStretchNotify;

public:
	AGDArcher();

protected:
	UPROPERTY(BlueprintReadOnly)
	bool bIsInOverWatch;
	
	UPROPERTY(EditDefaultsOnly, Category="Weapons")
	TSubclassOf<AGDBow> BowClass;

	UPROPERTY(EditDefaultsOnly, Category="Weapons")
	TSubclassOf<AGDArrow> ArrowClass;

	UPROPERTY(VisibleDefaultsOnly, Category="Weapons")
	FName BowAttachSocketName;

	UPROPERTY(VisibleDefaultsOnly, Category="Weapons")
	FName ArrowAttachSocketName;

	UPROPERTY(VisibleDefaultsOnly, Category="Weapons")
	FName CablesAttachSocketName;

	UPROPERTY(EditDefaultsOnly, Category="Animation")
	UAnimMontage* MeleeAttackAnimation;

	UPROPERTY(EditDefaultsOnly, Category="Animation")
	UAnimMontage* OverWatchShootAnimation;

	TSet<AGDTile*> OverWatchingTiles;
	
	TSet<AGDTile*> HighlightedOverWatchingTiles;

	bool bCriticalHit;

	bool bMiss;

	UFUNCTION(BlueprintCallable)
	void HighlightOverWatchingTiles();

	UFUNCTION(BlueprintCallable)
	void ResetHighlightedOverWatchingTiles();

	virtual bool IsCriticalHit() override;

	virtual bool Miss() override;

	virtual void Attack() override;

	void GuardTilesInAttackRange();

	virtual void ResetAllHighlightedTiles() override;

	virtual void RemoveSpecial() override;
	
	virtual void UseSpecial() override;
	
	virtual void BeginPlay() override;

public:
	AGDBow* GetBow() const;

private:
	UPROPERTY()
	AGDBow* Bow;

	UPROPERTY()
	AGDArrow* Arrow;

	void SpawnArrow();
	
	void SpawnBow();

	void FireArrow();
};
