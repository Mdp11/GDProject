// © 2021 Alessandro Arena, Mattia De Prisco, Nunzio Lopardo All Rights Reserved


#include "GDFireballActor.h"

#include "GDProject/Player/GDPlayerPawn.h"
#include "GDProject/Units/GDUnit.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Particles/ParticleSystemComponent.h"

AGDFireballActor::AGDFireballActor()
{
	PrimaryActorTick.bCanEverTick = true;

	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("DummyRoot"));
	SetRootComponent(SceneComponent);

	FireballEffect = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("FireballEffect"));
	FireballEffect->SetupAttachment(RootComponent);
	FireballEffect->SetAutoActivate(true);
	FireballEffect->SetWorldScale3D(FVector{3.f});

	const FStringAssetReference FireballEffectPath(
		TEXT("/Game/InfinityBladeEffects/Effects/FX_Mobile/Fire/combat/P_FireBall_Strong.P_FireBall_Strong"));
	UParticleSystem* FireballParticle = Cast<UParticleSystem>(FireballEffectPath.TryLoad());

	FireballEffect->SetTemplate(FireballParticle);

	const FStringAssetReference ExplosionEffectPath(TEXT(
		"/Game/InfinityBladeEffects/Effects/FX_Mobile/Fire/combat/P_ProjectileLob_Explo_Fire_02.P_ProjectileLob_Explo_Fire_02"));
	ExplosionEffect = Cast<UParticleSystem>(ExplosionEffectPath.TryLoad());

	Damage = 50.f;
}

void AGDFireballActor::Initialize(const FVector& Target, const TSet<AGDUnit*>& Units)
{
	TargetLocation = Target;
	UnitsToDamage = Units;
}

void AGDFireballActor::BeginPlay()
{
	Super::BeginPlay();

	PlayerPawn = Cast<AGDPlayerPawn>(GetWorld()->GetFirstPlayerController()->GetPawn());
	if (PlayerPawn)
	{
		PlayerPawn->AddActiveEntity(this);
	}

	SetActorRotation(UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), TargetLocation));
}

void AGDFireballActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (GetActorLocation().Equals(TargetLocation, 50.f))
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplosionEffect, TargetLocation, FRotator::ZeroRotator,
		                                         FVector{2.5f});
		for (AGDUnit* Unit : UnitsToDamage)
		{
			Unit->TakeDamage(Damage, FDamageEvent{}, nullptr, nullptr);
		}
		if (PlayerPawn)
		{
			PlayerPawn->RemoveActiveEntity(this);
		}
		Destroy();
	}
	else
	{
		SetActorLocation(
			UKismetMathLibrary::VInterpTo_Constant(GetActorLocation(), TargetLocation, DeltaTime, 1200.f));
	}
}
