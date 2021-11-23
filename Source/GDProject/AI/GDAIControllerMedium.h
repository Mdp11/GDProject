// © 2021 Alessandro Arena, Mattia De Prisco, Nunzio Lopardo All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "GDProject/AI/GDAIControllerBase.h"
#include "GDAIControllerMedium.generated.h"

UCLASS()
class GDPROJECT_API AGDAIControllerMedium : public AGDAIControllerBase
{
	GENERATED_BODY()

protected:
	virtual AGDTile* GetAttackTargetTile() override;
};
