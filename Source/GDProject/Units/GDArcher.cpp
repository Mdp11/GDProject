// © 2021 Alessandro Arena, Mattia De Prisco, Nunzio Lopardo All Rights Reserved


#include "GDArcher.h"

#include "Actors/GDArrow.h"
#include "Actors/GDBow.h"
#include "GDProject/Tiles/GDGrid.h"
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
		                                : bIsInOverWatch
		                                ? OverWatchShootAnimation
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

void AGDArcher::GuardTilesInAttackRange()
{
	for (const auto& TileRow : CurrentTile->GetGrid()->GetTilesGrid())
	{
		for (const auto& Tile : TileRow)
		{
			if (IsTileInAttackRange(Tile))
			{
				GuardingTiles.Add(Tile);
				Tile->AddGuardingUnit(this);
			}
		}
	}
}

void AGDArcher::RemoveSpecial()
{
	bIsInOverWatch = false;

	if(Arrow)
	{
		Arrow->SetLifeSpan(0.01f);
		Arrow = nullptr;
	}
	
	for (auto& Tile : GuardingTiles)
	{
		Tile->RemoveGuardingUnit(this);
	}
}

void AGDArcher::UseSpecial()
{
	bIsInOverWatch = true;

	GuardTilesInAttackRange();
}

void AGDArcher::SpawnBow()
{
	FActorSpawnParameters SpawnParameters;
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	Bow = GetWorld()->SpawnActor<AGDBow>(BowClass, SpawnParameters);
	if (Bow)
	{
		Bow->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale,
		                       BowAttachSocketName);
		Bow->SetOwner(this);
	}
}

void AGDArcher::BeginPlay()
{
	Super::BeginPlay();

	SpawnBow();
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

void AGDArcher::FireArrow()
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
		TargetLocation.Y += FMath::RandRange(-5.f, 0.f);
		TargetLocation.Z += 15.f;
	}
	else
	{
		TargetLocation = AttackedEnemy->GetMesh()->GetBoneLocation("Spine1", EBoneSpaces::WorldSpace);
		TargetLocation.Y += FMath::RandRange(-10.f, 0.f);
		TargetLocation.Z += FMath::RandRange(-10.f, 10.f);
	}
	Arrow->FireInDirection(TargetLocation);
	
	Arrow = nullptr;
}
