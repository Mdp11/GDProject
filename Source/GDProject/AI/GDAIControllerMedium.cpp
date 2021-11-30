// © 2021 Alessandro Arena, Mattia De Prisco, Nunzio Lopardo All Rights Reserved


#include "GDProject/AI/GDAIControllerMedium.h"

#include "GDProject/Tiles/GDTile.h"
#include "GDProject/Units/GDUnit.h"
#include "GDProject/AI/Utils/TRouletteSelector.h"


AGDTile* AGDAIControllerMedium::GetAttackTargetTile()
{
	AGDTile* CurrentTile = IGDTileElement::Execute_GetTile(ControlledUnit);
	TRouletteSelector<AGDTile*, float> Selector;
	for (const auto& Tile : ControlledUnit->HighlightedEnemyTilesInRange)
	{
		const AGDUnit* TargetUnit = Cast<AGDUnit>(Tile->GetTileElement());
		const float TargetCurrentHealthPercentage = TargetUnit->GetCurrentHealth() / TargetUnit->GetMaxHealth();
		const float TargetDefence = TargetUnit->GetDefence();
		const int DistanceFromTarget = Tile->GetDistanceFrom(CurrentTile);

		const float TargetWeight = 1.f / static_cast<float>(DistanceFromTarget) / 1.2f + 1.f /
			TargetCurrentHealthPercentage + 1.f / TargetDefence * 1.2f;

		Selector.AddEntry(Tile, TargetWeight);
	}

	return Selector.Size() > 0 ? Selector.RouletteWheel() : nullptr;
}
