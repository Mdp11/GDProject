// © 2021 Alessandro Arena, Mattia De Prisco, Nunzio Lopardo All Rights Reserved


#include "HitNotify.h"

#include "GDProject/Units/GDUnit.h"
#include "GDProject/Units/GDWarrior.h"

void UHitNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	AGDUnit* AttackingUnit = Cast<AGDUnit>(MeshComp->GetOwner());

	if (AttackingUnit)
	{
		if (bMiss)
		{
			AGDWarrior* Warrior = Cast<AGDWarrior>(AttackingUnit->AttackedEnemy);
			if (Warrior && Warrior->bIsInGuard)
			{
				Warrior->RequestAttack(AttackingUnit, true);
			}
		}
		else if (bApplyDamage)
		{
			AttackingUnit->ApplyDamage();
		}
		else if (AttackingUnit->AttackedEnemy)
		{
			UAnimMontage* ImpactAnimMontage = AttackingUnit->AttackedEnemy->ImpactAnimation;

			AGDWarrior* EnemyWarrior = Cast<AGDWarrior>(AttackingUnit->AttackedEnemy);
			if (EnemyWarrior && EnemyWarrior->bIsInGuard)
			{
				ImpactAnimMontage = EnemyWarrior->GuardImpactAnimation;
			}

			AttackingUnit->AttackedEnemy->PlayAnimationAndDoAction(
				ImpactAnimMontage, [Unit = AttackingUnit->AttackedEnemy]()
				{
					Unit->OnActionFinished();
				});
		}
	}
}
