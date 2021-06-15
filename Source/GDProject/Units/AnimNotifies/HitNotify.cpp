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
		else
		{
			if (AttackingUnit->AttackedEnemy)
			{
				AttackingUnit->AttackedEnemy->AddToActiveUnits();
				AttackingUnit->AttackedEnemy->PlayAnimation(AttackingUnit->AttackedEnemy->ImpactAnimation);
			}
		}
	}
}
