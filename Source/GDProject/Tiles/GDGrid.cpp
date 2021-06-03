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
			Tiles[RowIndex][ColumnIndex] = NewTile;
		}
		if(++ColumnIndex >= Size)
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
			Tile->SetTileElement(NewUnit);
			IGDTileElement::Execute_SetTile(NewUnit, Tile);
		}
	}
}
