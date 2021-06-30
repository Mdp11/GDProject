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

	const FStringAssetReference UsableMaterialPath(TEXT("/Game/Materials/M_Item_Usable_Decal.M_Item_Usable_Decal"));
	UsableMaterial = Cast<UMaterial>(UsableMaterialPath.TryLoad());

	const FStringAssetReference NonUsableMaterialPath(
		TEXT("/Game/Materials/M_Item_Non_Usable_Decal.M_Item_Non_Usable_Decal"));
	NonUsableMaterial = Cast<UMaterial>(NonUsableMaterialPath.TryLoad());
}

bool UGDItemBase::RequestUse(AGDTile* TargetTile)
{
	if (CanBeUsed(TargetTile))
	{
		Use(TargetTile);
		return true;
	}
	return false;
}

void UGDItemBase::HighlightAffectedTiles(class AGDTile* TargetTile, bool NonUsable)
{
	if (!NonUsable && CanBeUsed(TargetTile))
	{
		TargetTile->HighlightWithMaterial(UsableMaterial);
	}
	else
	{
		TargetTile->HighlightWithMaterial(NonUsableMaterial);
	}
}

void UGDItemBase::OnSelect()
{
}

void UGDItemBase::OnDeselect()
{
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
