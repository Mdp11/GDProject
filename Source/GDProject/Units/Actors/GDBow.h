// © 2021 Alessandro Arena, Mattia De Prisco, Nunzio Lopardo All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GDBow.generated.h"

class UCableComponent;
class AGDArcher;

UCLASS()
class GDPROJECT_API AGDBow : public AActor
{
	GENERATED_BODY()

public:
	AGDBow();

protected:
	UPROPERTY(VisibleAnywhere)
	USkeletalMeshComponent* SkeletalMeshComponent;

	UPROPERTY(VisibleAnywhere)
	FName TopCableSocketName;

	UPROPERTY(VisibleAnywhere)
	FName BottomCableSocketName;

	UPROPERTY(VisibleAnywhere)
	FName IdleBowCablesSocket;

	UPROPERTY(VisibleAnywhere)
	FName ArrowSocketName;

	UPROPERTY()
	UCableComponent* TopCable;

	UPROPERTY()
	UCableComponent* BottomCable;

	virtual void BeginPlay() override;

public:
	void AttachCablesTo(USceneComponent* Component, const FName& SocketName) const;

	USkeletalMeshComponent* GetMesh() const;

	FName GetIdleBowCablesSocketName() const;
};
