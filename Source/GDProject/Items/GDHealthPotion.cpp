// © 2021 Alessandro Arena, Mattia De Prisco, Nunzio Lopardo All Rights Reserved


#include "GDHealthPotion.h"

#include "GDProject/Tiles/GDTile.h"
#include "GDProject/Units/GDUnit.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"


UGDHealthPotion::UGDHealthPotion()
{
	Name = FText::FromString("Health potion");

	Description = FText::FromString("A potion that can be used to restore the 25% health of an ally");

	UseConditions = EUseConditions::AlliesOnly;

	static ConstructorHelpers::FObjectFinder<UTexture2D> TextureFinder(
		TEXT("Texture2D'/Game/Textures/Items/T_HealthPotion.T_HealthPotion'"));
	Thumbnail = TextureFinder.Object;

	const FStringAssetReference HealEffectPath(TEXT("/Game/FX/Particles/P_HealEffect.P_HealEffect"));
	HealEffect = Cast<UParticleSystem>(HealEffectPath.TryLoad());
}

void UGDHealthPotion::Use(AGDTile* TargetTile)
{
	if (AGDUnit* TargetUnit = Cast<AGDUnit>(TargetTile->GetTileElement()))
	{
		TargetUnit->TakeDamage(-HealValue, FDamageEvent{}, nullptr, nullptr);
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), HealEffect, TargetUnit->GetActorLocation());
	}
}
