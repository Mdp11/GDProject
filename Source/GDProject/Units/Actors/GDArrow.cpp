// © 2021 Alessandro Arena, Mattia De Prisco, Nunzio Lopardo All Rights Reserved


#include "GDArrow.h"

AGDArrow::AGDArrow()
{
	PrimaryActorTick.bCanEverTick = true;

	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	SetRootComponent(StaticMeshComponent);

	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(
		TEXT("ProjectileMovementComponent"));
}

void AGDArrow::BeginPlay()
{
	Super::BeginPlay();
}

void AGDArrow::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AGDArrow::Fire()
{
	ProjectileMovementComponent->InitialSpeed = 3000.f;
	ProjectileMovementComponent->MaxSpeed = 3000.f;
	ProjectileMovementComponent->bRotationFollowsVelocity = true;
	ProjectileMovementComponent->bShouldBounce = false;
	StaticMeshComponent->SetSimulatePhysics(true);
	StaticMeshComponent->IgnoreActorWhenMoving(GetOwner(), true);
	DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

}
