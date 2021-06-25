// © 2021 Alessandro Arena, Mattia De Prisco, Nunzio Lopardo All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "GDBowStretchNotify.generated.h"

UCLASS()
class GDPROJECT_API UGDBowStretchNotify : public UAnimNotify
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category="Action")
	bool bPull;

	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;
};
