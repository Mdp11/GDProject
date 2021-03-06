// © 2021 Alessandro Arena, Mattia De Prisco, Nunzio Lopardo All Rights Reserved


#include "GDInventoryComponent.h"

#include "GDProject/Items/GDFireball.h"
#include "GDProject/Items/GDHealthPotion.h"

UGDInventoryComponent::UGDInventoryComponent()
{
	Souls = 0;
	GoldenSouls = 0;
}

void UGDInventoryComponent::AddItemToStash(UGDItemBase* Item)
{
	AddItemToMap(StashedItems, Item);
}

void UGDInventoryComponent::RemoveItemFromStash(UGDItemBase* Item)
{
	RemoveItemFromMap(StashedItems, Item);
}

void UGDInventoryComponent::BringItemToBattle(UGDItemBase* Item)
{
	if (StashedItems.Find(Item))
	{
		AddItemToMap(BattleItems, Item);
		RemoveItemFromStash(Item);
	}
}

void UGDInventoryComponent::RemoveBattleItem(UGDItemBase* Item)
{
	RemoveItemFromMap(BattleItems, Item);
	OnInventoryUpdated.Broadcast();
}

void UGDInventoryComponent::StashLeftoverBattleItems()
{
	for (auto& Tuple : BattleItems)
	{
		for (int i = 0; i < Tuple.Value; ++i)
		{
			AddItemToStash(Tuple.Key);
		}
	}
	BattleItems.Empty();
}

void UGDInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	UGDHealthPotion* HealthPot = NewObject<UGDHealthPotion>(this, "HealthPotion");
	UGDFireball* Fireball = NewObject<UGDFireball>(this, "Fireball");
	BattleItems.Add(HealthPot, 3);
	BattleItems.Add(Fireball, 3);
}

void AddItemToMap(TMap<UGDItemBase*, uint8>& Map, UGDItemBase* Item)
{
	if (uint8* Counter = Map.Find(Item))
	{
		(*Counter)++;
	}
	else
	{
		Map.Add(Item, 1);
	}
}

void RemoveItemFromMap(TMap<UGDItemBase*, uint8>& Map, UGDItemBase* Item)
{
	if (uint8* Counter = Map.Find(Item))
	{
		if (--(*Counter) == 0)
		{
			Map.Remove(Item);
		}
	}
}
