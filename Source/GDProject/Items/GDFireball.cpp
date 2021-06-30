// © 2021 Alessandro Arena, Mattia De Prisco, Nunzio Lopardo All Rights Reserved

#include "GDFireball.h"

#include "Actors/GDFireballActor.h"
#include "GDProject/Tiles/GDTile.h"
#include "GDProject/Units/GDUnit.h"
#include "Kismet/GameplayStatics.h"

UGDFireball::UGDFireball()
{
	Name = FText::FromString("Fireball");

	Description = FText::FromString("A fireball that deals 50 damage in an area");

	UseConditions = EUseConditions::Anywhere;

	static ConstructorHelpers::FObjectFinder<UTexture2D> TextureFinder(
		TEXT("Texture2D'/Game/Textures/Items/T_Fireball.T_Fireball'"));
	Thumbnail = TextureFinder.Object;

	const FStringAssetReference UsableMaterialPath(TEXT("/Game/Materials/M_Item_Damage_Decal.M_Item_Damage_Decal"));
	UsableMaterial = Cast<UMaterial>(UsableMaterialPath.TryLoad());

	Damage = 50.f;
}

void UGDFireball::Use(AGDTile* TargetTile)
{
	AGDFireballActor* Fireball = TargetTile->GetWorld()->SpawnActorDeferred<AGDFireballActor>(
		AGDFireballActor::StaticClass(), FTransform::Identity, nullptr, nullptr,
		ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

	TSet<AGDUnit*> UnitsToDamage;
	for (auto& Tile : TargetTiles)
	{
		if (AGDUnit* Unit = Cast<AGDUnit>(Tile->GetTileElement()))
		{
			UnitsToDamage.Add(Unit);
		}
	}

	FVector TargetLocation = TargetTile->GetActorLocation();
	TargetLocation.Z += 20;
	Fireball->Initialize(TargetLocation, UnitsToDamage);

	FVector FireballSpawnLocation = TargetTile->GetActorLocation();
	FireballSpawnLocation.Z += 1000.f;

	FTransform FireballTransform = FTransform::Identity;
	FireballTransform.SetLocation(FireballSpawnLocation);

	Fireball->FinishSpawning(FireballTransform);
}

void UGDFireball::ResetTargetTiles()
{
	for (auto& Tile : TargetTiles)
	{
		Tile->RemoveHighlight();
		if (AGDUnit* Unit = Cast<AGDUnit>(Tile->GetTileElement()))
		{
			Unit->RemoveOutline();
		}
	}
	TargetTiles.Empty();
}

void UGDFireball::HighlightAffectedTiles(AGDTile* TargetTile, bool NonUsable)
{
	ResetTargetTiles();
	if (TargetTile)
	{
		TargetTiles = TargetTile->GetTilesAround(1);
		for (auto& Tile : TargetTiles)
		{
			if (!NonUsable)
			{
				Tile->HighlightWithMaterial(UsableMaterial);
				if (AGDUnit* Unit = Cast<AGDUnit>(Tile->GetTileElement()))
				{
					Unit->AddOutline(FLinearColor::Red);
				}
			}
			else
			{
				Tile->HighlightWithMaterial(NonUsableMaterial);
			}
		}
	}
}

void UGDFireball::OnDeselect()
{
	ResetTargetTiles();
}
