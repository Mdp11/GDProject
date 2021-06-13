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
	Defence -= DefenceBonus;
}

void AGDWarrior::BeginPlay()
{
	Super::BeginPlay();
}
