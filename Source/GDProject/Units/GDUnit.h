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

public:
	AGDUnit();

protected:
	UPROPERTY(VisibleAnywhere, Category = "Health")
	UGDHealthComponent* HealthComponent;

	UPROPERTY(EditDefaultsOnly, Category = "Material")
	UMaterial* ActiveMaterial;

	UPROPERTY(EditDefaultsOnly, Category = "Material")
	UMaterial* InactiveMaterial;

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
	bool bIsWalking;

	UPROPERTY(EditDefaultsOnly, Category="Special")
	TSubclassOf<UUserWidget> SpecialWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category="Animation")
	UAnimMontage* AttackAnimation;

	UPROPERTY(EditDefaultsOnly, Category="Animation")
	UAnimMontage* AlternativeAttackAnimation;

	UPROPERTY(EditDefaultsOnly, Category="Animation")
	UAnimMontage* MissAnimation;

	UPROPERTY(EditDefaultsOnly, Category="Animation")
	UAnimMontage* PowerupAnimation;

	UPROPERTY(BlueprintReadOnly)
	AGDUnit* AttackedEnemy;

	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void OnHealthChanged(UGDHealthComponent* HealthComp, float Health,
	                             float HealthDelta, const class UDamageType* DamageType,
	                             class AController* InstigatedBy, AActor* DamageCauser);

	UFUNCTION(BlueprintCallable)
	virtual void UseSpecial() PURE_VIRTUAL(AGDUnit::Special);

	virtual void RemoveSpecial();

	virtual void Die();

	void PerformMove(float DeltaTime);

	void StopMove();

	void DecreaseActionPointsBy(const int Value);

	UFUNCTION(BlueprintCallable)
	void Powerup();

	void UpdateTransparency() const;

public:
	virtual void Tick(float DeltaTime) override;

	virtual AGDTile* GetTile_Implementation() override;

	virtual void SetTile_Implementation(AGDTile* Tile) override;

	virtual bool CanBeSelected_Implementation() override;

	virtual bool CanMove_Implementation() override;

	virtual void Select_Implementation() override;

	virtual void Deselect_Implementation() override;

	virtual bool RequestMove();

	float GetDefence() const;
	
	UFUNCTION(BlueprintCallable)
	virtual bool Attack(AGDUnit* Enemy, bool bIgnoreActionPoints = false);

	int GetMovementRange() const;

	UFUNCTION(BlueprintCallable)
	int GetActionPoints() const;

	int GetAttackRange() const;

	UFUNCTION(BlueprintCallable)
	void ResetActionPoints();

	void HighlightAction(AGDTile* TargetTile);
	
	void PerformAction(AGDTile* TargetTile);

	void OnActionFinished() const;
	
	void ResetAllHighlightedTiles();

	void ResetHighlightedActionTiles();

private:
	float CriticalChanceAdjuster;

	TSet<AGDTile*> HighlightedTilesInRange;

	UPROPERTY()
	AGDTile* HighlightedEnemyTile;

	TArray<AGDTile*> MovementPath;

	void HighlightMovementRange();

	void HighlightEnemiesInAttackRange();


	template <typename T>
	void StoreAndHighlightedCurrentComputedPath(T&& Path)
	{
		MovementPath = Forward<T>(Path);

		for (const auto& Tile : MovementPath)
		{
			Tile->Highlight(true);
		}
	}
};
