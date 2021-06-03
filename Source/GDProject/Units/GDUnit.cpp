// © 2021 Alessandro Arena, Mattia De Prisco, Nunzio Lopardo All Rights Reserved


#include "GDUnit.h"

#include "NavigationSystem.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "GDProject/Components/GDHealthComponent.h"
#include "GDProject/Tiles/GDTile.h"


AGDUnit::AGDUnit()
{
	HealthComponent = CreateDefaultSubobject<UGDHealthComponent>(TEXT("HealthComp"));
	HealthComponent->OnHealthChanged.AddDynamic(this, &AGDUnit::OnHealthChanged);

	ActionPoints = 2;

	MovementRange = 5;

	AttackRange = 5;
	BaseDamage = 10.f;
	HitChance = 90.f;
	CriticalChance = 10.f;
	CriticalChanceAdjuster = 0.f;
	CriticalDamageMultiplier = 2.f;

	bIsDead = false;
}

void AGDUnit::BeginPlay()
{
	Super::BeginPlay();
}

void AGDUnit::OnHealthChanged(UGDHealthComponent* HealthComp, float Health, float HealthDelta,
                              const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
	UE_LOG(LogTemp, Warning, TEXT("%s received %f damage!"), *GetName(), HealthDelta);
	if (!bIsDead && Health <= 0.f)
	{
		Die();
	}
}

void AGDUnit::Die()
{
	UE_LOG(LogTemp, Warning, TEXT("%s died!"), *GetName());
	bIsDead = true;
	SetLifeSpan(3.f);

	CurrentTile->SetTileElement(nullptr);
}

AGDTile* AGDUnit::GetTile_Implementation()
{
	return CurrentTile;
}

void AGDUnit::SetTile_Implementation(AGDTile* Tile)
{
	if (!Tile)
	{
		return;
	}

	CurrentTile->SetTileElement(nullptr);
	CurrentTile = Tile;
	CurrentTile->SetTileElement(this);
}

bool AGDUnit::CanBeSelected_Implementation()
{
	return true;
}

bool AGDUnit::CanMove_Implementation()
{
	return true;
}

void AGDUnit::RequestMoveTo(AGDTile* TargetTile)
{
	if (!TargetTile)
	{
		return;
	}

	FVector Destination = TargetTile->GetActorLocation();
	Destination.Z = GetActorLocation().Z;

	UAIBlueprintHelperLibrary::SimpleMoveToLocation(GetController(), Destination);
	SetTile_Implementation(TargetTile);
}

void AGDUnit::Attack(AGDUnit* Enemy)
{
	if (!Enemy)
	{
		return;
	}

	const bool Miss = FMath::FRandRange(0.f, 100.f) > HitChance;
	if (Miss)
	{
		UE_LOG(LogTemp, Warning, TEXT("Missed!"));
	}
	else
	{
		float Damage = BaseDamage;
		const bool CriticalHit = FMath::FRandRange(0.f, 100.f) <= CriticalChance + CriticalChanceAdjuster;
		if (!CriticalHit)
		{
			CriticalChanceAdjuster += CriticalChance;
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Critical hit!"));
			CriticalChanceAdjuster = 0.f;
			Damage *= CriticalDamageMultiplier;
		}
		Enemy->TakeDamage(Damage, FDamageEvent{}, GetController(), this);
	}
}
