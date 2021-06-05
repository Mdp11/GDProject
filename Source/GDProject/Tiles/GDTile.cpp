// Fill out your copyright notice in the Description page of Project Settings.


#include "GDTile.h"
#include "GDTile_Forest.h"
#include "GDTile_River.h"

AGDTile::AGDTile()
{
	DummyRoot = CreateDefaultSubobject<USceneComponent>(TEXT("DummyRoot"));
	RootComponent = DummyRoot;

	TileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TileMesh"));
	TileMesh->SetupAttachment(DummyRoot);


	bIsActive = false;
}

void AGDTile::SetOwningGrid(AGDGrid* Grid)
{
	OwningGrid = Grid;
}

void AGDTile::SetCoordinates(const FIntPoint& NewCoordinates)
{
	Coordinates = NewCoordinates;
}

void AGDTile::HandleClicked()
{
	if (!bIsActive)
	{
		if (SelectedMaterial)
		{
			TileMesh->SetMaterial(0, SelectedMaterial);
		}
	}
	else
	{
		if (SelectedMaterial)
		{
			TileMesh->SetMaterial(0, BaseMaterial);
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
			if (HoverMaterial)
			{
				TileMesh->SetMaterial(0, HoverMaterial);
			}
		}
		else
		{
			if (BaseMaterial)
			{
				TileMesh->SetMaterial(0, BaseMaterial);
			}
		}
	}
}

void AGDTile::AddElement(AActor* NewTileElement)
{
	/*
	if (NewTileElement->GetClass()->ImplementsInterface(UGDTileElement::StaticClass()))
	{
		TileElement = NewTileElement;
	}
	*/
}

void AGDTile::ChangeInForest() {
	TSubclassOf<AGDTile_Forest> TileClass;
	AGDTile* ClassGetter = NewObject<AGDTile_Forest>(AGDTile_Forest::StaticClass());
	TileClass = ClassGetter->GetClass();
	AGDTile* Tile = GetWorld()->SpawnActor<AGDTile_Forest>(TileClass, 
		this->GetActorLocation(), 
		FRotator(0, 0, 0));
	TArray<TArray<AGDTile*>> TilesGrid = this->OwningGrid->GetTilesGrid();
	int32 X = this->Coordinates.X;
	int32 Y = this->Coordinates.Y;
	TilesGrid[X][Y] = Tile;
	Tile->SetCoordinates({ X, Y });
	Tile->SetOwningGrid(OwningGrid);
	GetWorld()->DestroyActor(this);
}

void AGDTile::ChangeInRiver() {
	TSubclassOf<AGDTile_River> TileClass;
	AGDTile* ClassGetter = NewObject<AGDTile_River>(AGDTile_River::StaticClass());
	TileClass = ClassGetter->GetClass();
	AGDTile* Tile = GetWorld()->SpawnActor<AGDTile_River>(TileClass,
		this->GetActorLocation(),
		FRotator(0, 0, 0));
	TArray<TArray<AGDTile*>> TilesGrid = this->OwningGrid->GetTilesGrid();
	int32 X = this->Coordinates.X;
	int32 Y = this->Coordinates.Y;
	TilesGrid[X][Y] = Tile;
	Tile->SetCoordinates({ X, Y });
	Tile->SetOwningGrid(OwningGrid);
	GetWorld()->DestroyActor(this);
}