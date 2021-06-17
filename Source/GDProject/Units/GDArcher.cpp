// © 2021 Alessandro Arena, Mattia De Prisco, Nunzio Lopardo All Rights Reserved


#include "GDArcher.h"

AGDArcher::AGDArcher()
{
	bIsInOverWatch = false;

	AttackRange = 5;

	ArrowAttachSocketName = "ArrowSocket";
}

void AGDArcher::BeginPlay()
{
	Super::BeginPlay();
	
	AlternativeAttackAnimation = CriticalAttackAnimation = MissAnimation = BaseAttackAnimation;
}


