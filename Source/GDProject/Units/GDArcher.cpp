// © 2021 Alessandro Arena, Mattia De Prisco, Nunzio Lopardo All Rights Reserved


#include "GDArcher.h"

#include "Actors/GDArrow.h"
#include "Actors/GDBow.h"
#include "GDProject/Tiles/GDTile.h"

AGDArcher::AGDArcher()
{
	bIsInOverWatch = false;

	AttackRange = 5;

	BowAttachSocketName = "BowSocket";
	ArrowAttachSocketName = "ArrowSocket";
	CablesAttachSocketName = "CablesSocket";
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

void AGDArcher::Attack()
{
	ComputedDamage = 0.f;
	UAnimMontage* AttackAnimation = CurrentTile->GetDistanceFrom(IGDTileElement::Execute_GetTile(AttackedEnemy)) <= 1
		                                ? MeleeAttackAnimation
		                                : BaseAttackAnimation;

	if (!Miss())
	{
		ComputedDamage = BaseDamage + CurrentTile->GetAttackModifier();
		if (IsCriticalHit())
		{
			UE_LOG(LogTemp, Warning, TEXT("Critical hit!"));
			ComputedDamage *= CriticalDamageMultiplier;
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Missed!"));
	}

	PlayAnimationAndDoAction(AttackAnimation, [&]() { OnActionFinished(); });
}

void AGDArcher::Die()
{
	Super::Die();

	Bow->SetLifeSpan(LifeSpanOnDeath);
}

void AGDArcher::BeginPlay()
{
	Super::BeginPlay();

	AlternativeAttackAnimation = CriticalAttackAnimation = MissAnimation = BaseAttackAnimation;

	FActorSpawnParameters SpawnParameters;
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	
	Bow = GetWorld()->SpawnActor<AGDBow>(BowClass, SpawnParameters);
	if(Bow)
	{
		Bow->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, BowAttachSocketName);
		Bow->SetOwner(this);
	}
}

AGDBow* AGDArcher::GetBow() const
{
	return Bow;
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
	if (bMiss)
	{
		TargetLocation = AttackedEnemy->GetActorLocation();
		TargetLocation.Z += FMath::RandRange(150.f, 300.f);
	}
	else if (bCriticalHit)
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
