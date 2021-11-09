// © 2021 Alessandro Arena, Mattia De Prisco, Nunzio Lopardo All Rights Reserved


#include "GDProject/AI/GDAIControllerBase.h"

#include "EngineUtils.h"
#include "GDProject/Tiles/GDGrid.h"
#include "GDProject/Tiles/GDTile.h"
#include "GDProject/Units/GDUnit.h"

void AGDAIControllerBase::Play()
{
	AGDUnit* ControlledUnit = Cast<AGDUnit>(GetPawn());

	if (!ControlledUnit)
	{
		return;
	}
	ControlledUnit->AddOutline(FColor::Red);

	AGDTile* CurrentTile = IGDTileElement::Execute_GetTile(ControlledUnit);
	AGDGrid* Grid = CurrentTile->GetGrid();

	ControlledUnit->ComputeMovementRange();
	ControlledUnit->ComputeEnemiesInAttackRange();

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
		ControlledUnit->ComputeMovementPath(TargetEnemyTile);
		ControlledUnit->RequestAction(TargetEnemyTile);
	}
	else
	{
		auto Enemies = GetEnemiesSortedByDistance();
		AGDTile* TargetTile{nullptr};

		for (const auto& Tile : ControlledUnit->HighlightedTilesInLongRange)
		{
			if (TargetTile == nullptr || TargetTile->GetDistanceFrom(IGDTileElement::Execute_GetTile(Enemies[0])) > Tile->GetDistanceFrom(IGDTileElement::Execute_GetTile(Enemies[0])))
			{
				TargetTile = Tile;
			}
		}
		ControlledUnit->ComputeMovementPath(TargetTile);
		ControlledUnit->RequestAction(TargetTile);
	}

	ControlledUnit->RemoveOutline();
}

TArray<AGDUnit*> AGDAIControllerBase::GetEnemiesSortedByDistance() const
{
	AGDUnit* ControlledUnit = Cast<AGDUnit>(GetPawn());

	if (!ControlledUnit)
	{
		return {};
	}

	TArray<AGDUnit*> Enemies;

	for (TActorIterator<AGDUnit> It(GetWorld()); It; ++It)
	{
		AGDUnit* CurrentUnit = *It;
		if (CurrentUnit != ControlledUnit && CurrentUnit->IsEnemy(ControlledUnit) && !CurrentUnit->bIsDead)
		{
			Enemies.Push(CurrentUnit);
		}
	}

	if (Enemies.Num() == 0)
	{
		return {};
	}

	Algo::Sort(Enemies, [CurrentTile = IGDTileElement::Execute_GetTile(ControlledUnit)](const AGDUnit* Lhs, const AGDUnit* Rhs)
	{
		return IGDTileElement::Execute_GetTile(Lhs)->GetDistanceFrom(CurrentTile) <
			IGDTileElement::Execute_GetTile(Rhs)->GetDistanceFrom(CurrentTile);
	});

	return Enemies;
}
