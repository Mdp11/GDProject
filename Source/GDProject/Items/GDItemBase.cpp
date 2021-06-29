// © 2021 Alessandro Arena, Mattia De Prisco, Nunzio Lopardo All Rights Reserved


#include "GDItemBase.h"

#include "GDProject/GDProjectGameModeBase.h"
#include "GDProject/Tiles/GDTile.h"
#include "GDProject/Units/GDUnit.h"

UGDItemBase::UGDItemBase()
{
	Name = FText::FromString("Item");
	Description = FText::FromString("Description");

	UseConditions = EUseConditions::Anywhere;
}

void UGDItemBase::RequestUse(AGDTile* TargetTile)
{
	if (CanBeUsed(TargetTile))
	{
		Use(TargetTile);
	}
}

bool UGDItemBase::CanBeUsed(AGDTile* TargetTile)
{
	bool bCanBeUsed = false;

	if (AGDProjectGameModeBase* GameMode = Cast<AGDProjectGameModeBase>(GetWorld()->GetAuthGameMode()))
	{
		switch (UseConditions)
		{
		case EUseConditions::AlliesOnly:
			if (AGDUnit* Unit = Cast<AGDUnit>(TargetTile->GetTileElement()))
			{
				bCanBeUsed = Unit->IsOwnedByPlayer(GameMode->GetCurrentPlayerTurn());
			}
			break;

		case EUseConditions::EnemiesOnly:
			if (AGDUnit* Unit = Cast<AGDUnit>(TargetTile->GetTileElement()))
			{
				bCanBeUsed = !Unit->IsOwnedByPlayer(GameMode->GetCurrentPlayerTurn());
			}
			break;

		case EUseConditions::NeutralOnly:
			bCanBeUsed = !TargetTile->IsOccupied();
			break;

		case EUseConditions::Anywhere:
			bCanBeUsed = true;
		}
	}

	return bCanBeUsed;
}
