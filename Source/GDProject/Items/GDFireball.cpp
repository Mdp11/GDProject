// © 2021 Alessandro Arena, Mattia De Prisco, Nunzio Lopardo All Rights Reserved

#include "GDFireball.h"

#include "GDProject/Tiles/GDTile.h"
#include "GDProject/Units/GDUnit.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"

UGDFireball::UGDFireball()
{
	Name = FText::FromString("Fireball");

	Description = FText::FromString("A fireball that deals 50 damage in an area");

	UseConditions = EUseConditions::Anywhere;

	static ConstructorHelpers::FObjectFinder<UTexture2D> TextureFinder(
		TEXT("Texture2D'/Game/Textures/Items/T_Fireball.T_Fireball'"));
	Thumbnail = TextureFinder.Object;

	const FStringAssetReference FireballEffectPath(TEXT("/Game/FX/Particles/P_HealEffect.P_HealEffect"));
	FireballEffect = Cast<UParticleSystem>(FireballEffectPath.TryLoad());

	const FStringAssetReference UsableMaterialPath(TEXT("/Game/Materials/M_Item_Damage_Decal.M_Item_Damage_Decal"));
	UsableMaterial = Cast<UMaterial>(UsableMaterialPath.TryLoad());

	Damage = 50.f;
}

void UGDFireball::Use(AGDTile* TargetTile)
{
	for (auto& Tile : TargetTiles)
	{
		if (AGDUnit* Unit = Cast<AGDUnit>(Tile->GetTileElement()))
		{
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), FireballEffect, TargetTile->GetActorLocation());
			Unit->TakeDamage(Damage, FDamageEvent{}, nullptr, nullptr);
		}
	}
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

void UGDFireball::HighlightAffectedTiles(AGDTile* TargetTile)
{
	ResetTargetTiles();
	if (TargetTile)
	{
		TargetTiles = TargetTile->GetTilesAround(1);
		for (auto& Tile : TargetTiles)
		{
			Tile->HighlightWithMaterial(UsableMaterial);
			if (AGDUnit* Unit = Cast<AGDUnit>(Tile->GetTileElement()))
			{
				Unit->AddOutline(FLinearColor::Red);
			}
		}
	}
}

void UGDFireball::OnDeselect()
{
	ResetTargetTiles();
}
