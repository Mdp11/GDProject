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
				UKismetMathLibrary::VInterpTo_Constant(GetActorLocation(), NextTileLocation, DeltaTime,
				                                       bIsWalking ? 250.f : 600.f));
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

	if (TargetToAttackAfterMove)
	{
		RequestAttack(TargetToAttackAfterMove, false);
	}
	else
	{
		bRotationRequested = true;
	}

	UpdateTransparency();
	OnActionFinished();
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

void AGDUnit::Rotate()
{
	bRotationRequested = false;
	FRotator NewRotation(0, 0, 0);
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Orange,
	                                 FString::Printf(TEXT("My Rotation is: %s"), *GetActorRotation().ToString()));
	if (GetActorRotation().Yaw > -45 && GetActorRotation().Yaw <= 45)
	{
		SetActorRotation(NewRotation);
	}
	else if (GetActorRotation().Yaw > 45 && GetActorRotation().Yaw <= 135)
	{
		NewRotation.Yaw = 90.0f;
		SetActorRotation(NewRotation);
	}
	else if ((GetActorRotation().Yaw > 135 && GetActorRotation().Yaw < 180) || (GetActorRotation().Yaw < -135 &&
		GetActorRotation().Yaw > -180))
	{
		NewRotation.Yaw = 180.0f;
		SetActorRotation(NewRotation);
	}
	else
	{
		NewRotation.Yaw = -90.0f;
		SetActorRotation(NewRotation);
	}
	OnActionFinished();
}

bool AGDUnit::IsEnemy(AGDUnit* OtherUnit) const
{
	return OwningPlayer != OtherUnit->OwningPlayer;
}

void AGDUnit::SetOwningPlayer(const int NewOwningPlayer)
{
	OwningPlayer = NewOwningPlayer;
}

bool AGDUnit::IsOwnedByPlayer(const int Player) const
{
	return Player == OwningPlayer;
}

void AGDUnit::OnTurnBegin()
{
	CurrentActionPoints = MaxActionPoints;
	RemoveSpecial();
}

void AGDUnit::OnTurnEnd()
{
}

void AGDUnit::DecreaseActionPointsBy(const int Value)
{
	CurrentActionPoints = FMath::Clamp(CurrentActionPoints - Value, 0, MaxActionPoints);
}

void AGDUnit::Powerup()
{
	AddToActiveUnits();
	float PowerupAnimationDuration = PlayAnimMontage(PowerupAnimation) + 0.01f;

	FTimerHandle TimerHandle_Powerup;
	GetWorldTimerManager().SetTimer(TimerHandle_Powerup, this, &AGDUnit::OnActionFinished, PowerupAnimationDuration);
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

void AGDUnit::Select_Implementation()
{
	HighlightMovementRange();
	HighlightEnemiesInAttackRange();
}

void AGDUnit::Deselect_Implementation()
{
	ResetAllHighlightedTiles();
}

bool AGDUnit::RequestMove()
{
	if (MovementPath.Num() < 1 || MovementPath.Num() > MovementRange * CurrentActionPoints)
	{
		return false;
	}

	MovementPath.Num() <= MovementRange ? DecreaseActionPointsBy(1) : DecreaseActionPointsBy(2);

	bMoveRequested = true;

	AddToActiveUnits();

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

bool AGDUnit::RequestAttack(AGDUnit* Enemy, bool bIgnoreActionPoints)
{
	bool bAttackPerformed = false;

	if (Enemy && (bIgnoreActionPoints || CurrentActionPoints > 0))
	{
		AddToActiveUnits();

		bAttackPerformed = true;

		LastAttackedEnemy = Enemy;

		SetActorRotation(UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), Enemy->GetActorLocation()));

		if (!bIgnoreActionPoints)
		{
			DecreaseActionPointsBy(MaxActionPoints);
		}

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

			UAnimMontage* AttackAnimation;

			const bool CriticalHit = FMath::FRandRange(0.f, 100.f) <= CriticalChance + CurrentTile->
				GetCriticalChanceModifier() + CriticalChanceAdjuster;
			if (!CriticalHit)
			{
				AttackAnimation = FMath::RandBool() ? BaseAttackAnimation : AlternativeAttackAnimation;
				CriticalChanceAdjuster += CriticalChance + CurrentTile->GetCriticalChanceModifier();
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("Critical hit!"));
				AttackAnimation = CriticalAttackAnimation;
				CriticalChanceAdjuster = 0.f;
				Damage *= CriticalDamageMultiplier;
			}

			Damage /= Enemy->GetDefence();

			Enemy->TakeDamage(Damage, FDamageEvent{}, GetController(), this);

			AttackAnimationDuration += PlayAnimMontage(AttackAnimation);
		}


		FTimerHandle TimerHandle_AttackOver;
		GetWorldTimerManager().SetTimer(TimerHandle_AttackOver, this, &AGDUnit::OnActionFinished,
		                                AttackAnimationDuration);


		UpdateTransparency();
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

void AGDUnit::ResetHighlightedTilesInRange()
{
	for (const auto& Tile : HighlightedTilesInShortRange)
	{
		Tile->RemoveInfoDecal();
	}

	for (const auto& Tile : HighlightedTilesInLongRange)
	{
		Tile->RemoveInfoDecal();
	}

	for (const auto& Tile : HighlightedEnemyTilesInRange)
	{
		Tile->RemoveInfoDecal();
	}

	HighlightedTilesInShortRange.Empty();
	HighlightedTilesInLongRange.Empty();
	HighlightedEnemyTilesInRange.Empty();
}

void AGDUnit::ResetHighlightedActionTiles()
{
	if (HighlightedEnemyTile)
	{
		HighlightedEnemyTile->RemoveHighlight();
		HighlightedEnemyTile = nullptr;
	}

	for (const auto& Tile : MovementPath)
	{
		Tile->RemoveHighlight();
	}
}

void AGDUnit::ResetAllHighlightedTiles()
{
	ResetHighlightedTilesInRange();

	ResetHighlightedActionTiles();
}

void AGDUnit::AddToActiveUnits()
{
	if (AGDPlayerPawn* PlayerPawn = Cast<AGDPlayerPawn>(GetWorld()->GetFirstPlayerController()->GetPawn()))
	{
		PlayerPawn->AddActiveUnit(this);
	}
}

void AGDUnit::RemoveFromActiveUnits()
{
	if (AGDPlayerPawn* PlayerPawn = Cast<AGDPlayerPawn>(GetWorld()->GetFirstPlayerController()->GetPawn()))
	{
		PlayerPawn->RemoveActiveUnit(this);
	}
}


void AGDUnit::HighlightMovementPath(AGDTile* TargetTile, float StopAtDistance)
{
	TArray<AGDTile*> NewMovementPath = CurrentTile->GetGrid()->ComputePathBetweenTiles(
		CurrentTile, TargetTile, StopAtDistance);
	if (NewMovementPath.Num() > 1)
	{
		NewMovementPath.RemoveAt(0); //First tile is the one unit is on, so it can be removed

		const bool bCanReachAndAttackEnemy = TargetTile->IsOccupiedByEnemy(this) && CurrentActionPoints > 1 &&
			NewMovementPath.
			Num() <= GetMovementRange();

		const bool bCanReachEmptyTile = !TargetTile->IsOccupied() &&
			NewMovementPath.Num() <= GetMovementRange() * GetActionPoints();

		if (bCanReachAndAttackEnemy || bCanReachEmptyTile)
		{
			MovementPath = MoveTemp(NewMovementPath);
			for (const auto& Tile : MovementPath)
			{
				Tile->Highlight(EHighlightInfo::Default);
			}
		}
		else
		{
			MovementPath.Empty();
		}
	}
}

void AGDUnit::HighlightActions(AGDTile* TargetTile)
{
	ResetHighlightedActionTiles();

	if (TargetTile && TargetTile->IsTraversable())
	{
		float StopAtDistance = 0;

		if (TargetTile->IsOccupiedByEnemy(this))
		{
			HighlightedEnemyTile = TargetTile;
			HighlightedEnemyTile->Highlight(EHighlightInfo::Default);

			StopAtDistance = AttackRange;
		}

		HighlightMovementPath(TargetTile, StopAtDistance);
	}
}

bool AGDUnit::RequestMoveAndAttack(AGDUnit* Enemy)
{
	if (CurrentActionPoints > 1 && MovementPath.Num() > 0 && MovementRange >= MovementPath.Num())
	{
		TargetToAttackAfterMove = Enemy;

		RequestMove();
		return true;
	}
	return false;
}

void AGDUnit::RequestAction(AGDTile* TargetTile)
{
	bool bActionPerformed = false;

	if (TargetTile)
	{
		if (!TargetTile->IsOccupied())
		{
			bActionPerformed = RequestMove();
		}
		else if (AGDUnit* TargetUnit = Cast<AGDUnit>(TargetTile->GetTileElement()))
		{
			if (IsEnemy(TargetUnit))
			{
				if (CurrentTile->GetDistanceFrom(TargetTile) <= AttackRange)
				{
					bActionPerformed = RequestAttack(TargetUnit);
				}
				else
				{
					bActionPerformed = RequestMoveAndAttack(TargetUnit);
				}
			}
		}
	}

	if (bActionPerformed)
	{
		ResetAllHighlightedTiles();
	}
}

bool AGDUnit::IsUnitRotating()
{
	return bRotationRequested;
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
				if (PathLength > 0 && PathLength <= GetMovementRange() * GetActionPoints())
				{
					const bool bShortDistance = PathLength <= GetMovementRange() ? true : false;

					Tile->ApplyMovementRangeInfoDecal(bShortDistance);

					if (bShortDistance)
					{
						HighlightedTilesInShortRange.Emplace(MoveTemp(Tile));
					}
					else
					{
						HighlightedTilesInLongRange.Emplace(MoveTemp(Tile));
					}
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
			if (Tile->IsOccupiedByEnemy(this))
			{
				Tile->ApplyEnemyInfoDecal();
				HighlightedEnemyTilesInRange.Emplace(MoveTemp(Tile));
			}
		}

		if (UnitActionPoints > 1)
		{
			for (const auto& ReachableTile : HighlightedTilesInShortRange)
			{
				TilesInAttackRange = Grid->GetTilesAtDistance(
					ReachableTile, GetAttackRange());

				for (auto& Tile : TilesInAttackRange)
				{
					if (Tile->IsOccupiedByEnemy(this))
					{
						Tile->ApplyEnemyInfoDecal();
						HighlightedEnemyTilesInRange.Emplace(MoveTemp(Tile));
					}
				}
			}
		}
	}
}

void AGDUnit::OnActionFinished()
{
	RemoveFromActiveUnits();
	TargetToAttackAfterMove = nullptr;
}
