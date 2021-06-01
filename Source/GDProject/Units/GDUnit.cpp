// © 2021 Alessandro Arena, Mattia De Prisco, Nunzio Lopardo All Rights Reserved


#include "GDUnit.h"

#include "GDProject/Components/GDHealthComponent.h"


AGDUnit::AGDUnit()
{
	HealthComponent = CreateDefaultSubobject<UGDHealthComponent>(TEXT("HealthComp"));
	HealthComponent->OnHealthChanged.AddDynamic(this, &AGDUnit::OnHealthChanged);
	
	ActionPoints = 2;
	
	MovementRange = 5;
	
	AttackRange = 5;
	BaseDamage = 10.f;
	HitChance = 90.f;
	CriticalChance = 20.f;
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
	if(!bIsDead && Health <= 0.f)
	{
		Die();
	}
}

void AGDUnit::Die()
{
	bIsDead = true;
	SetLifeSpan(3.f);
}

AGDTile* AGDUnit::GetTile_Implementation()
{
	return CurrentTile;
}

void AGDUnit::SetTile_Implementation(AGDTile* Tile)
{
	CurrentTile = Tile;
}

bool AGDUnit::CanBeSelected_Implementation()
{
	return true;
}

bool AGDUnit::CanMove_Implementation()
{
	return true;
}

void AGDUnit::RequestMoveTo(UObject* TargetTile)
{
	return;
}
