// © 2021 Alessandro Arena, Mattia De Prisco, Nunzio Lopardo All Rights Reserved


#include "GDGrid.h"
#include "GDTile.h"
#include "Math/UnrealMathUtility.h"
#include "Kismet/KismetArrayLibrary.h"
#include "GDProject/Units/GDUnit.h"

AGDGrid::AGDGrid()
{
	DummyRoot = CreateDefaultSubobject<USceneComponent>(TEXT("DummyRoot"));
	RootComponent = DummyRoot;

	Width = 20;
	Height = 20;
	BlockSpacing = 10.f;
	bMapGenerated = false;
}

#define BASE_TILE 0
#define LOWLAND 1
#define FOREST 2
#define ROCK 3

void AGDGrid::BeginPlay()
{
	Super::BeginPlay();

	BuildMap();
}

void AGDGrid::BuildMap()
{
	const int32 NumBlocks = Width * Height;

	TArray<AGDTile*> SampleRow;
	SampleRow.Init(nullptr, Width);
	Tiles.Init(SampleRow, Height);

	int RowIndex = 0;
	int ColumnIndex = 0;
	
	int RowScheme = 1;
	int ColumnScheme = 1;
	
	GenerateGrid();
	// UE_LOG(LogTemp, Display, TEXT("Tile type %i"), TileScheme[RowIndex][ColumnIndex]);
	// UE_LOG(LogTemp, Display, TEXT("Tile type %i"), TileScheme[RowIndex][ColumnIndex]);
	
	for (int32 BlockIndex = 0; BlockIndex < NumBlocks; BlockIndex++)
	{
		const float XOffset = (BlockIndex / Width) * BlockSpacing;
		const float YOffset = (BlockIndex % Height) * BlockSpacing;

		const FVector BlockLocation = FVector(XOffset, YOffset, 0.f) + GetActorLocation();

		AGDTile* NewTile;

		if (TileScheme[RowScheme][ColumnScheme] == LOWLAND)
		{
			NewTile = GetWorld()->SpawnActor<AGDTileLowland>(LowlandTileClass, BlockLocation, FRotator(0, 0, 0));
		}
		else if (TileScheme[RowScheme][ColumnScheme] == FOREST)
		{
			NewTile = GetWorld()->SpawnActor<AGDTileForest>(ForestTileClass, BlockLocation, FRotator(0, 0, 0));
		}
		else if (TileScheme[RowScheme][ColumnScheme] == ROCK)
		{
			NewTile = GetWorld()->SpawnActor<AGDTileRocks>(RockTileClass, BlockLocation, FRotator(0, 0, 0));
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
		
		ColumnScheme++;
		if (++ColumnIndex == Width)
		{
			RowIndex++;
			ColumnIndex = 0;
			RowScheme++;
			ColumnScheme = 1;
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

void AGDGrid::ShufflePositions(TArray<TPair<int, int>>& Undefined_Tiles)
{
	if (Undefined_Tiles.Num() > 0)
	{
		int32 LastIndex = Undefined_Tiles.Num() - 1;
		for (int32 i = 0; i <= LastIndex; ++i)
		{
			int32 Index = FMath::RandRange(i, LastIndex);
			if (i != Index)
			{
				Undefined_Tiles.Swap(i, Index);
			}
		}
	}
}

const TArray<TArray<AGDTile*>>& AGDGrid::GetTilesGrid() const
{
	return Tiles;
}

#define G_SCORE Key
#define F_SCORE Value

TArray<AGDTile*> AGDGrid::ComputePathBetweenTiles(AGDTile* StartTile, AGDTile* TargetTile, AGDUnit* MovingUnit)
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

		for (const auto& Neighbour : CurrentTile->GetTraversableNeighbours())
		{
			if (Neighbour != TargetTile && Neighbour->IsOccupied())
			{
				continue;
			}

			float TentativeGScore = Grid[CurrentTile].G_SCORE + 1.f;

			//Increase score if enemy will attack when walking on that tile
			if (MovingUnit && Neighbour->HasGuardingUnits())
			{
				for (auto& GuardingUnit : Neighbour->GetGuardingUnits())
				{
					if (MovingUnit->IsEnemy(GuardingUnit))
					{
						TentativeGScore += 0.5f;
					}
				}
			}

			//Slightly increase score if there's a turn (to prefer straighter paths)
			AGDTile** PreviousTilePointer = CameFrom.Find(CurrentTile);
			if (PreviousTilePointer)
			{
				AGDTile* PreviousTile = *PreviousTilePointer;
				if (PreviousTile->GetCoordinates().X == CurrentTile->GetCoordinates().X && CurrentTile->GetCoordinates()
					.X != Neighbour->GetCoordinates().X || PreviousTile->GetCoordinates().Y == CurrentTile->
					GetCoordinates().Y && CurrentTile->GetCoordinates().Y != Neighbour->GetCoordinates().Y)
				{
					TentativeGScore += 0.2f;
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

void AGDGrid::GenerateGrid()
{
	TArray<int> TileMapRow;
	TileMapRow.Init(BASE_TILE, (Width + 2));
	TileScheme.Init(TileMapRow, (Height + 2));
	//Shuffle the pairs of coordinates to build the grid selecting random tile for iteration
	TArray<TPair<int, int>> Undefined_Tiles;
	for (int i = 1; i < (Width + 1); i++)
	{
		for (int j = 1; j < (Height + 1) ; j++)
		{
			TPair<int, int> Tmp(i, j);
			Undefined_Tiles.Push(Tmp);
			// UE_LOG(LogTemp, Warning, TEXT("Selecting tile -> %i - %i"), i, j);
		}
	}
	ShufflePositions(Undefined_Tiles);
	for (int i = 0; i < Undefined_Tiles.Num(); i++)
	{
		int Row = Undefined_Tiles[i].Key;
		int Col = Undefined_Tiles[i].Value;
		// UE_LOG(LogTemp, Warning, TEXT("Selecting tile -> %i - %i"), Row, Col);
		const TPair<int, int> Tile(Row, Col);
		TileScheme[Row][Col] = ComputeTileType(Tile);
	}
}

TArray<int> AGDGrid::SurroundCheck(TPair<int, int> Tile)
{
	TArray<int> SurroundCount;
	SurroundCount.Init(0, 3);

	const int Row = Tile.Key;
	const int Col = Tile.Value;

	TArray<int> Surround{
		TileScheme[Row - 1][Col - 1], TileScheme[Row - 1][Col], TileScheme[Row - 1][Col + 1],
		TileScheme[Row][Col - 1], TileScheme[Row][Col + 1],
		TileScheme[Row + 1][Col - 1], TileScheme[Row + 1][Col], TileScheme[Row + 1][Col + 1]
	};

	for (int i = 0; i < Surround.Num(); i++)
	{
		if (Surround[i] == LOWLAND)
		{
			SurroundCount[0] += 1;
		}
		else if (Surround[i] == FOREST)
		{
			SurroundCount[1] += 1;
		}
		else if (Surround[i] == ROCK)
		{
			SurroundCount[2] += 1;
		}
	}
	return SurroundCount;
}

int32 AGDGrid::ComputeTileType(TPair<int, int> Tile)
{
	TArray<int> SurroundCount = SurroundCheck(Tile);
	const float LowlandWeight = SurroundCount[0] / 8.f;
	const float ForestWeight = SurroundCount[1] / 8.f;
	const float RiverWeight = SurroundCount[2] / 8.f;

	float Weight_Sum = LowlandWeight + ForestWeight + RiverWeight;

	//Calcolo del valore per scalare i pesi circostanti
	float surround_weight = 1 - Weight_Sum;

	float Scaled_LowlandWeight;
	float Scaled_ForestWeight;
	float Scaled_RiverWeight;

	int row = Tile.Key;
	int col = Tile.Value;
	
	// if (RiverWeight > 0 && (TileScheme[row - 1][col] == ROCK ||
	// 	TileScheme[row + 1][col] == ROCK ||
	// 	TileScheme[row][col - 1] == ROCK ||
	// 	TileScheme[row][col + 1] == ROCK))
	// {
	// 	float new_river_prob = 0.95;
	// 	float new_forest_prob = 0.03;
	// 	float new_lowland_prob = 0.02;
	//
	// 	Scaled_LowlandWeight = (new_lowland_prob * surround_weight) + LowlandWeight;
	// 	Scaled_ForestWeight = (new_forest_prob * surround_weight) + ForestWeight;
	// 	Scaled_RiverWeight = (new_river_prob * surround_weight) + RiverWeight; 
	// }
	// else
	// {
	//
	// }
	Scaled_LowlandWeight = (Lowland_Percentage * surround_weight) + LowlandWeight;
	Scaled_ForestWeight = (Forest_Percentage * surround_weight) + ForestWeight;
	Scaled_RiverWeight = (Rock_Percentage * surround_weight) + RiverWeight; 

	const float ForestValue = Scaled_LowlandWeight + Scaled_ForestWeight;
	//Tile type generation
	const float TileType = FMath::FRandRange(0.f, 1.f);
	if (TileType <= Scaled_LowlandWeight)
	{
		return LOWLAND;
	}
	else if (TileType <= ForestValue)
	{
		return FOREST;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Selecting tile -> %i - %i"), row-1, col-1);
		UE_LOG(LogTemp, Warning, TEXT("Height -> %i"), Height);
		UE_LOG(LogTemp, Warning, TEXT("Width -> %i"), Width);
		if (row == 1 || row == Width || col == 1 || col == Width)
		{
			UE_LOG(LogTemp, Warning, TEXT("Lowland instead Rock"), Height);
			return LOWLAND;
		}
		return ROCK;
	}
}
