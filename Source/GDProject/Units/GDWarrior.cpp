// © 2021 Alessandro Arena, Mattia De Prisco, Nunzio Lopardo All Rights Reserved


#include "GDWarrior.h"

#include "GDProject/Tiles/GDTile.h"

AGDWarrior::AGDWarrior()
{
	bIsInGuard = false;
}

void AGDWarrior::UseSpecial()
{
	bIsInGuard = true;
	Defence += DefenceBonus;
}

void AGDWarrior::RemoveGuard()
{
	bIsInGuard = false;
	Defence -= DefenceBonus;
}

void AGDWarrior::BeginPlay()
{
	Super::BeginPlay();
}

void AGDWarrior::OnHealthChanged(UGDHealthComponent* HealthComp, float Health, float HealthDelta,
                                 const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
	Super::OnHealthChanged(HealthComp, Health, HealthDelta, DamageType, InstigatedBy, DamageCauser);

	if (bIsInGuard && !bIsDead)
	{
		if (AGDUnit* Attacker = Cast<AGDUnit>(DamageCauser))
		{
			if (CurrentTile->GetDistanceFrom(IGDTileElement::Execute_GetTile(Attacker)) == 1)
			{
				Attack(Attacker, true);
			}
		}
	}
}
