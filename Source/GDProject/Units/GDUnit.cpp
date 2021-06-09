// © 2021 Alessandro Arena, Mattia De Prisco, Nunzio Lopardo All Rights Reserved


#include "GDUnit.h"

#include "NavigationSystem.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Blueprint/UserWidget.h"
#include "GDProject/Components/GDHealthComponent.h"
#include "GDProject/Player/GDPlayerPawn.h"
#include "GDProject/Tiles/GDTile.h"
#include "GDProject/Tiles/GDGrid.h"
#include "Kismet/KismetMathLibrary.h"


AGDUnit::AGDUnit()
{
	PrimaryActorTick.bCanEverTick = true;

	HealthComponent = CreateDefaultSubobject<UGDHealthComponent>(TEXT("HealthComp"));
	HealthComponent->OnHealthChanged.AddDynamic(this, &AGDUnit::OnHealthChanged);

	WidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("WidgetComp"));
	WidgetComponent->SetupAttachment(GetMesh());
	WidgetComponent->SetDrawAtDesiredSize(true);
	WidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);

	GetMesh()->SetRenderCustomDepth(true);

	MaxActionPoints = 2;

	MovementRange = 5;

	BaseDamage = 10.f;
	AttackRange = 5;
	HitChance = 90.f;

	CriticalChance = 10.f;
	CriticalChanceAdjuster = 0.f;
	CriticalDamageMultiplier = 2.f;

	Defence = 1;

	bIsDead = false;

	bMoveRequested = false;
}

void AGDUnit::BeginPlay()
{
	Super::BeginPlay();

	CurrentActionPoints = MaxActionPoints;

	if (UUserWidget* SpecialWidget = WidgetComponent->GetUserWidgetObject())
	{
		SpecialWidget->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void AGDUnit::OnHealthChanged(UGDHealthComponent* HealthComp, float Health, float HealthDelta,
                              const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
	UE_LOG(LogTemp, Warning, TEXT("%s received %f damage and now has %f!"), *GetName(), HealthDelta, Health);
	if (!bIsDead && Health <= 0.f)
	{
		Die();
	}
}

void AGDUnit::RemoveSpecial()
{
}

void AGDUnit::Die()
{
	UE_LOG(LogTemp, Warning, TEXT("%s died!"), *GetName());
	bIsDead = true;
	SetLifeSpan(5.f);

	CurrentTile->SetTileElement(nullptr);
}

void AGDUnit::PerformMove(float DeltaTime)
{
	if (MovementPath.Num() > 0)
	{
		FVector NextTileLocation = MovementPath[0]->GetActorLocation();
		NextTileLocation.Z = GetActorLocation().Z;

		if (GetActorLocation().Equals(NextTileLocation))
		{
			MovementPath.RemoveAt(0);
		}
		else
		{
			SetActorRotation(UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), NextTileLocation));
			SetActorLocation(
				UKismetMathLibrary::VInterpTo_Constant(GetActorLocation(), NextTileLocation, DeltaTime, bIsWalking ? 250.f : 600.f));
		}
	}
	else
	{
		StopMove();
	}
}

void AGDUnit::StopMove()
{
	bMoveRequested = false;
	UpdateTransparency();
	OnActionFinished();
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

void AGDUnit::Select_Implementation()
{
	if (UUserWidget* SpecialWidget = WidgetComponent->GetUserWidgetObject())
	{
		SpecialWidget->SetVisibility(ESlateVisibility::Visible);
	}

	HighlightMovementRange();
	HighlightEnemiesInAttackRange();
}

void AGDUnit::Deselect_Implementation()
{
	if (UUserWidget* SpecialWidget = WidgetComponent->GetUserWidgetObject())
	{
		SpecialWidget->SetVisibility(ESlateVisibility::Collapsed);
	}

	ResetAllHighlightedTiles();
}

bool AGDUnit::RequestMove()
{
	if (MovementPath.Num() < 1 || MovementPath.Num() > MovementRange * CurrentActionPoints)
	{
		OnActionFinished();
		return false;
	}

	MovementPath.Num() <= MovementRange ? DecreaseActionPointsBy(1) : DecreaseActionPointsBy(2);

	bMoveRequested = true;

	bIsWalking = MovementPath.Num() <= 2;
	
	CurrentTile->Deselect();
	IGDTileElement::Execute_SetTile(this, MovementPath.Top());
	
	return true;
}

float AGDUnit::GetDefence() const
{
	return Defence + CurrentTile->GetDefenceModifier();
}

void AGDUnit::UpdateTransparency() const
{
	if (CurrentActionPoints == 0 && InactiveMaterial)
	{
		GetMesh()->SetMaterial(0, InactiveMaterial);
		GetMesh()->SetCastShadow(false);
	}
}

bool AGDUnit::Attack(AGDUnit* Enemy, bool bIgnoreActionPoints)
{
	bool bAttackPerformed = false;

	if (Enemy && (bIgnoreActionPoints || CurrentActionPoints > 0)
		&& CurrentTile->GetDistanceFrom(IGDTileElement::Execute_GetTile(Enemy)) <= AttackRange)
	{
		bAttackPerformed = true;

		AttackedEnemy = Enemy;

		SetActorRotation(UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), Enemy->GetActorLocation()));

		DecreaseActionPointsBy(MaxActionPoints);

		float AttackAnimationDuration = 0.01f;
		
		const bool Miss = FMath::FRandRange(0.f, 100.f) > HitChance + CurrentTile->GetHitChanceModifier();
		if (Miss)
		{
			UE_LOG(LogTemp, Warning, TEXT("Missed!"));
			Enemy->TakeDamage(0.f, FDamageEvent{}, GetController(), this);

			AttackAnimationDuration += PlayAnimMontage(MissAnimation);
		}
		else
		{
			float Damage = BaseDamage + CurrentTile->GetAttackModifier();
			const bool CriticalHit = FMath::FRandRange(0.f, 100.f) <= CriticalChance + CurrentTile->
				GetCriticalChanceModifier() + CriticalChanceAdjuster;
			if (!CriticalHit)
			{
				CriticalChanceAdjuster += CriticalChance + CurrentTile->GetCriticalChanceModifier();
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("Critical hit!"));
				CriticalChanceAdjuster = 0.f;
				Damage *= CriticalDamageMultiplier;
			}

			Damage /= Enemy->GetDefence();

			Enemy->TakeDamage(Damage, FDamageEvent{}, GetController(), this);

			AttackAnimationDuration += FMath::RandBool() ? PlayAnimMontage(AttackAnimation) : PlayAnimMontage(AlternativeAttackAnimation);

		}

		FTimerHandle TimerHandle_AttackOver;
		GetWorldTimerManager().SetTimer(TimerHandle_AttackOver, this, &AGDUnit::OnActionFinished, AttackAnimationDuration);
		
		UpdateTransparency();
	}
	else
	{
		OnActionFinished();
	}


	return bAttackPerformed;
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

void AGDUnit::ResetAllHighlightedTiles()
{
	for (const auto& Tile : HighlightedTilesInRange)
	{
		Tile->RemoveInfoDecal();
	}

	HighlightedTilesInRange.Empty();

	ResetHighlightedActionTiles();
}

void AGDUnit::ResetHighlightedActionTiles()
{
	if (HighlightedEnemyTile)
	{
		HighlightedEnemyTile->HighlightTargetEnemy(false);
		HighlightedEnemyTile = nullptr;
	}

	for (const auto& Tile : MovementPath)
	{
		Tile->Highlight(false);
	}
}

void AGDUnit::HighlightAction(AGDTile* TargetTile)
{
	ResetHighlightedActionTiles();

	if (TargetTile->IsTraversable())
	{
		if (TargetTile->IsOccupied() /* TODO: && TargetTile contains enemy */)
		{
			HighlightedEnemyTile = TargetTile;
			HighlightedEnemyTile->HighlightTargetEnemy(true);
		}
		else
		{
			AGDGrid* Grid = CurrentTile->GetGrid();

			TArray<AGDTile*> NewMovementPath = Grid->ComputePathBetweenTiles(CurrentTile, TargetTile);
			if (NewMovementPath.Num() > 0)
			{
				NewMovementPath.RemoveAt(0); // First tile is the one unit is on, so it can be removed
				if (NewMovementPath.Num() <= GetMovementRange() * GetActionPoints())
				{
					StoreAndHighlightedCurrentComputedPath(MoveTemp(NewMovementPath));
				}
				else
				{
					MovementPath.Empty();
				}
			}
		}
	}
}

void AGDUnit::PerformAction(AGDTile* TargetTile)
{
	if (TargetTile)
	{
		bool bActionPerformed = false;

		if (TargetTile->IsOccupied())
		{
			if (AGDUnit* TargetUnit = Cast<AGDUnit>(TargetTile->GetTileElement()))
			{
				/* TODO: If Target unit is enemy */
				bActionPerformed = Attack(TargetUnit);
			}
		}
		else
		{
			bActionPerformed = RequestMove();
		}

		if (bActionPerformed)
		{
			ResetAllHighlightedTiles();
		}
	}
	else
	{
		OnActionFinished();
	}
}

void AGDUnit::HighlightMovementRange()
{
	AGDGrid* Grid = CurrentTile->GetGrid();

	const int UnitActionPoints = GetActionPoints();

	if (UnitActionPoints > 0)
	{
		TSet<AGDTile*> TilesInMovementRange = Grid->GetTilesAtDistance(
			CurrentTile, GetMovementRange() * GetActionPoints());

		for (auto& Tile : TilesInMovementRange)
		{
			if (Tile->IsTraversable() && !Tile->IsOccupied())
			{
				const int PathLength = Grid->ComputePathBetweenTiles(CurrentTile, Tile).Num() - 1;
				if (PathLength <= GetMovementRange() * GetActionPoints())
				{
					const bool bShortDistance = PathLength <= GetMovementRange() ? true : false;
					Tile->ApplyMovementRangeInfoDecal(bShortDistance);

					HighlightedTilesInRange.Emplace(MoveTemp(Tile));
				}
			}
		}
	}
}

void AGDUnit::HighlightEnemiesInAttackRange()
{
	AGDGrid* Grid = CurrentTile->GetGrid();

	const int UnitActionPoints = GetActionPoints();

	if (UnitActionPoints > 0)
	{
		TSet<AGDTile*> TilesInAttackRange = Grid->GetTilesAtDistance(
			CurrentTile, GetAttackRange());

		for (auto& Tile : TilesInAttackRange)
		{
			if (Tile->IsOccupied())
			{
				Tile->ApplyEnemyInfoDecal();
				HighlightedTilesInRange.Emplace(MoveTemp(Tile));
			}
		}
	}
}

void AGDUnit::OnActionFinished() const
{
	AGDPlayerPawn* PlayerPawn = Cast<AGDPlayerPawn>(GetWorld()->GetFirstPlayerController()->GetPawn());

	if (PlayerPawn)
	{
		PlayerPawn->ActionFinished(CurrentTile);
	}
}
