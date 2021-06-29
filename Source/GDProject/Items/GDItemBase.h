// © 2021 Alessandro Arena, Mattia De Prisco, Nunzio Lopardo All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GDItemBase.generated.h"

UENUM(BlueprintType)
enum class EUseConditions : uint8
{
	Anywhere UMETA(DisplayName = "Anywhere"),
	AlliesOnly UMETA(DisplayName = "AlliesOnly"),
	EnemiesOnly UMETA(DisplayName = "EnemiesOnly"),
	NeutralOnly UMETA(DisplayName = "NeutralOnly"),
};

UCLASS(Abstract, BlueprintType, Blueprintable, EditInlineNew, DefaultToInstanced)
class GDPROJECT_API UGDItemBase : public UObject
{
	GENERATED_BODY()

public:
	UGDItemBase();

	UFUNCTION(BlueprintCallable)
	virtual void RequestUse(class AGDTile* TargetTile);

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Item")
	class UTexture2D* Thumbnail;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Item")
	FText Name;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Item", meta = (Multiline = true))
	FText Description;

	EUseConditions UseConditions;

	virtual bool CanBeUsed(class AGDTile* TargetTile);

	virtual void Use(class AGDTile* TargetTile) PURE_VIRTUAL(UGDItemBase,);
};
