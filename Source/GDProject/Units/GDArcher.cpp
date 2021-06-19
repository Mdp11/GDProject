// © 2021 Alessandro Arena, Mattia De Prisco, Nunzio Lopardo All Rights Reserved


#include "GDArcher.h"

#include "Actors/GDArrow.h"

AGDArcher::AGDArcher()
{
	bIsInOverWatch = false;

	AttackRange = 5;

	ArrowAttachSocketName = "ArrowSocket";
}

bool AGDArcher::IsCriticalHit()
{
	bCriticalHit = Super::IsCriticalHit();

	return bCriticalHit;
}

bool AGDArcher::Miss()
{
	bMiss = Super::Miss();

	return bMiss;
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
	FVector TargetLocation;
	if(bMiss)
	{
		TargetLocation = AttackedEnemy->GetActorLocation();
		TargetLocation.Z += FMath::RandRange(150.f, 300.f);
	}
	else if(bCriticalHit)
	{
		TargetLocation = AttackedEnemy->GetMesh()->GetBoneLocation("Head", EBoneSpaces::WorldSpace);
		TargetLocation.Y += FMath::RandRange(-5.f, 5.f);
		TargetLocation.Z += FMath::RandRange(-5.f, 5.f);
	}
	else
	{
		TargetLocation = AttackedEnemy->GetMesh()->GetBoneLocation("Spine1", EBoneSpaces::WorldSpace);
		TargetLocation.Y += FMath::RandRange(-15.f, 15.f);
		TargetLocation.Z += FMath::RandRange(-20.f, 20.f);
	}
	Arrow->FireInDirection(TargetLocation);
}
