// © 2021 Alessandro Arena, Mattia De Prisco, Nunzio Lopardo All Rights Reserved


#include "GDTile.h"

#include "GDGrid.h"
#include "GDTileForest.h"
#include "GDTileRiver.h"
#include "Components/DecalComponent.h"
#include "GDProject/Units/GDUnit.h"

AGDTile::AGDTile()
{
	PrimaryActorTick.bCanEverTick = false;

	DummyRoot = CreateDefaultSubobject<USceneComponent>(TEXT("DummyRoot"));
	RootComponent = DummyRoot;

	TileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TileMesh"));
	TileMesh->SetCastShadow(false);
	TileMesh->SetupAttachment(DummyRoot);

	SelectionDecalComponent = CreateDefaultSubobject<UDecalComponent>(TEXT("SelectionDecalComponent"));
	SelectionDecalComponent->SetupAttachment(TileMesh);

	SelectionDecalComponent->SetHiddenInGame(true);

	InfoDecalComponent = CreateDefaultSubobject<UDecalComponent>(TEXT("InfoDecalComponent"));
	InfoDecalComponent->SetupAttachment(TileMesh);

	InfoDecalComponent->SetHiddenInGame(true);

	bIsActive = false;

	bIsTraversable = true;

	TileElement = nullptr;

	AttackModifier = 0.f;
	DefenceModifier = 0.f;
	HitChanceModifier = 0.f;
	CriticalModifier = 0.f;
	CriticalChanceModifier = 0.f;
}


void AGDTile::SetOwningGrid(AGDGrid* Grid)
{
	OwningGrid = Grid;
}

void AGDTile::Select()
{
	if (!bIsActive)
	{
		if (OwningGrid->SelectedDecalMaterial)
		{
			SelectionDecalComponent->SetMaterial(0, OwningGrid->SelectedDecalMaterial);
			SelectionDecalComponent->SetHiddenInGame(false);
		}

		bIsActive = true;
	}
}

void AGDTile::Deselect()
{
	if (bIsActive)
	{
		bIsActive = false;
		RemoveHighlight();
	}
}

void AGDTile::Highlight(const EHighlightInfo& HighlightInfo) const
{
	if (!bIsActive)
	{
		switch (HighlightInfo)
		{
		case EHighlightInfo::Default:
			if (OwningGrid->HoverDecalMaterial)
			{
				SelectionDecalComponent->SetMaterial(0, OwningGrid->HoverDecalMaterial);
			}
			break;

		case EHighlightInfo::Ally:
			if (OwningGrid->SelectedDecalMaterial)
			{
				SelectionDecalComponent->SetMaterial(0, OwningGrid->SelectedDecalMaterial);
			}
			break;

		case EHighlightInfo::Enemy:
			if (OwningGrid->TargetedEnemyDecalMaterial)
			{
				SelectionDecalComponent->SetMaterial(0, OwningGrid->TargetedEnemyDecalMaterial);
			}
		}

		SelectionDecalComponent->SetHiddenInGame(false);
	}
}

void AGDTile::RemoveHighlight() const
{
	if (!bIsActive)
	{
		SelectionDecalComponent->SetHiddenInGame(true);
	}
}

void AGDTile::SetTileElement(UObject* NewTileElement)
{
	if (NewTileElement && NewTileElement->GetClass()->ImplementsInterface(UGDTileElement::StaticClass()))
	{
		TileElement = NewTileElement;
	}
	else
	{
		TileElement = nullptr;
	}
}

UObject* AGDTile::GetTileElement() const
{
	return TileElement;
}

bool AGDTile::IsOccupied() const
{
	return TileElement != nullptr;
}

bool AGDTile::IsOccupiedByEnemy(AGDUnit* OtherUnit) const
{
	if (AGDUnit* Unit = Cast<AGDUnit>(TileElement))
	{
		return OtherUnit->IsEnemy(Unit);
	}

	return false;
}

bool AGDTile::IsTraversable() const
{
	return bIsTraversable;
}

void AGDTile::SetCoordinates(const FIntPoint& NewCoordinates)
{
	Coordinates = NewCoordinates;
}

FIntPoint AGDTile::GetCoordinates() const
{
	return Coordinates;
}

int AGDTile::GetDistanceFrom(AGDTile* TargetTile) const
{
	const FIntPoint TargetCoordinates = TargetTile->GetCoordinates();
	return FMath::Abs(Coordinates.X - TargetCoordinates.X) + FMath::Abs(Coordinates.Y - TargetCoordinates.Y);
}

TArray<AGDTile*> AGDTile::GetTraversableNeighbours() const
{
	TArray<AGDTile*> Neighbours;

	AGDTile* NeighbourTile = OwningGrid->GetTile({Coordinates.X - 1, Coordinates.Y});
	if (NeighbourTile && NeighbourTile->IsTraversable())
	{
		Neighbours.Add(NeighbourTile);
	}

	NeighbourTile = OwningGrid->GetTile({Coordinates.X + 1, Coordinates.Y});
	if (NeighbourTile && NeighbourTile->IsTraversable())
	{
		Neighbours.Add(NeighbourTile);
	}

	NeighbourTile = OwningGrid->GetTile({Coordinates.X, Coordinates.Y - 1});
	if (NeighbourTile && NeighbourTile->IsTraversable())
	{
		Neighbours.Add(NeighbourTile);
	}

	NeighbourTile = OwningGrid->GetTile({Coordinates.X, Coordinates.Y + 1});
	if (NeighbourTile && NeighbourTile->IsTraversable())
	{
		Neighbours.Add(NeighbourTile);
	}

	return Neighbours;
}

AGDGrid* AGDTile::GetGrid() const
{
	return OwningGrid;
}

void AGDTile::ApplyMovementRangeInfoDecal(const bool bShort) const
{
	if (OwningGrid->ShortMovementDecalMaterial && OwningGrid->LongMovementDecalMaterial)
	{
		InfoDecalComponent->SetHiddenInGame(false);

		UMaterial* MovementDecalMat = bShort
			                              ? OwningGrid->ShortMovementDecalMaterial
			                              : OwningGrid->LongMovementDecalMaterial;

		InfoDecalComponent->SetMaterial(0, MovementDecalMat);
	}
}

void AGDTile::ApplyEnemyInfoDecal() const
{
	if (OwningGrid->EnemyDecalMaterial)
	{
		InfoDecalComponent->SetHiddenInGame(false);
		InfoDecalComponent->SetMaterial(0, OwningGrid->EnemyDecalMaterial);
	}
}

void AGDTile::RemoveInfoDecal() const
{
	InfoDecalComponent->SetHiddenInGame(true);
}

bool AGDTile::IsPathClearTowardsTile(AGDTile* Tile) const
{
	const bool OnSameX = Tile->GetCoordinates().X == GetCoordinates().X;
	const bool OnSameY = Tile->GetCoordinates().Y == GetCoordinates().Y;

	if (OnSameX)
	{
		for (int i = FMath::Min(Tile->GetCoordinates().Y, GetCoordinates().Y) + 1; i < FMath::Max(
			     Tile->GetCoordinates().Y, GetCoordinates().Y); ++i)
		{
			AGDTile* CurrentTile = GetGrid()->GetTile({GetCoordinates().X, i});
			if (CurrentTile->IsOccupied() || CurrentTile->bObstructVisual)
			{
				return false;
			}
		}
		return true;
	}

	if (OnSameY)
	{
		for (int i = FMath::Min(Tile->GetCoordinates().X, GetCoordinates().X) + 1; i < FMath::Max(
			     Tile->GetCoordinates().X, GetCoordinates().X); ++i)
		{
			AGDTile* CurrentTile = GetGrid()->GetTile({i, GetCoordinates().Y});
			if (CurrentTile->IsOccupied() || CurrentTile->bObstructVisual)
			{
				return false;
			}
		}
		return true;
	}

	return false;
}

TArray<AGDTile*> AGDTile::GetTilesInDirection(const EDirection Direction, const int Num) const
{
	if (Num == 0)
	{
		return {};
	}
	
	TArray<AGDTile*> Tiles;
	AGDTile* Tile = nullptr;

	switch (Direction)
	{
	case EDirection::North:
		Tile = OwningGrid->GetTile({Coordinates.X, Coordinates.Y + 1});
		break;

	case EDirection::East:
		Tile = OwningGrid->GetTile({Coordinates.X - 1, Coordinates.Y});
		break;

	case EDirection::South:
		Tile = OwningGrid->GetTile({Coordinates.X, Coordinates.Y - 1});
		break;

	case EDirection::West:
		Tile = OwningGrid->GetTile({Coordinates.X + 1, Coordinates.Y});
		break;
	}

	if (Tile)
	{
		Tiles.Add(Tile);
		Tiles.Append(Tile->GetTilesInDirection(Direction, Num - 1));
	}

	return Tiles;
}

TSet<AGDUnit*> AGDTile::GetGuardingUnits() const
{
	return GuardingUnits;
}

void AGDTile::AddGuardingUnit(AGDUnit* Unit)
{
	GuardingUnits.Add(Unit);
}

void AGDTile::RemoveGuardingUnit(AGDUnit* Unit)
{
	GuardingUnits.Remove(Unit);
}

bool AGDTile::HasGuardingUnits() const
{
	return GuardingUnits.Num() > 0;
}

void AGDTile::ChangeInForest()
{
	AGDTile* ClassGetter = NewObject<AGDTileForest>(AGDTileForest::StaticClass());
	const TSubclassOf<AGDTileForest> TileClass = ClassGetter->GetClass();
	AGDTile* Tile = GetWorld()->SpawnActor<AGDTileForest>(TileClass,
	                                                      this->GetActorLocation(),
	                                                      FRotator(0, 0, 0));
	TArray<TArray<AGDTile*>> TilesGrid = this->OwningGrid->GetTilesGrid();
	const int32 X = this->Coordinates.X;
	const int32 Y = this->Coordinates.Y;
	TilesGrid[X][Y] = Tile;
	Tile->SetCoordinates({X, Y});
	Tile->SetOwningGrid(OwningGrid);
	GetWorld()->DestroyActor(this);
}

void AGDTile::ChangeInRiver()
{
	AGDTile* ClassGetter = NewObject<AGDTileRiver>(AGDTileRiver::StaticClass());
	const TSubclassOf<AGDTileRiver> TileClass = ClassGetter->GetClass();
	AGDTile* Tile = GetWorld()->SpawnActor<AGDTileRiver>(TileClass,
	                                                     this->GetActorLocation(),
	                                                     FRotator(0, 0, 0));
	TArray<TArray<AGDTile*>> TilesGrid = this->OwningGrid->GetTilesGrid();
	const int32 X = this->Coordinates.X;
	const int32 Y = this->Coordinates.Y;
	TilesGrid[X][Y] = Tile;
	Tile->SetCoordinates({X, Y});
	Tile->SetOwningGrid(OwningGrid);
	GetWorld()->DestroyActor(this);
}
