// © 2021 Alessandro Arena, Mattia De Prisco, Nunzio Lopardo All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GDInventoryComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInventoryUpdated);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class GDPROJECT_API UGDInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UGDInventoryComponent();

	UPROPERTY(BlueprintReadOnly)
	TMap<class UGDItemBase*, uint8> StashedItems;

	UPROPERTY(BlueprintReadOnly)
	TMap<class UGDItemBase*, uint8> BattleItems;

	UPROPERTY(BlueprintAssignable)
	FOnInventoryUpdated OnInventoryUpdated;

	void AddItemToStash(class UGDItemBase* Item);

	void RemoveItemFromStash(class UGDItemBase* Item);

	void BringItemToBattle(class UGDItemBase* Item);

	void RemoveBattleItem(class UGDItemBase* Item);

	void StashLeftoverBattleItems();

protected:
	int Souls;

	int GoldenSouls;

	virtual void BeginPlay() override;
};

void AddItemToMap(TMap<class UGDItemBase*, uint8>& Map, class UGDItemBase* Item);

void RemoveItemFromMap(TMap<class UGDItemBase*, uint8>& Map, class UGDItemBase* Item);
