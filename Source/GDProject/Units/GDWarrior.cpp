// © 2021 Alessandro Arena, Mattia De Prisco, Nunzio Lopardo All Rights Reserved


#include "GDWarrior.h"

AGDWarrior::AGDWarrior()
{
	bIsInGuard = false;

	DefenceBonus = 1;

	AttackRange = 1;
}

void AGDWarrior::UseSpecial()
{
	if (GetActionPoints() >= 1)
	{
		bIsInGuard = true;
		Defence += DefenceBonus;

		DecreaseActionPointsBy(MaxActionPoints);

		ResetAllHighlightedTiles();
		OnActionFinished();
	}
}

void AGDWarrior::RemoveSpecial()
{
	bIsInGuard = false;
	Defence = BaseDefence;
}

void AGDWarrior::BeginPlay()
{
	Super::BeginPlay();

	BaseDefence = Defence;
}

void AGDWarrior::OnHealthChanged(UGDHealthComponent* HealthComp, float Health, float HealthDelta,
                                 const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
	UE_LOG(LogTemp, Warning, TEXT("%s received %f damage and now has %f!"), *GetName(), HealthDelta, Health);

	if (!bIsDead && Health <= 0.f)
	{
		Die();
	}
	else if (bIsInGuard)
	{
		PlayAnimationAndDoAction(GuardImpactAnimation, [&, EnemyToAttack = Cast<AGDUnit>(DamageCauser)]()
		{
			RequestAttack(EnemyToAttack, true);
		});
	}
	else
	{
		PlayAnimationAndDoAction(ImpactAnimation, [&]()
		{
			OnActionFinished();
		});
	}
}
