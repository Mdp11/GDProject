// © 2021 Alessandro Arena, Mattia De Prisco, Nunzio Lopardo All Rights Reserved


#include "GDGrid.h"

#include "GDTile.h"
#include "GDProject/Units/GDUnit.h"

AGDGrid::AGDGrid()
{
	DummyRoot = CreateDefaultSubobject<USceneComponent>(TEXT("DummyRoot"));
	RootComponent = DummyRoot;

	Size = 10;
	BlockSpacing = 250.f;
}

void AGDGrid::BeginPlay()
{
	Super::BeginPlay();

	const int32 NumBlocks = Size * Size;

	TArray<AGDTile*> SampleRow;
	SampleRow.Init(nullptr, Size);

	Tiles.Init(SampleRow, Size);

	int RowIndex = 0;
	int ColumnIndex = 0;

	for (int32 BlockIndex = 0; BlockIndex < NumBlocks; BlockIndex++)
	{
		const float XOffset = (BlockIndex / Size) * BlockSpacing;
		const float YOffset = (BlockIndex % Size) * BlockSpacing;

		const FVector BlockLocation = FVector(XOffset, YOffset, 0.f) + GetActorLocation();

		AGDTile* NewTile = GetWorld()->SpawnActor<AGDTile>(TileClass, BlockLocation, FRotator(0, 0, 0));

		if (NewTile != nullptr)
		{
			NewTile->SetOwningGrid(this);
			NewTile->SetCoordinates({RowIndex, ColumnIndex});
			Tiles[RowIndex][ColumnIndex] = NewTile;
		}
		if (++ColumnIndex >= Size)
		{
			ColumnIndex = 0;
			RowIndex++;
		}
	}

	for (auto& Tile : Tiles[0])
	{
		if (Tile)
		{
			FVector UnitLocation = Tile->GetActorLocation();
			UnitLocation.Z += 100.f;
			AGDUnit* NewUnit = GetWorld()->SpawnActor<AGDUnit>(TileUnitClassDummy, UnitLocation,
			                                                   FRotator(0, 0, 0));
			IGDTileElement::Execute_SetTile(NewUnit, Tile);
		}
	}
}

#define G_SCORE Key
#define F_SCORE Value

TArray<AGDTile*> AGDGrid::ComputePathBetweenTiles(AGDTile* StartTile, AGDTile* TargetTile)
{
	//Map having tiles as keys and g/h values in pair
	TMap<AGDTile*, TPair<float, float>> Grid;
	for (const auto& TileRow : Tiles)
	{
		for (const auto& Tile : TileRow)
		{
			Grid.Add(Tile, TPair<float, float>{TNumericLimits<float>::Max(), TNumericLimits<float>::Max()});
		}
	}

	TMap<AGDTile*, AGDTile*> CameFrom;

	Grid[StartTile] = TPair<float, float>{0, StartTile->GetDistanceFrom(TargetTile)};

	TArray<AGDTile*> OpenTiles;
	OpenTiles.Add(StartTile);

	while (OpenTiles.Num() > 0)
	{
		Algo::Sort(OpenTiles, [&Grid](const AGDTile* Lhs, const AGDTile* Rhs)
		{
			return Grid[Lhs].F_SCORE > Grid[Rhs].F_SCORE;
		});
		AGDTile* CurrentTile = OpenTiles.Pop();

		if (CurrentTile == TargetTile)
		{
			return ReconstructPath(CameFrom, CurrentTile);
		}

		for (const auto& Neighbour : CurrentTile->GetNeighbours())
		{
			if (Neighbour->IsOccupied())
			{
				continue;
			}

			float TentativeGScore = Grid[CurrentTile].G_SCORE + 0.6f;

			AGDTile** PreviousTilePointer = CameFrom.Find(CurrentTile);
			if (PreviousTilePointer)
			{
				AGDTile* PreviousTile = *PreviousTilePointer;
				if (PreviousTile->GetCoordinates().X == CurrentTile->GetCoordinates().X && CurrentTile->GetCoordinates()
					.X != Neighbour->GetCoordinates().X ||
					PreviousTile->GetCoordinates().Y == CurrentTile->GetCoordinates().Y && CurrentTile->
					GetCoordinates().Y != Neighbour->GetCoordinates().Y)
				{
					TentativeGScore += 1;
				}
			}

			if (TentativeGScore < Grid[Neighbour].G_SCORE)
			{
				Grid[Neighbour] = TPair<float, float>{
					TentativeGScore, TentativeGScore + Neighbour->GetDistanceFrom(TargetTile)
				};

				CameFrom.FindOrAdd(Neighbour, CurrentTile);
				if (!OpenTiles.Contains(Neighbour))
				{
					OpenTiles.Add(Neighbour);
				}
			}
		}
	}
	return TArray<AGDTile*>{};
}

#undef G_SCORE
#undef F_SCORE

TSet<AGDTile*> AGDGrid::GetTilesAtDistance(AGDTile* StartTile, int Distance) const
{
	TSet<AGDTile*> ValidTiles;

	for (const auto& TileRow : Tiles)
	{
		for (const auto& Tile : TileRow)
		{
			if (StartTile != Tile && StartTile->GetDistanceFrom(Tile) <= Distance)
			{
				ValidTiles.Add(Tile);
			}
		}
	}

	return ValidTiles;
}


TArray<AGDTile*> AGDGrid::ReconstructPath(const TMap<AGDTile*, AGDTile*> CameFrom, AGDTile* Tile)
{
	TArray<AGDTile*> Path{Tile};

	while (CameFrom.Contains(Tile))
	{
		Tile = CameFrom[Tile];
		Path.Insert(Tile, 0);
	}

	return Path;
}

AGDTile* AGDGrid::GetTile(const FIntPoint& Coordinates) const
{
	return Tiles[Coordinates.X][Coordinates.Y];
}

int32 AGDGrid::GetSize() const
{
	return Size;
}
