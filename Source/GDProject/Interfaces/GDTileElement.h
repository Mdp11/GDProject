// © 2021 Alessandro Arena, Mattia De Prisco, Nunzio Lopardo All Rights Reserved

#pragma once

#include "CoreMinimal.h"

#include "UObject/Interface.h"
#include "GDTileElement.generated.h"

class AGDTile;

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
	AGDTile* GetTile() const;
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Tiles")
	void SetTile(AGDTile* Tile);
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Tiles")
	bool CanBeSelected() const;
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Tiles")
	bool CanMove() const;
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Tiles")
	void Select();
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Tiles")
	void Deselect();
};
