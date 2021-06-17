// © 2021 Alessandro Arena, Mattia De Prisco, Nunzio Lopardo All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "GDUnit.h"
#include "GDArcher.generated.h"

UCLASS()
class GDPROJECT_API AGDArcher : public AGDUnit
{
	GENERATED_BODY()

public:
	AGDArcher();
protected:
	UPROPERTY(BlueprintReadOnly)
	bool bIsInOverWatch;
};
