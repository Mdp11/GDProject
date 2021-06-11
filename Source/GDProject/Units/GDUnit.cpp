// © 2021 Alessandro Arena, Mattia De Prisco, Nunzio Lopardo All Rights Reserved


#include "GDUnit.h"

#include <UIAutomationCore.h>

#include "NavigationSystem.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Components/Button.h"
#include "Core/Private/Windows/WindowsEvent.h"
#include "GameFramework/PlayerInput.h"
#include "GDProject/Components/GDHealthComponent.h"
#include "GDProject/Tiles/GDTile.h"
#include "Kismet/KismetMathLibrary.h"


AGDUnit::AGDUnit()
{
	PrimaryActorTick.bCanEverTick = true;

	HealthComponent = CreateDefaultSubobject<UGDHealthComponent>(TEXT("HealthComp"));
	HealthComponent->OnHealthChanged.AddDynamic(this, &AGDUnit::OnHealthChanged);

	GetMesh()->SetRenderCustomDepth(true);

	MaxActionPoints = 2;

	MovementRange = 5;

	AttackRange = 5;
	BaseDamage = 10.f;
	HitChance = 90.f;
	CriticalChance = 10.f;
	CriticalChanceAdjuster = 0.f;
	CriticalDamageMultiplier = 2.f;

	bIsDead = false;

	bMoveRequested = false;
	bRotationRequested = false;
}

void AGDUnit::BeginPlay()
{
	Super::BeginPlay();

	CurrentActionPoints = MaxActionPoints;
}

void AGDUnit::OnHealthChanged(UGDHealthComponent* HealthComp, float Health, float HealthDelta,
                              const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
	UE_LOG(LogTemp, Warning, TEXT("%s received %f damage!"), *GetName(), HealthDelta);
	if (!bIsDead && Health <= 0.f)
	{
		Die();
	}
}

void AGDUnit::Die()
{
	UE_LOG(LogTemp, Warning, TEXT("%s died!"), *GetName());
	bIsDead = true;
	SetLifeSpan(3.f);

	CurrentTile->SetTileElement(nullptr);
}

void AGDUnit::PerformMove(float DeltaTime)
{
	if (PathToFollow.Num() > 0)
	{
		FVector NextTileLocation = PathToFollow[0]->GetActorLocation();
		NextTileLocation.Z = GetActorLocation().Z;

		if (GetActorLocation().Equals(NextTileLocation))
		{
			PathToFollow.RemoveAt(0);
		}
		else
		{
			SetActorRotation(UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), NextTileLocation));
			SetActorLocation(
				UKismetMathLibrary::VInterpTo_Constant(GetActorLocation(), NextTileLocation, DeltaTime, 600.f));
		}
	}
	else
	{
		StopMove();
	}
}

void AGDUnit::PerformRotation(float DeltaTime)
{
	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	if (!PC)
	{
		UE_LOG(LogTemp, Warning, TEXT("Player Controller null"));
		return;
	}
	FVector Start, Dir;
    PC->DeprojectMousePositionToWorld(Start, Dir);
	FHitResult HitResult;
	const FVector End = Start + Dir * 8000.f;
	GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility);
	FRotator CurrentUnitRotation = GetActorRotation();
	FRotator RotationOffset = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), HitResult.Location);
	FRotator NewRotation = FRotator(CurrentUnitRotation.Pitch, RotationOffset.Yaw, CurrentUnitRotation.Roll);
	this->SetActorRotation(NewRotation);
}

void AGDUnit::StopMove()
{
	PathToFollow.Empty();
	bMoveRequested = false;
	bRotationRequested = true;
}

void AGDUnit::DecreaseActionPointsBy(const int Value)
{
	CurrentActionPoints = FMath::Clamp(CurrentActionPoints - Value, 0, MaxActionPoints);
}

void AGDUnit::Tick(float DeltaTime)
{
	if (bMoveRequested)
	{
		PerformMove(DeltaTime);
	}
	if (bRotationRequested)
	{
		PerformRotation(DeltaTime);
	}
	
}

AGDTile* AGDUnit::GetTile_Implementation()
{
	return CurrentTile;
}

void AGDUnit::SetTile_Implementation(AGDTile* Tile)
{
	if (!Tile)
	{
		return;
	}

	if (CurrentTile)
	{
		CurrentTile->SetTileElement(nullptr);
	}
	CurrentTile = Tile;
	CurrentTile->SetTileElement(this);
}

bool AGDUnit::CanBeSelected_Implementation()
{
	return true;
}

bool AGDUnit::CanMove_Implementation()
{
	return true;
}

void AGDUnit::RequestMoveToTile(AGDTile* TargetTile)
{
	if (!TargetTile)
	{
		return;
	}

	FVector Destination = TargetTile->GetActorLocation();
	Destination.Z = GetActorLocation().Z;

	UAIBlueprintHelperLibrary::SimpleMoveToLocation(GetController(), Destination);
	SetTile_Implementation(TargetTile);
}

void AGDUnit::RequestMoveAlongPath(const TArray<AGDTile*>& Path)
{
	if (Path.Num() < 1 || Path.Num() > MovementRange * CurrentActionPoints)
	{
		return;
	}

	Path.Num() <= MovementRange ? DecreaseActionPointsBy(1) : DecreaseActionPointsBy(2);

	bMoveRequested = true;
	PathToFollow = Path;
	IGDTileElement::Execute_SetTile(this, Path.Top());
}

void AGDUnit::UpdateTransparency() const
{
	if (CurrentActionPoints == 0 && InactiveMaterial)
	{
		GetMesh()->SetMaterial(0, InactiveMaterial);
		GetMesh()->SetCastShadow(false);
	}
}

void AGDUnit::Attack(AGDUnit* Enemy)
{
	if (!Enemy || CurrentActionPoints <= 0 || CurrentTile->GetDistanceFrom(IGDTileElement::Execute_GetTile(Enemy)) >
		AttackRange)
	{
		return;
	}

	SetActorRotation(UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), Enemy->GetActorLocation()));

	DecreaseActionPointsBy(MaxActionPoints);

	const bool Miss = FMath::FRandRange(0.f, 100.f) > HitChance;
	if (Miss)
	{
		UE_LOG(LogTemp, Warning, TEXT("Missed!"));
	}
	else
	{
		float Damage = BaseDamage;
		const bool CriticalHit = FMath::FRandRange(0.f, 100.f) <= CriticalChance + CriticalChanceAdjuster;
		if (!CriticalHit)
		{
			CriticalChanceAdjuster += CriticalChance;
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Critical hit!"));
			CriticalChanceAdjuster = 0.f;
			Damage *= CriticalDamageMultiplier;
		}
		Enemy->TakeDamage(Damage, FDamageEvent{}, GetController(), this);
	}

	UpdateTransparency();
}

void AGDUnit::Rotate()
{
	UE_LOG(LogTemp, Warning, TEXT("Stop rotation"));
	bRotationRequested = false;
	FRotator NewRotation(0, 0, 0);
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Orange, FString::Printf(TEXT("My Location is: %s"), *GetActorRotation().ToString()));
	if (GetActorRotation().Yaw > -45 && GetActorRotation().Yaw <= 45)
	{
		SetActorRotation(NewRotation);
		
	} else if (GetActorRotation().Yaw > 45 && GetActorRotation().Yaw <= 135)
	{
		NewRotation.Yaw = 90.0f;
		SetActorRotation(NewRotation);
	} else if ((GetActorRotation().Yaw > 135 && GetActorRotation().Yaw < 180) || (GetActorRotation().Yaw < -135 && GetActorRotation().Yaw > -180))
	{
		NewRotation.Yaw = 180.0f;
		SetActorRotation(NewRotation);
	} else
	{
		NewRotation.Yaw = -90.0f;
		SetActorRotation(NewRotation);
	}

}

int AGDUnit::GetMovementRange() const
{
	return MovementRange;
}

int AGDUnit::GetActionPoints() const
{
	return CurrentActionPoints;
}

int AGDUnit::GetAttackRange() const
{
	return AttackRange;
}

void AGDUnit::ResetActionPoints()
{
	CurrentActionPoints = MaxActionPoints;
	if (ActiveMaterial)
	{
		GetMesh()->SetMaterial(0, ActiveMaterial);
		GetMesh()->SetCastShadow(true);
	}
}
