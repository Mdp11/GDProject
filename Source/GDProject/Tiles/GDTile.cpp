// © 2021 Alessandro Arena, Mattia De Prisco, Nunzio Lopardo All Rights Reserved


#include "GDTile.h"

#include "GDGrid.h"
#include "GDProject/Interfaces/GDTileElement.h"
#include "Components/DecalComponent.h"

AGDTile::AGDTile()
{
	DummyRoot = CreateDefaultSubobject<USceneComponent>(TEXT("DummyRoot"));
	RootComponent = DummyRoot;

	TileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TileMesh"));
	TileMesh->SetupAttachment(DummyRoot);

	DecalComponent = CreateDefaultSubobject<UDecalComponent>(TEXT("DecalComponent"));
	DecalComponent->SetupAttachment(TileMesh);

	DecalComponent->SetHiddenInGame(true);

	MovementRangeDecalComponent = CreateDefaultSubobject<UDecalComponent>(TEXT("MovementRangeDecalComponent"));
	MovementRangeDecalComponent->SetupAttachment(TileMesh);

	MovementRangeDecalComponent->SetHiddenInGame(true);

	bIsActive = false;

	bIsTraversable = true;

	TileElement = nullptr;
}


void AGDTile::SetOwningGrid(AGDGrid* Grid)
{
	OwningGrid = Grid;
}

void AGDTile::HandleClicked()
{
	if (!bIsActive)
	{
		if (SelectedDecalMaterial)
		{
			DecalComponent->SetMaterial(0, SelectedDecalMaterial);
			DecalComponent->SetHiddenInGame(false);
		}
	}
	else
	{
		if (SelectedDecalMaterial)
		{
			DecalComponent->SetHiddenInGame(true);
		}
	}

	bIsActive = !bIsActive;
}

void AGDTile::Highlight(const bool bOn) const
{
	if (!bIsActive)
	{
		if (bOn)
		{
			if (HoverDecalMaterial)
			{
				DecalComponent->SetMaterial(0, HoverDecalMaterial);
				DecalComponent->SetHiddenInGame(false);
			}
		}
		else
		{
			DecalComponent->SetHiddenInGame(true);
		}
	}
}

void AGDTile::HighlightDanger(bool bOn) const
{
	if (!bIsActive)
	{
		if (bOn)
		{
			if (EnemyDecalMaterial)
			{
				DecalComponent->SetMaterial(0, EnemyDecalMaterial);
				DecalComponent->SetHiddenInGame(false);
			}
		}
		else
		{
			DecalComponent->SetHiddenInGame(true);
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

TArray<AGDTile*> AGDTile::GetNeighbours(const bool bOnlyTraversable) const
{
	TArray<AGDTile*> Neighbours;

	if (Coordinates.X - 1 >= 0)
	{
		AGDTile* NeighbourTile = OwningGrid->GetTile({Coordinates.X - 1, Coordinates.Y});
		if (bOnlyTraversable)
		{
			if (NeighbourTile->IsTraversable())
			{
				Neighbours.Add(NeighbourTile);
			}
		}
		else
		{
			Neighbours.Add(NeighbourTile);
		}
	}
	if (Coordinates.X + 1 < OwningGrid->GetSize())
	{
		AGDTile* NeighbourTile = OwningGrid->GetTile({Coordinates.X + 1, Coordinates.Y});
		if (bOnlyTraversable)
		{
			if (NeighbourTile->IsTraversable())
			{
				Neighbours.Add(NeighbourTile);
			}
		}
		else
		{
			Neighbours.Add(NeighbourTile);
		}
	}
	if (Coordinates.Y - 1 >= 0)
	{
		AGDTile* NeighbourTile = OwningGrid->GetTile({Coordinates.X, Coordinates.Y - 1});
		if (bOnlyTraversable)
		{
			if (NeighbourTile->IsTraversable())
			{
				Neighbours.Add(NeighbourTile);
			}
		}
		else
		{
			Neighbours.Add(NeighbourTile);
		}
	}
	if (Coordinates.Y + 1 < OwningGrid->GetSize())
	{
		AGDTile* NeighbourTile = OwningGrid->GetTile({Coordinates.X, Coordinates.Y + 1});
		if (bOnlyTraversable)
		{
			if (NeighbourTile->IsTraversable())
			{
				Neighbours.Add(NeighbourTile);
			}
		}
		else
		{
			Neighbours.Add(NeighbourTile);
		}
	}

	return Neighbours;
}

AGDGrid* AGDTile::GetGrid() const
{
	return OwningGrid;
}

void AGDTile::ApplyMovementRangeDecal(const bool bShort)
{
	if (ShortMovementDecalMaterial && LongMovementDecalMaterial)
	{
		MovementRangeDecalComponent->SetHiddenInGame(false);

		UMaterial* MovementDecalMat = bShort ? ShortMovementDecalMaterial : LongMovementDecalMaterial;

		MovementRangeDecalComponent->SetMaterial(0, MovementDecalMat);
	}
}

void AGDTile::ApplyEnemyDecal()
{
	if (EnemyTileDecalMaterial)
	{
		MovementRangeDecalComponent->SetHiddenInGame(false);
		MovementRangeDecalComponent->SetMaterial(0, EnemyTileDecalMaterial);
	}
}

void AGDTile::RemoveMovementRangeDecal() const
{
	MovementRangeDecalComponent->SetHiddenInGame(true);
}
