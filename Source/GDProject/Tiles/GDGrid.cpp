// Fill out your copyright notice in the Description page of Project Settings.

#include "GDTile.h"
#include "GDTile_Forest.h"
#include "GDGrid.h"

// Sets default values
AGDGrid::AGDGrid()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	DummyRoot = CreateDefaultSubobject<USceneComponent>(TEXT("DummyRoot"));
	RootComponent = DummyRoot;


	Width = 6;
	Hight = 6;
	BlockSpacing = 100.f;
	mapGenerated = false;

}



// Called when the game starts or when spawned
void AGDGrid::BeginPlay()
{
	Super::BeginPlay();

	//BuildMap();
}

void AGDGrid::BuildMap()
{
	if (mapGenerated == false)
	{  
		if (GEngine)
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow,
				FString::Printf(TEXT("CREAZIONE MAP")));
		const int32 NumBlocks = Width * Hight;


		TArray<AGDTile*> SampleRow;
		SampleRow.Init(nullptr, NumBlocks);

		TilesGrid.Init(SampleRow, NumBlocks);

		int RowIndex = 0;
		int ColumnIndex = 0;

		for (int32 BlockIndex = 0; BlockIndex < NumBlocks; BlockIndex++)
		{
			const float XOffset = (BlockIndex / Width) * BlockSpacing;
			const float YOffset = (BlockIndex % Hight) * BlockSpacing;

			const FVector BlockLocation = FVector(XOffset, YOffset, 0.f) + GetActorLocation();

			//AGDTile* NewTile = NewObject<AGDTile_Forest>(AGDTile_Forest::StaticClass());	
			AGDTile* NewTile = GetWorld()->SpawnActor<AGDTile>(TileClass, BlockLocation, FRotator(0, 0, 0));

			if (NewTile != nullptr)
			{
				NewTile->SetOwningGrid(this);
				NewTile->SetCoordinates({ RowIndex, ColumnIndex });
				TilesGrid[RowIndex][ColumnIndex] = NewTile;
			}
			if (ColumnIndex <= Width)
			{
				RowIndex++;

				if (RowIndex <= Hight)
				{
					ColumnIndex++;
					RowIndex = 0;
				}
			}

		}
		mapGenerated = true;
	}
	else
	{
		if (GEngine)
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow,
				FString::Printf(TEXT("RETURN MAP")));
		return;
		
	}
	
}

void AGDGrid::CleanMap()
{
	if (TilesGrid.Num() > 0)
	{
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow,
			FString::Printf(TEXT("RILEVATA MAP")));
		for (int i = 0; i < Width; i++)
		{
			for (int j = 0; j < Hight; j++)
			{
				if (GEngine)
					GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow,
						FString::Printf(TEXT("CREAZIONE MAP %f  %f"), i, j));
				GetWorld()->DestroyActor(TilesGrid[i][j]);
			}
		}
		TilesGrid.Empty();
		return;
	}

	mapGenerated = false;
}

TArray<TArray<AGDTile*>> AGDGrid::GetTilesGrid()
{
	return this->TilesGrid;
}
