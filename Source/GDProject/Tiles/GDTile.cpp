// © 2021 Alessandro Arena, Mattia De Prisco, Nunzio Lopardo All Rights Reserved


#include "GDTile.h"

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
	if(NewTileElement->GetClass()->ImplementsInterface(UGDTileElement::StaticClass()))
	{
		TileElement = NewTileElement;
	}
}


