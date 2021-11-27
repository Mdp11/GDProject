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
	else if (AttackableTiles.Num() > 0)
	{
		Attack();
	}
	else
	{
		Move();
	}

	AttackableTiles.Empty();
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
	ComputeAttackableTiles();
}

bool AGDAIControllerBase::ShouldUseSpecial()
{
	return false;
}

void AGDAIControllerBase::UseSpecial()
{
}

AGDTile* AGDAIControllerBase::GetAttackTargetTile()
{
	AGDTile* TargetEnemyTile{nullptr};

	AGDTile* CurrentTile = IGDTileElement::Execute_GetTile(ControlledUnit);

	for (const auto& Tile : AttackableTiles)
	{
		if (TargetEnemyTile == nullptr || TargetEnemyTile->GetDistanceFrom(CurrentTile) > Tile->GetDistanceFrom(
			CurrentTile))
		{
			TargetEnemyTile = Tile;
		}
	}

	return TargetEnemyTile;
}

bool AGDAIControllerBase::CanAttack()
{
	return true;
}

bool AGDAIControllerBase::ShouldAttack()
{
	return true;
}

void AGDAIControllerBase::Attack()
{
	AGDTile* TargetEnemyTile = GetAttackTargetTile();

	ControlledUnit->ComputeAttackPath(TargetEnemyTile);
	ControlledUnit->RequestAction(TargetEnemyTile);
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

void AGDAIControllerBase::ComputeAttackableTiles()
{
	for (const auto& Tile : ControlledUnit->HighlightedEnemyTilesInRange)
	{
		if (ControlledUnit->IsTileInAttackRange(Tile))
		{
			AttackableTiles.Push(Tile);
		}
		else
		{
			ControlledUnit->ComputeAttackPath(Tile);
			if (ControlledUnit->MovementPath.Num() > 0)
			{
				AttackableTiles.Push(Tile);
			}
		}
	}
}

void AGDAIControllerBase::SetControlledUnit()
{
	if (!ControlledUnit)
	{
		ControlledUnit = Cast<AGDUnit>(GetPawn());
	}
}
