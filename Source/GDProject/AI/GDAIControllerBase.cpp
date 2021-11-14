// © 2021 Alessandro Arena, Mattia De Prisco, Nunzio Lopardo All Rights Reserved


#include "GDProject/AI/GDAIControllerBase.h"

#include "EngineUtils.h"
#include "GDProject/Tiles/GDTile.h"
#include "GDProject/Units/GDUnit.h"

void AGDAIControllerBase::Play()
{
	SetControlledUnit();
	
	if (!ControlledUnit)
	{
		UE_LOG(LogTemp, Warning, TEXT("AIController unit not set."))
		return;
	}

	ComputeActions();

	if (ShouldUseSpecial())
	{
		UseSpecial();
	}
	else if (ControlledUnit->HighlightedEnemyTilesInRange.Num() > 0)
	{
		Attack();
	}
	else
	{
		Move();
	}
}

TArray<AGDUnit*> AGDAIControllerBase::GetEnemiesSortedByDistance() const
{
	TArray<AGDUnit*> Enemies;

	for (TActorIterator<AGDUnit> It(GetWorld()); It; ++It)
	{
		AGDUnit* CurrentUnit = *It;
		if (CurrentUnit != ControlledUnit && CurrentUnit->IsEnemy(ControlledUnit) && !CurrentUnit->bIsDead)
		{
			Enemies.Push(CurrentUnit);
		}
	}

	Algo::Sort(Enemies,
	           [CurrentTile = IGDTileElement::Execute_GetTile(ControlledUnit)](const AGDUnit* Lhs, const AGDUnit* Rhs)
	           {
		           return IGDTileElement::Execute_GetTile(Lhs)->GetDistanceFrom(CurrentTile) <
			           IGDTileElement::Execute_GetTile(Rhs)->GetDistanceFrom(CurrentTile);
	           });

	return Enemies;
}

void AGDAIControllerBase::ComputeActions()
{
	ControlledUnit->ComputeMovementRange();
	ControlledUnit->ComputeEnemiesInAttackRange();
}

bool AGDAIControllerBase::ShouldUseSpecial()
{
	return false;
}

void AGDAIControllerBase::UseSpecial()
{
}

void AGDAIControllerBase::Attack()
{
	AGDTile* CurrentTile = IGDTileElement::Execute_GetTile(ControlledUnit);

	AGDUnit* NearestEnemy{nullptr};

	for (const auto& Tile : ControlledUnit->HighlightedEnemyTilesInRange)
	{
		if (NearestEnemy == nullptr || IGDTileElement::Execute_GetTile(NearestEnemy)->GetDistanceFrom(CurrentTile) >
			Tile->
			GetDistanceFrom(CurrentTile))
		{
			NearestEnemy = Cast<AGDUnit>(Tile->GetTileElement());
		}
	}

	if (NearestEnemy)
	{
		AGDTile* TargetEnemyTile = IGDTileElement::Execute_GetTile(NearestEnemy);
		ControlledUnit->ComputeAttackPath(TargetEnemyTile);
		ControlledUnit->RequestAction(TargetEnemyTile);
	}
}

void AGDAIControllerBase::Move()
{
	auto Enemies = GetEnemiesSortedByDistance();
	AGDTile* TargetTile{nullptr};

	for (const auto& Tile : ControlledUnit->HighlightedTilesInLongRange)
	{
		if (TargetTile == nullptr || TargetTile->GetDistanceFrom(IGDTileElement::Execute_GetTile(Enemies[0])) > Tile->
			GetDistanceFrom(IGDTileElement::Execute_GetTile(Enemies[0])))
		{
			TargetTile = Tile;
		}
	}
	ControlledUnit->ComputeMovementPath(TargetTile);
	ControlledUnit->RequestAction(TargetTile);
}

void AGDAIControllerBase::SetControlledUnit()
{
	if (!ControlledUnit)
	{
		ControlledUnit = Cast<AGDUnit>(GetPawn());
	}
}
