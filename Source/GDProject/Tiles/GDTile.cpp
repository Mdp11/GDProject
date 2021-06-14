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
		SelectionDecalComponent->SetHiddenInGame(true);
		bIsActive = false;
	}
}

void AGDTile::Highlight(const bool bOn) const
{
	if (!bIsActive)
	{
		if (bOn)
		{
			if (OwningGrid->HoverDecalMaterial)
			{
				SelectionDecalComponent->SetMaterial(0, OwningGrid->HoverDecalMaterial);
				SelectionDecalComponent->SetHiddenInGame(false);
			}
		}
		else
		{
			SelectionDecalComponent->SetHiddenInGame(true);
		}
	}
}

void AGDTile::HighlightTargetEnemy(const bool bOn) const
{
	if (!bIsActive)
	{
		if (bOn)
		{
			if (OwningGrid->TargetedEnemyDecalMaterial)
			{
				SelectionDecalComponent->SetMaterial(0, OwningGrid->TargetedEnemyDecalMaterial);
				SelectionDecalComponent->SetHiddenInGame(false);
			}
		}
		else
		{
			SelectionDecalComponent->SetHiddenInGame(true);
		}
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
	if(AGDUnit* Unit = Cast<AGDUnit>(TileElement))
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

void AGDTile::ChangeInForest()
{
	TSubclassOf<AGDTileForest> TileClass;
	AGDTile* ClassGetter = NewObject<AGDTileForest>(AGDTileForest::StaticClass());
	TileClass = ClassGetter->GetClass();
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
	TSubclassOf<AGDTileRiver> TileClass;
	AGDTile* ClassGetter = NewObject<AGDTileRiver>(AGDTileRiver::StaticClass());
	TileClass = ClassGetter->GetClass();
	AGDTile* Tile = GetWorld()->SpawnActor<AGDTileRiver>(TileClass,
	                                                     this->GetActorLocation(),
	                                                     FRotator(0, 0, 0));
	TArray<TArray<AGDTile*>> TilesGrid = this->OwningGrid->GetTilesGrid();
	int32 X = this->Coordinates.X;
	int32 Y = this->Coordinates.Y;
	TilesGrid[X][Y] = Tile;
	Tile->SetCoordinates({X, Y});
	Tile->SetOwningGrid(OwningGrid);
	GetWorld()->DestroyActor(this);
}
