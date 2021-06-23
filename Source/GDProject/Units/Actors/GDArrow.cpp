// © 2021 Alessandro Arena, Mattia De Prisco, Nunzio Lopardo All Rights Reserved


#include "GDArrow.h"

#include "DrawDebugHelpers.h"
#include "GDProject/Units/GDArcher.h"
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
	OwnerUnit = Cast<AGDArcher>(GetOwner());
	StaticMeshComponent->IgnoreActorWhenMoving(OwnerUnit, true);
	StaticMeshComponent->IgnoreActorWhenMoving(Cast<AActor>(OwnerUnit->GetBow()), true);

	DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

	SetActorRotation(UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), TargetLocation));

	ProjectileMovementComponent = Cast<UProjectileMovementComponent>(
		AddComponentByClass(UProjectileMovementComponent::StaticClass(), false, FTransform::Identity, false));
	ProjectileMovementComponent->SetUpdatedComponent(StaticMeshComponent);
	ProjectileMovementComponent->SetVelocityInLocalSpace(FVector{4000.f, 0.f, 0.f});
	ProjectileMovementComponent->bRotationFollowsVelocity = true;

	SetActorEnableCollision(true);
}

// void AGDArrow::Drop()
// {
// 	StaticMeshComponent->OnComponentBeginOverlap.RemoveAll(this);
// 	DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
//     SetActorEnableCollision(true);
//     StaticMeshComponent->SetEnableGravity(true);
//     StaticMeshComponent->SetSimulatePhysics(true);
//     SetLifeSpan(5.f);
// }

void AGDArrow::OnComponentHit(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                              UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                              const FHitResult& SweepResult)
{
	ProjectileMovementComponent->SetVelocityInLocalSpace(FVector::ZeroVector);
	ProjectileMovementComponent->ProjectileGravityScale = 0.f;

	SetActorEnableCollision(false);

	SetActorLocation(SweepResult.ImpactPoint + GetActorForwardVector() * 10.f, false, nullptr,
	                 ETeleportType::TeleportPhysics);


	if (USkeletalMeshComponent* SkeletalMeshComponent = Cast<USkeletalMeshComponent>(
		OtherActor->FindComponentByClass(USkeletalMeshComponent::StaticClass())))
	{
		const FName BoneName = SkeletalMeshComponent->FindClosestBone(SweepResult.ImpactPoint);
		AttachToComponent(SkeletalMeshComponent, FAttachmentTransformRules::KeepWorldTransform, BoneName);
		if (OwnerUnit)
		{
			OwnerUnit->ApplyDamage();
		}
	}
	else
	{
		AttachToActor(OtherActor, FAttachmentTransformRules::KeepWorldTransform);
	}

	SetLifeSpan(5.f);
}
