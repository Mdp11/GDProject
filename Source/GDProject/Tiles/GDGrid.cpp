// © 2021 Alessandro Arena, Mattia De Prisco, Nunzio Lopardo All Rights Reserved


#include "GDGrid.h"

#include "GDTile.h"

AGDGrid::AGDGrid()
{
	DummyRoot = CreateDefaultSubobject<USceneComponent>(TEXT("DummyRoot"));
	RootComponent = DummyRoot;

	Width = 20;
	Height = 20;
	BlockSpacing = 100.f;
	bMapGenerated = false;
}

#define BASE_TILE 0
#define FOREST 1
#define RIVER 2

void AGDGrid::BeginPlay()
{
	Super::BeginPlay();

	TArray<TArray<int>> TileScheme;

	TArray<int> TileMapRow;
	TileMapRow.Init(BASE_TILE, Width);

	TileScheme.Init(TileMapRow, Height);

	BuildMap(TileScheme);
}

void AGDGrid::BuildMap(const TArray<TArray<int>>& TileScheme)
{
	const int32 NumBlocks = Width * Height;

	TArray<AGDTile*> SampleRow;
	SampleRow.Init(nullptr, Width);

	Tiles.Init(SampleRow, Height);

	int RowIndex = 0;
	int ColumnIndex = 0;

	for (int32 BlockIndex = 0; BlockIndex < NumBlocks; BlockIndex++)
	{
		const float XOffset = (BlockIndex / Width) * BlockSpacing;
		const float YOffset = (BlockIndex % Height) * BlockSpacing;

		const FVector BlockLocation = FVector(XOffset, YOffset, 0.f) + GetActorLocation();

		AGDTile* NewTile;

		if (TileScheme[RowIndex][ColumnIndex] == FOREST && ForestTileClass)
		{
			NewTile = GetWorld()->SpawnActor<AGDTile>(ForestTileClass, BlockLocation, FRotator(0, 0, 0));
		}
		else if (TileScheme[RowIndex][ColumnIndex] == RIVER && RiverTileClass)
		{
			NewTile = GetWorld()->SpawnActor<AGDTile>(RiverTileClass, BlockLocation, FRotator(0, 0, 0));
		}
		else
		{
			NewTile = GetWorld()->SpawnActor<AGDTile>(BaseTileClass, BlockLocation, FRotator(0, 0, 0));
		}

		if (NewTile != nullptr)
		{
			NewTile->SetOwningGrid(this);
			NewTile->SetCoordinates({RowIndex, ColumnIndex});
			Tiles[RowIndex][ColumnIndex] = NewTile;
		}

		if (++ColumnIndex == Width)
		{
			RowIndex++;
			ColumnIndex = 0;
		}
	}
}

void AGDGrid::CleanMap()
{
	if (Tiles.Num() > 0)
	{
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow,
		                                 FString::Printf(TEXT("RILEVATA MAP")));
		for (int i = 0; i < Width; i++)
		{
			for (int j = 0; j < Height; j++)
			{
				if (GEngine)
					GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow,
					                                 FString::Printf(TEXT("CREAZIONE MAP %d  %d"), i, j));
				GetWorld()->DestroyActor(Tiles[i][j]);
			}
		}
		Tiles.Empty();
		return;
	}

	bMapGenerated = false;
}

const TArray<TArray<AGDTile*>>& AGDGrid::GetTilesGrid() const
{
	return Tiles;
}

#define G_SCORE Key
#define F_SCORE Value

TArray<AGDTile*> AGDGrid::ComputePathBetweenTiles(AGDTile* StartTile, AGDTile* TargetTile, int StopAtDistance)
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

		if (CurrentTile == TargetTile || CurrentTile->GetDistanceFrom(TargetTile) <= StopAtDistance)
		{
			return ReconstructPath(CameFrom, CurrentTile);
		}

		for (const auto& Neighbour : CurrentTile->GetTraversableNeighbours())
		{
			if (Neighbour != TargetTile && Neighbour->IsOccupied())
			{
				continue;
			}

			float TentativeGScore = Grid[CurrentTile].G_SCORE + 1.f;

			AGDTile** PreviousTilePointer = CameFrom.Find(CurrentTile);
			if (PreviousTilePointer)
			{
				AGDTile* PreviousTile = *PreviousTilePointer;
				if (PreviousTile->GetCoordinates().X == CurrentTile->GetCoordinates().X && CurrentTile->
					GetCoordinates()
					.X != Neighbour->GetCoordinates().X ||
					PreviousTile->GetCoordinates().Y == CurrentTile->GetCoordinates().Y && CurrentTile->
					GetCoordinates().Y != Neighbour->GetCoordinates().Y)
				{
					TentativeGScore += 0.6f;
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
	return Coordinates.X >= 0 && Coordinates.X < Width && Coordinates.Y >= 0 && Coordinates.Y < Height
		       ? Tiles[Coordinates.X][Coordinates.Y]
		       : nullptr;
}

int32 AGDGrid::GetSize() const
{
	return Width * Height;
}
