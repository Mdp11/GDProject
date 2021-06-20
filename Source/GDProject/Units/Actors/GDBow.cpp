// © 2021 Alessandro Arena, Mattia De Prisco, Nunzio Lopardo All Rights Reserved


#include "GDBow.h"

#include "CableComponent.h"

AGDBow::AGDBow()
{
	PrimaryActorTick.bCanEverTick = false;

	SkeletalMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMeshComponent"));
	SetRootComponent(SkeletalMeshComponent);

	TopCableSocketName = "TopWire";
	BottomCableSocketName = "BottomWire";
	IdleBowCablesSocket = "BowIdle";

	TopCable = CreateDefaultSubobject<UCableComponent>(TEXT("TopCable"));
	BottomCable = CreateDefaultSubobject<UCableComponent>(TEXT("BottomCable"));

	TopCable->SetupAttachment(RootComponent, TopCableSocketName);
	BottomCable->SetupAttachment(RootComponent, BottomCableSocketName);

	TopCable->CableLength = 0.f;
	TopCable->NumSegments = 1.f;
	TopCable->CableWidth = 0.5f;
	TopCable->NumSides = 4.f;
	
	BottomCable->CableLength = 100.f;
	BottomCable->NumSegments = 1.f;
	BottomCable->CableWidth = 0.5f;
	BottomCable->NumSides = 4.f;
}

void AGDBow::BeginPlay()
{
	Super::BeginPlay();
	TopCable->SetAttachEndTo(this, "SkeletalMeshComponent", IdleBowCablesSocket);
	BottomCable->SetAttachEndTo(this, "SkeletalMeshComponent", IdleBowCablesSocket);
}
