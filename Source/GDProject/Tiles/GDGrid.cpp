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

TArray<AGDTile*> AGDGrid::ComputePathBetweenTiles(AGDTile* StartTile, AGDTile* TargetTile)
{

	TMap<AGDTile*, AGDTile*> CameFrom;
	
	TMap<AGDTile*, int> GScores;
	GScores.Add(StartTile, 0);

	TMap<AGDTile*, int> FScores;
	FScores.Add(StartTile, StartTile->GetDistanceFrom(TargetTile));

	TArray<TPair<AGDTile*, int>> OpenTiles;
	OpenTiles.Add(TPair<AGDTile*, int>{StartTile, FScores[StartTile]});
	
	while (OpenTiles.Num() > 0)
	{
		OpenTiles.Sort([](const auto& Lhs, const auto& Rhs) { return Lhs.Value > Rhs.Value; });
		AGDTile* CurrentTile = OpenTiles.Pop().Key;
	
		if (CurrentTile == TargetTile)
		{
			return ReconstructPath(CameFrom, CurrentTile);
		}
	
		for (const auto& Neighbour : CurrentTile->GetNeighbours())
		{
			if(Neighbour->IsOccupied())
			{
				continue;
			}
			int TentativeGScore = GScores[CurrentTile] + 1;
			if (!GScores.Contains(Neighbour))
			{
				CameFrom.Add(Neighbour, CurrentTile);
				GScores.Add(Neighbour, TentativeGScore);
				FScores.Add(Neighbour, TentativeGScore + Neighbour->GetDistanceFrom(TargetTile));
				if (!OpenTiles.Contains(TPair<AGDTile*, int>{Neighbour, FScores[Neighbour]}))
				{
					OpenTiles.Add(TPair<AGDTile*, int>{Neighbour, FScores[Neighbour]});
					// VisitedCells.Add(Neighbour);
				}
			}
			else if (TentativeGScore < GScores[Neighbour])
			{
				CameFrom[Neighbour] = CurrentTile;
				GScores[Neighbour] = TentativeGScore;
				FScores[Neighbour] = TentativeGScore + Neighbour->GetDistanceFrom(TargetTile);
				if (!OpenTiles.Contains(TPair<AGDTile*, int>{Neighbour, FScores[Neighbour]}))
				{
					OpenTiles.Add(TPair<AGDTile*, int>{Neighbour, FScores[Neighbour]});
					// VisitedCells.Add(Neighbour);
				}
			}
		}
	}
	return TArray<AGDTile*>{};
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
