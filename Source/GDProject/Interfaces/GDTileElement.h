// © 2021 Alessandro Arena, Mattia De Prisco, Nunzio Lopardo All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "GDTileElement.generated.h"

UINTERFACE(MinimalAPI)
class UGDTileElement : public UInterface
{
	GENERATED_BODY()
};

class GDPROJECT_API IGDTileElement
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Tiles")
	UObject* GetTile();
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Tiles")
	bool CanBeSelected();
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Tiles")
	bool CanMove();
};
