// © 2021 Alessandro Arena, Mattia De Prisco, Nunzio Lopardo All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "GDAIControllerBase.generated.h"

class AGDUnit;

UCLASS()
class GDPROJECT_API AGDAIControllerBase : public AAIController
{
	GENERATED_BODY()

public:
	void Play();

private:

	TArray<AGDUnit*> GetEnemiesSortedByDistance() const;
	
};
