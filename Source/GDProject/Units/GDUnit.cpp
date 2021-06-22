// © 2021 Alessandro Arena, Mattia De Prisco, Nunzio Lopardo All Rights Reserved


#include "GDUnit.h"

#include <algorithm>

#include "GDProject/GDProjectGameModeBase.h"
#include "GDProject/Components/GDHealthComponent.h"
#include "GDProject/Player/GDPlayerPawn.h"
#include "GDProject/Tiles/GDTile.h"
#include "GDProject/Tiles/GDGrid.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/CapsuleComponent.h"


AGDUnit::AGDUnit()
{
	PrimaryActorTick.bCanEverTick = true;

	HealthComponent = CreateDefaultSubobject<UGDHealthComponent>(TEXT("HealthComp"));
	HealthComponent->OnHealthChanged.AddDynamic(this, &AGDUnit::OnHealthChanged);

	// GetMesh()->SetRenderCustomDepth(true);

	static ConstructorHelpers::FObjectFinder<UMaterialInstance> OutlineMaterialFinder(
		TEXT("MaterialInstanceConstant'/Game/Materials/MI_Outline.MI_Outline'"));
	OutlineMaterialInstance = OutlineMaterialFinder.Object;

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

	LifeSpanOnDeath = 5.f;
}

void AGDUnit::CheckAnimations()
{
	if (!BaseAttackAnimation)
	{
		UE_LOG(LogTemp, Warning, TEXT("Base attack animation not set for actor %s"), *GetName());
	}
	else
	{
		if (!AlternativeAttackAnimation)
		{
			AlternativeAttackAnimation = BaseAttackAnimation;
		}
		if (!CriticalAttackAnimation)
		{
			CriticalAttackAnimation = BaseAttackAnimation;
		}
		if (!MissAnimation)
		{
			MissAnimation = BaseAttackAnimation;
		}
	}
}

void AGDUnit::BeginPlay()
{
	Super::BeginPlay();

	CheckAnimations();

	ResetActionPoints();
}

void AGDUnit::OnHealthChanged(UGDHealthComponent* HealthComp, float Health, float HealthDelta,
                              const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
	UE_LOG(LogTemp, Warning, TEXT("%s received %f damage and now has %f!"), *GetName(), HealthDelta, Health);
	if (!bIsDead && Health <= 0.f)
	{
		Die();
	}
	else
	{
		PlayAnimationAndDoAction(ImpactAnimation, [&]() { OnActionFinished(); });
	}
}

void AGDUnit::RemoveSpecial()
{
}

void AGDUnit::Die()
{
	UE_LOG(LogTemp, Warning, TEXT("%s died!"), *GetName());
	bIsDead = true;

	CurrentTile->SetTileElement(nullptr);

	Execute_Deselect(this);

	SetLifeSpan(LifeSpanOnDeath);

	TArray<AActor*> AttachedActors;
	GetAttachedActors(AttachedActors);
	for (auto& Actor : AttachedActors)
	{
		Actor->SetLifeSpan(LifeSpanOnDeath);
	}

	FTimerHandle TimerHandle_OnUnitDead;
	FTimerDelegate TimerDelegate;
	TimerDelegate.BindLambda([&]()
	{
		if (AGDPlayerPawn* PlayerPawn = Cast<AGDPlayerPawn>(GetWorld()->GetFirstPlayerController()->GetPawn()))
		{
			PlayerPawn->OnUnitDead(this, OwningPlayer);
		}
	});
	GetWorldTimerManager().SetTimer(TimerHandle_OnUnitDead, TimerDelegate, LifeSpanOnDeath - 0.5f, false);
}

void AGDUnit::PerformMove(float DeltaTime)
{
	if (MovementPath.Num() > 0)
	{
		FVector NextTileLocation = MovementPath[0]->GetActorLocation();
		NextTileLocation.Z = GetActorLocation().Z;

		if (GetActorLocation().Equals(NextTileLocation))
		{
			AGDTile* ReachedTile = MovementPath[0];
			Execute_SetTile(this, ReachedTile);
			MovementPath.RemoveAt(0);

			if (ReachedTile->HasGuardingUnits())
			{
				for (auto& Unit : ReachedTile->GetGuardingUnits())
				{
					if(IsEnemy(Unit))
					{
						Unit->RequestAttack(this, true);
					}
				}
			}
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

	CurrentTile->Select();
	Execute_Select(this);

	if (TargetToAttackAfterMove)
	{
		RequestAttack(TargetToAttackAfterMove, false);
	}
	else
	{
		bRotationRequested = true;
	}

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

	const FRotator CurrentUnitRotation = GetActorRotation();
	const FRotator RotationOffset = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), HitResult.Location);
	const FRotator NewRotation = FRotator(CurrentUnitRotation.Pitch, RotationOffset.Yaw, CurrentUnitRotation.Roll);
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
	ResetActionPoints();
	RemoveSpecial();
}

void AGDUnit::OnTurnEnd() const
{
}

void AGDUnit::DecreaseActionPointsBy(const int Value)
{
	CurrentActionPoints = FMath::Clamp(CurrentActionPoints - Value, 0, MaxActionPoints);
}

void AGDUnit::PowerUp()
{
	PlayAnimationAndDoAction(PowerUpAnimation, [&]() { OnActionFinished(); });
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
	AddOutline(FLinearColor::Green);
	HighlightMovementRange();
	HighlightEnemiesInAttackRange();
}

void AGDUnit::Deselect_Implementation()
{
	RemoveOutline();
	ResetAllHighlightedTiles();
}

void AGDUnit::RequestMove()
{
	if (MovementPath.Num() >= 1 && MovementPath.Num() <= MovementRange * CurrentActionPoints)
	{
		OnActionBegin();

		MovementPath.Num() <= MovementRange ? DecreaseActionPointsBy(1) : DecreaseActionPointsBy(2);

		bMoveRequested = true;

		bIsWalking = MovementPath.Num() <= 2;

		CurrentTile->Deselect();
	}
}

float AGDUnit::GetDefence() const
{
	return Defence + CurrentTile->GetDefenceModifier();
}

bool AGDUnit::CanAttackUnit(AGDUnit* Enemy, const bool bIgnoreActionPoints) const
{
	return Enemy && IsTileInAttackRange(Execute_GetTile(Enemy))
		&& (bIgnoreActionPoints || CurrentActionPoints > 0);
}

bool AGDUnit::IsTileInAttackRange(AGDTile* Tile) const
{
	return IsTileInAttackRangeFromTile(Tile, CurrentTile);
}

bool AGDUnit::IsTileInAttackRangeFromTile(AGDTile* SourceTile, AGDTile* TargetTile) const
{
	return (SourceTile->GetCoordinates().X == TargetTile->GetCoordinates().X && FMath::Abs(
				SourceTile->GetCoordinates().Y - TargetTile->GetCoordinates().Y) <= AttackRange ||
			SourceTile->GetCoordinates().Y == TargetTile->GetCoordinates().Y && FMath::Abs(
				SourceTile->GetCoordinates().X - TargetTile->GetCoordinates().X) <= AttackRange)
		&& SourceTile->IsPathClearTowardsTile(TargetTile);
}

bool AGDUnit::IsCriticalHit()
{
	const bool bCriticalHit = FMath::FRandRange(0.f, 100.f)
		<= CriticalChance + CurrentTile->GetCriticalChanceModifier() + CriticalChanceAdjuster;
	if (!bCriticalHit)
	{
		CriticalChanceAdjuster += CriticalChance + CurrentTile->GetCriticalChanceModifier();
	}
	else
	{
		CriticalChanceAdjuster = 0.f;
	}

	return bCriticalHit;
}

void AGDUnit::ApplyDamage()
{
	AttackedEnemy->TakeDamage(ComputedDamage, FDamageEvent{}, GetController(), this);
}

bool AGDUnit::Miss()
{
	return FMath::FRandRange(0.f, 100.f) > HitChance + CurrentTile->GetHitChanceModifier();
}

void AGDUnit::Attack()
{
	ComputedDamage = 0.f;
	UAnimMontage* AttackAnimation;

	if (Miss())
	{
		UE_LOG(LogTemp, Warning, TEXT("Missed!"));
		AttackAnimation = MissAnimation;
	}
	else
	{
		ComputedDamage = BaseDamage + CurrentTile->GetAttackModifier();

		if (!IsCriticalHit())
		{
			AttackAnimation = FMath::RandBool() ? BaseAttackAnimation : AlternativeAttackAnimation;
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Critical hit!"));
			AttackAnimation = CriticalAttackAnimation;
			ComputedDamage *= CriticalDamageMultiplier;
		}

		ComputedDamage /= AttackedEnemy->GetDefence();
	}

	PlayAnimationAndDoAction(AttackAnimation, [&]() { OnActionFinished(); });
}

void AGDUnit::OnActionBegin()
{
	AddToActiveUnits();
	ResetAllHighlightedTiles();
}

void AGDUnit::RequestAttack(AGDUnit* Enemy, const bool bIgnoreActionPoints)
{
	if (CanAttackUnit(Enemy, bIgnoreActionPoints))
	{
		OnActionBegin();

		AttackedEnemy = Enemy;
		SetActorRotation(UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), AttackedEnemy->GetActorLocation()));

		if (!bIgnoreActionPoints)
		{
			DecreaseActionPointsBy(MaxActionPoints);
		}

		Attack();
	}
	else
	{
		OnActionFinished();
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
}

void AGDUnit::ResetHighlightedTilesInRange()
{
	for (const auto& Tile : HighlightedTilesInShortRange)
	{
		Tile->RemoveInfoDecal();
	}
	HighlightedTilesInShortRange.Empty();

	for (const auto& Tile : HighlightedTilesInLongRange)
	{
		Tile->RemoveInfoDecal();
	}
	HighlightedTilesInLongRange.Empty();

	for (const auto& Tile : HighlightedEnemyTilesInRange)
	{
		Tile->RemoveInfoDecal();
	}
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

void AGDUnit::HighlightMovementPath(AGDTile* TargetTile)
{
	TArray<AGDTile*> NewMovementPath = CurrentTile->GetGrid()->ComputePathBetweenTiles(
		CurrentTile, TargetTile);
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

void AGDUnit::HighlightAttackPath(AGDTile* TargetTile)
{
	AGDTile* TileToReach = nullptr;

	for (const auto& Tile : HighlightedTilesInShortRange)
	{
		if (TileToReach)
		{
			if (IsTileInAttackRangeFromTile(Tile, TargetTile)
				&& CurrentTile->GetDistanceFrom(Tile)
				< CurrentTile->GetDistanceFrom(TileToReach))
			{
				TileToReach = Tile;
			}
		}
		else if (IsTileInAttackRangeFromTile(Tile, TargetTile))
		{
			TileToReach = Tile;
		}
	}

	if (TileToReach)
	{
		HighlightMovementPath(TileToReach);
	}
}

void AGDUnit::HighlightActions(AGDTile* TargetTile)
{
	if (!bRotationRequested)
	{
		ResetHighlightedActionTiles();

		if (TargetTile && TargetTile->IsTraversable())
		{
			if (TargetTile->IsOccupiedByEnemy(this))
			{
				if (!IsTileInAttackRange(TargetTile))
				{
					HighlightedEnemyTile = TargetTile;
					HighlightedEnemyTile->Highlight(EHighlightInfo::Enemy);
					HighlightAttackPath(TargetTile);
				}
			}
			else if (IsTileInRangeOfAction(TargetTile))
			{
				HighlightMovementPath(TargetTile);
			}
		}
	}
}

void AGDUnit::RequestMoveAndAttack(AGDUnit* Enemy)
{
	if (CurrentActionPoints > 1 && MovementPath.Num() > 0 && MovementRange >= MovementPath.Num())
	{
		TargetToAttackAfterMove = Enemy;
		RequestMove();
	}
}

void AGDUnit::RequestAction(AGDTile* TargetTile)
{
	if (TargetTile && IsTileInRangeOfAction(TargetTile))
	{
		if (!TargetTile->IsOccupied())
		{
			RequestMove();
		}
		else if (AGDUnit* TargetUnit = Cast<AGDUnit>(TargetTile->GetTileElement()))
		{
			if (IsEnemy(TargetUnit))
			{
				if (IsTileInAttackRange(TargetTile))
				{
					RequestAttack(TargetUnit);
				}
				else
				{
					RequestMoveAndAttack(TargetUnit);
				}
			}
		}
	}
}

bool AGDUnit::IsUnitRotating() const
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
			if (Tile->IsOccupiedByEnemy(this) && IsTileInAttackRange(Tile))
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
					if (Tile->IsOccupiedByEnemy(this) && IsTileInAttackRangeFromTile(Tile, ReachableTile))
					{
						Tile->ApplyEnemyInfoDecal();
						HighlightedEnemyTilesInRange.Emplace(MoveTemp(Tile));
					}
				}
			}
		}
	}
}

bool AGDUnit::IsTileInRangeOfAction(AGDTile* Tile) const
{
	return HighlightedTilesInShortRange.Contains(Tile) || HighlightedTilesInLongRange.Contains(Tile) ||
		HighlightedEnemyTilesInRange.Contains(Tile);
}

void AGDUnit::AddOutline(const FLinearColor& OutlineColor)
{
	if (!OutlineComponent)
	{
		OutlineComponent = Cast<USkeletalMeshComponent>(
			AddComponentByClass(USkeletalMeshComponent::StaticClass(), true, GetMesh()->GetRelativeTransform(), true));
		OutlineComponent->SetupAttachment(GetCapsuleComponent());

		OutlineComponent->SetSkeletalMesh(GetMesh()->SkeletalMesh, false);
		OutlineComponent->SetMasterPoseComponent(GetMesh());

		OutlineComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		OutlineComponent->SetCastShadow(false);

		for (int i = 0; i < OutlineComponent->GetNumMaterials(); ++i)
		{
			UMaterialInstanceDynamic* MaterialInstanceDynamic_First = OutlineComponent->CreateDynamicMaterialInstance(
				i, OutlineMaterialInstance);
			MaterialInstanceDynamic_First->SetVectorParameterValue(TEXT("Color"), OutlineColor);
			MaterialInstanceDynamic_First->SetScalarParameterValue(TEXT("Scale"), 1.f);
		}

		FinishAddComponent(OutlineComponent, true, GetMesh()->GetRelativeTransform());
	}
}

void AGDUnit::RemoveOutline()
{
	if (OutlineComponent)
	{
		OutlineComponent->DestroyComponent();
		OutlineComponent = nullptr;
	}
}

void AGDUnit::OnActionFinished()
{
	RemoveFromActiveUnits();
	TargetToAttackAfterMove = nullptr;
}
