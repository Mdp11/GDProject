// © 2021 Alessandro Arena, Mattia De Prisco, Nunzio Lopardo All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "GDUnit.h"
#include "GDArcher.generated.h"

class AGDArrow;

UCLASS()
class GDPROJECT_API AGDArcher : public AGDUnit
{
	GENERATED_BODY()

	friend class UArrowNotify;

public:
	AGDArcher();

protected:
	UPROPERTY(BlueprintReadOnly)
	bool bIsInOverWatch;

	UPROPERTY(EditDefaultsOnly, Category="Arrow")
	TSubclassOf<AGDArrow> ArrowClass;

	UPROPERTY(VisibleDefaultsOnly, Category="Arrow")
	FName ArrowAttachSocketName;

	virtual void BeginPlay() override;

private:
	UPROPERTY()
	AGDArrow* Arrow;

	void SpawnArrow();

	void FireArrow() const;
};
