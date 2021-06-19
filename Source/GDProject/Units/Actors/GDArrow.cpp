// © 2021 Alessandro Arena, Mattia De Prisco, Nunzio Lopardo All Rights Reserved


#include "GDArrow.h"

#include "DrawDebugHelpers.h"
#include "GDProject/Units/GDUnit.h"
#include "Kismet/KismetMathLibrary.h"

AGDArrow::AGDArrow()
{
	PrimaryActorTick.bCanEverTick = true;

	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	SetRootComponent(StaticMeshComponent);

	StaticMeshComponent->SetCollisionProfileName("OverlapAllDynamic");
	StaticMeshComponent->OnComponentBeginOverlap.AddDynamic(this, &AGDArrow::OnComponentHit);

	SetActorEnableCollision(false);
}

void AGDArrow::BeginPlay()
{
	Super::BeginPlay();
}

void AGDArrow::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AGDArrow::FireInDirection(const FVector& TargetLocation)
{
	OwnerUnit = Cast<AGDUnit>(GetOwner());
	StaticMeshComponent->IgnoreActorWhenMoving(GetOwner(), true);

	DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

	SetActorEnableCollision(true);
	SetActorRotation(UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), TargetLocation));

	ProjectileMovementComponent = Cast<UProjectileMovementComponent>(
		AddComponentByClass(UProjectileMovementComponent::StaticClass(), false, FTransform::Identity, false));
	ProjectileMovementComponent->SetUpdatedComponent(StaticMeshComponent);
	ProjectileMovementComponent->SetVelocityInLocalSpace(FVector{4000.f, 0.f, 0.f});
	ProjectileMovementComponent->bRotationFollowsVelocity = true;
}

void AGDArrow::OnComponentHit(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                              UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                              const FHitResult& SweepResult)
{
	ProjectileMovementComponent->SetVelocityInLocalSpace(FVector::ZeroVector);
	ProjectileMovementComponent->ProjectileGravityScale = 0.f;

	SetActorLocation(SweepResult.ImpactPoint - GetActorForwardVector() * 50.f, false, nullptr,
	                 ETeleportType::TeleportPhysics);

	AttachToActor(OtherActor, FAttachmentTransformRules::KeepWorldTransform);

	if (OwnerUnit)
	{
		OwnerUnit->ApplyDamage();
	}

	SetLifeSpan(3.f);
}
