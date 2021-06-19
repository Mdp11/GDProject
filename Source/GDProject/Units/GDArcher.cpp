// © 2021 Alessandro Arena, Mattia De Prisco, Nunzio Lopardo All Rights Reserved


#include "GDArcher.h"

#include "Actors/GDArrow.h"

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

void AGDArcher::SpawnArrow()
{
	FActorSpawnParameters SpawnParameters;
	SpawnParameters.Owner = this;
	SpawnParameters.Instigator = GetInstigator();
	SpawnParameters.SpawnCollisionHandlingOverride =
		ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	Arrow = GetWorld()->SpawnActor<AGDArrow>(ArrowClass, SpawnParameters);
	if (Arrow)
	{
		Arrow->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale,
		                         ArrowAttachSocketName);
	}
}

void AGDArcher::FireArrow() const
{
	Arrow->FireInDirection(AttackedEnemy->GetActorLocation());
}
