// © 2021 Alessandro Arena, Mattia De Prisco, Nunzio Lopardo All Rights Reserved

#pragma once

#include "CoreMinimal.h"

#include "GameFramework/Character.h"
#include "GDProject/Interfaces/GDTileElement.h"

#include "GDUnit.generated.h"

class UGDHealthComponent;

UENUM(BlueprintType)
enum class EDirection : uint8
{
	North UMETA(DisplayName = "North"),
	West UMETA(DisplayName = "West"),
	South UMETA(DisplayName = "South"),
	East UMETA(DisplayName = "East"),
};

UCLASS(Abstract, Blueprintable)
class GDPROJECT_API AGDUnit : public ACharacter, public IGDTileElement
{
	GENERATED_BODY()

	friend class UGDHitNotify;
	friend class AGDAIControllerBase;
	friend class AGDAIControllerMedium;

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

	UPROPERTY(EditAnywhere, Category = "Combat")
	float HitChance;

	UPROPERTY(EditAnywhere, Category = "Combat")
	float CriticalChance;

	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	float CriticalDamageMultiplier;

	UPROPERTY()
	AGDTile* CurrentTile;

	UPROPERTY(BlueprintReadOnly)
	bool bIsDead;

	bool bWarp;

	bool bIsAIControlled;

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

	UPROPERTY(EditDefaultsOnly, Category="Animation")
	UAnimMontage* DeathAnimation;

	UPROPERTY(EditDefaultsOnly, Category="Animation")
	UAnimMontage* AlternativeDeathAnimation;

	UPROPERTY(EditDefaultsOnly, Category="Movement")
	float WalkingSpeed;

	UPROPERTY(EditDefaultsOnly, Category="Movement")
	float RunningSpeed;

	UPROPERTY()
	UParticleSystem* DeathEffect;

	UPROPERTY()
	UParticleSystemComponent* DeathParticleSystemComponent;

	UPROPERTY()
	AGDUnit* TargetToAttackAfterMove;

	UPROPERTY(BlueprintReadOnly)
	AGDUnit* AttackedEnemy;

	UPROPERTY(BlueprintReadOnly)
	float ComputedDamage;

	UPROPERTY(EditDefaultsOnly)
	float LifeSpanOnDeath;

	EDirection LookDirection;

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

	void PlayDeathEffects();

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

	bool IsTileInAttackRangeFromTile(const AGDTile* SourceTile, AGDTile* TargetTile) const;

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

	void AddToActiveEntities();

	void RemoveFromActiveEntities();

	UFUNCTION(BlueprintCallable)
	void SetDirection(const EDirection NewDirection);

	template <typename Function>
	void PlayAnimationAndDoAction(UAnimMontage* Animation, Function Action)
	{
		AddToActiveEntities();

		const float AnimationDuration = PlayAnimMontage(Animation) + 0.1f;

		FTimerHandle TimerHandle_Animation;
		FTimerDelegate TimerDelegate;
		TimerDelegate.BindLambda(Action);
		GetWorldTimerManager().SetTimer(TimerHandle_Animation, TimerDelegate, AnimationDuration, false);
	}

public:
	virtual void Tick(float DeltaTime) override;

	virtual AGDTile* GetTile_Implementation() const override;

	virtual void SetTile_Implementation(AGDTile* Tile) override;

	virtual bool CanBeSelected_Implementation() const override;

	virtual bool CanMove_Implementation() const override;

	virtual void Select_Implementation() override;

	virtual void Deselect_Implementation() override;

	void HighlightActions(AGDTile* TargetTile);

	void RequestAction(AGDTile* TargetTile);

	bool IsUnitRotating() const;

	void Rotate();

	bool IsEnemy(const AGDUnit* OtherUnit) const;

	UFUNCTION(BlueprintCallable)
	void SetOwningPlayer(int NewOwningPlayer);

	bool IsOwnedByPlayer(int Player) const;

	void OnTurnBegin();

	void OnTurnEnd() const;

	void ApplyDamage();

	float GetDefence() const;

	bool HasFullHealth() const;

	UFUNCTION(BlueprintCallable)
	float GetCurrentHealth() const;

	UFUNCTION(BlueprintCallable)
	float GetMaxHealth() const;

	void AddOutline(const FLinearColor& OutlineColor);

	void RemoveOutline();

private:
	float CriticalChanceAdjuster;

	TSet<AGDTile*> HighlightedTilesInShortRange;

	TSet<AGDTile*> HighlightedTilesInLongRange;

	TSet<AGDTile*> HighlightedEnemyTilesInRange;

	UPROPERTY()
	AGDTile* HighlightedEnemyTile;

	TArray<AGDTile*> MovementPath;

	void ComputeMovementPath(AGDTile* TargetTile);

	void ComputeAttackPath(AGDTile* TargetTile);

	void ComputeMovementRange();

	void ComputeEnemiesInAttackRange();

	bool IsTileInRangeOfAction(const AGDTile* Tile) const;
};

EDirection GetOppositeDirection(const EDirection Direction);
