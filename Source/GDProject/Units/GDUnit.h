// © 2021 Alessandro Arena, Mattia De Prisco, Nunzio Lopardo All Rights Reserved

#pragma once

#include "CoreMinimal.h"

#include "GameFramework/Character.h"
#include "GDProject/Interfaces/GDTileElement.h"

#include "GDUnit.generated.h"

class UGDHealthComponent;

UCLASS(Abstract, Blueprintable)
class GDPROJECT_API AGDUnit : public ACharacter, public IGDTileElement
{
	GENERATED_BODY()

	friend class UGDHitNotify;

public:
	AGDUnit();
	void CheckAnimations();

protected:
	UPROPERTY()
	USkeletalMeshComponent* OutlineComponent;

	UPROPERTY()
	UMaterialInstance* OutlineMaterialInstance;

	UPROPERTY(VisibleAnywhere, Category = "Health")
	UGDHealthComponent* HealthComponent;

	UPROPERTY(EditDefaultsOnly, Category = "Action")
	int MaxActionPoints;

	UPROPERTY(EditDefaultsOnly, Category = "Action")
	int CurrentActionPoints;

	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	int MovementRange;

	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	int AttackRange;

	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	float BaseDamage;

	UPROPERTY(EditDefaultsOnly, Category= "Combat")
	float Defence;

	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	float SideAttackModifier;

	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	float BackAttackModifier;

	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	float HitChance;

	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	float CriticalChance;

	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	float CriticalDamageMultiplier;

	UPROPERTY()
	AGDTile* CurrentTile;

	UPROPERTY(BlueprintReadOnly)
	bool bIsDead;

	UPROPERTY(BlueprintReadOnly)
	bool bMoveRequested;

	UPROPERTY(BlueprintReadOnly)
	bool bMoveInterrupted;

	UPROPERTY(BlueprintReadOnly)
	bool bRotationRequested;

	UPROPERTY(BlueprintReadOnly)
	bool bIsWalking;

	UPROPERTY(EditDefaultsOnly, Category="Animation")
	UAnimMontage* BaseAttackAnimation;

	UPROPERTY(EditDefaultsOnly, Category="Animation")
	UAnimMontage* AlternativeAttackAnimation;

	UPROPERTY(EditDefaultsOnly, Category="Animation")
	UAnimMontage* CriticalAttackAnimation;

	UPROPERTY(EditDefaultsOnly, Category="Animation")
	UAnimMontage* MissAnimation;

	UPROPERTY(EditDefaultsOnly, Category="Animation")
	UAnimMontage* ImpactAnimation;

	UPROPERTY(EditDefaultsOnly, Category="Animation")
	UAnimMontage* PowerUpAnimation;

	UPROPERTY()
	AGDUnit* TargetToAttackAfterMove;

	UPROPERTY(BlueprintReadOnly)
	AGDUnit* AttackedEnemy;

	UPROPERTY(BlueprintReadOnly)
	float ComputedDamage;

	UPROPERTY(EditDefaultsOnly)
	float LifeSpanOnDeath;

	int OwningPlayer;

	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void OnHealthChanged(UGDHealthComponent* HealthComp, float Health,
	                             float HealthDelta, const class UDamageType* DamageType,
	                             class AController* InstigatedBy, AActor* DamageCauser);

	UFUNCTION(BlueprintCallable)
	virtual void UseSpecial() PURE_VIRTUAL(AGDUnit::Special);

	virtual void RemoveSpecial();

	virtual void Die();

	void CheckForGuardingUnits();

	void PerformMove(float DeltaTime);

	void StopMove();

	void PerformRotation(float DeltaTime);

	void DecreaseActionPointsBy(const int Value);

	UFUNCTION(BlueprintCallable)
	virtual void PowerUp();

	virtual bool Miss();

	virtual void Attack();

	virtual void RequestMove();

	virtual bool CanAttackUnit(AGDUnit* Enemy, bool bIgnoreActionPoints) const;

	virtual bool IsTileInAttackRange(AGDTile* Tile) const;

	bool IsTileInAttackRangeFromTile(AGDTile* SourceTile, AGDTile* TargetTile) const;

	virtual bool IsCriticalHit();

	UFUNCTION(BlueprintCallable)
	virtual void RequestAttack(AGDUnit* Enemy, bool bIgnoreActionPoints = false);

	int GetMovementRange() const;

	UFUNCTION(BlueprintCallable)
	int GetActionPoints() const;

	int GetAttackRange() const;

	UFUNCTION(BlueprintCallable)
	void ResetActionPoints();

	void RequestMoveAndAttack(AGDUnit* Enemy);

	virtual void OnActionBegin();

	virtual void OnActionFinished();

	void ResetHighlightedTilesInRange();

	void ResetHighlightedActionTiles();

	virtual void ResetAllHighlightedTiles();

	void AddToActiveUnits();

	void RemoveFromActiveUnits();

	template <typename Function>
	void PlayAnimationAndDoAction(UAnimMontage* Animation, Function Action)
	{
		AddToActiveUnits();

		const float AnimationDuration = PlayAnimMontage(Animation) + 0.1f;

		FTimerHandle TimerHandle_Animation;
		FTimerDelegate TimerDelegate;
		TimerDelegate.BindLambda(Action);
		GetWorldTimerManager().SetTimer(TimerHandle_Animation, TimerDelegate, AnimationDuration, false);
	}

public:
	virtual void Tick(float DeltaTime) override;

	virtual AGDTile* GetTile_Implementation() override;

	virtual void SetTile_Implementation(AGDTile* Tile) override;

	virtual bool CanBeSelected_Implementation() override;

	virtual bool CanMove_Implementation() override;

	virtual void Select_Implementation() override;

	virtual void Deselect_Implementation() override;

	void HighlightActions(AGDTile* TargetTile);

	void RequestAction(AGDTile* TargetTile);

	bool IsUnitRotating() const;

	void Rotate();

	bool IsEnemy(AGDUnit* OtherUnit) const;

	UFUNCTION(BlueprintCallable)
	void SetOwningPlayer(int NewOwningPlayer);

	bool IsOwnedByPlayer(int Player) const;

	void OnTurnBegin();

	void OnTurnEnd() const;

	void ApplyDamage();

	float GetDefence() const;

private:
	float CriticalChanceAdjuster;

	TSet<AGDTile*> HighlightedTilesInShortRange;

	TSet<AGDTile*> HighlightedTilesInLongRange;

	TSet<AGDTile*> HighlightedEnemyTilesInRange;

	UPROPERTY()
	AGDTile* HighlightedEnemyTile;

	TArray<AGDTile*> MovementPath;

	void HighlightMovementPath(AGDTile* TargetTile);

	void HighlightAttackPath(AGDTile* TargetTile);

	void HighlightMovementRange();

	void HighlightEnemiesInAttackRange();

	bool IsTileInRangeOfAction(AGDTile* Tile) const;

	void AddOutline(const FLinearColor& OutlineColor);

	void RemoveOutline();
};
