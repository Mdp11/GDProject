// © 2021 Alessandro Arena, Mattia De Prisco, Nunzio Lopardo All Rights Reserved


#include "GDUnit.h"

#include <algorithm>

#include "GDProject/Components/GDHealthComponent.h"
#include "GDProject/Player/GDPlayerPawn.h"
#include "GDProject/Tiles/GDTile.h"
#include "GDProject/Tiles/GDGrid.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"


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

	SideAttackModifier = 0.2f;
	BackAttackModifier = 0.5f;

	CriticalChance = 10.f;
	CriticalChanceAdjuster = 0.f;
	CriticalDamageMultiplier = 2.f;

	Defence = 1;

	bIsDead = false;
	bWarp = false;

	WalkingSpeed = 250.f;
	RunningSpeed = 600.f;

	bMoveRequested = false;
	bMoveInterrupted = false;
	bRotationRequested = false;

	LifeSpanOnDeath = 5.f;

	const FStringAssetReference DeathEffectPath(
		TEXT("/Game/FXVarietyPack/Particles/P_ky_magicCircle1.P_ky_magicCircle1"));
	DeathEffect = Cast<UParticleSystem>(DeathEffectPath.TryLoad());
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
		bMoveInterrupted
			? PlayAnimationAndDoAction(ImpactAnimation, [&]()
			{
				bMoveInterrupted = false;
				bMoveRequested = true;
			})
			: PlayAnimationAndDoAction(ImpactAnimation, [&]() { OnActionFinished(); });
	}
}

void AGDUnit::RemoveSpecial()
{
}

void AGDUnit::Die()
{
	UE_LOG(LogTemp, Warning, TEXT("%s died!"), *GetName());

	PlayAnimationAndDoAction(FMath::RandBool() ? DeathAnimation : AlternativeDeathAnimation, [&]()
	{
		if (DeathEffect)
		{
			DeathParticleSystemComponent = UGameplayStatics::SpawnEmitterAtLocation(
				GetWorld(), DeathEffect, GetMesh()->GetSocketLocation("Spine"));
		}

		FTimerHandle TimerHandle_OnUnitDead;
		GetWorldTimerManager().SetTimer(TimerHandle_OnUnitDead, this, &AGDUnit::PlayDeathEffects, 0.5f, false);
	});
}

void AGDUnit::PlayDeathEffects()
{
	bIsDead = true;

	FTimerHandle TimerHandle_OnUnitDead;
	FTimerDelegate TimerDelegate;
	TimerDelegate.BindLambda([&]()
	{
		if (AGDPlayerPawn* PlayerPawn = Cast<AGDPlayerPawn>(GetWorld()->GetFirstPlayerController()->GetPawn()))
		{
			if (DeathParticleSystemComponent)
			{
				DeathParticleSystemComponent->Deactivate();
			}
			CurrentTile->SetTileElement(nullptr);
			Execute_Deselect(this);
			PlayerPawn->OnUnitDead(this, OwningPlayer);

			OnActionFinished();

			Destroy();
		}
	});
	GetWorldTimerManager().SetTimer(TimerHandle_OnUnitDead, TimerDelegate, 1.f, false);
}

void AGDUnit::CheckForGuardingUnits()
{
	if (CurrentTile->HasGuardingUnits())
	{
		for (auto& Unit : CurrentTile->GetGuardingUnits())
		{
			if (IsEnemy(Unit) && Unit->IsTileInAttackRange(CurrentTile))
			{
				bMoveRequested = false;
				bMoveInterrupted = true;
				Unit->RequestAttack(this, true);
			}
		}
	}
}

void AGDUnit::PerformMove(float DeltaTime)
{
	if (MovementPath.Num() > 0)
	{
		FVector NextTileLocation = MovementPath[0]->GetActorLocation();
		NextTileLocation.Z = GetActorLocation().Z;

		if (GetActorLocation().Equals(NextTileLocation))
		{
			Execute_SetTile(this, MovementPath[0]);
			MovementPath.RemoveAt(0);

			CheckForGuardingUnits();
		}
		else
		{
			SetActorRotation(UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), NextTileLocation));
			SetActorLocation(
				UKismetMathLibrary::VInterpTo_Constant(GetActorLocation(), NextTileLocation, DeltaTime,
				                                       bIsWalking ? WalkingSpeed : RunningSpeed));
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

	if (GetActorRotation().Yaw > -45 && GetActorRotation().Yaw <= 45)
	{
		SetDirection(EDirection::West);
	}
	else if (GetActorRotation().Yaw > 45 && GetActorRotation().Yaw <= 135)
	{
		SetDirection(EDirection::North);
	}
	else if ((GetActorRotation().Yaw > 135 && GetActorRotation().Yaw < 180) || (GetActorRotation().Yaw < -135 &&
		GetActorRotation().Yaw > -180))
	{
		SetDirection(EDirection::East);
	}
	else
	{
		SetDirection(EDirection::South);
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
	if (bIsDead)
	{
		FVector Destination = GetActorLocation();
		Destination.Z -= 100.f;
		SetActorLocation(
			UKismetMathLibrary::VInterpTo_Constant(GetActorLocation(), Destination, DeltaTime,
			                                       30.f));
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

bool AGDUnit::HasFullHealth() const
{
	return HealthComponent->HasFullHealth();
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
		const float AngleDiff = round(abs(AttackedEnemy->GetActorRotation().Yaw - GetActorRotation().Yaw));
		float EnemyDefence = AttackedEnemy->GetDefence();
		float CriticalSideBonus = 0;
		ComputedDamage = BaseDamage + CurrentTile->GetAttackModifier();
		if (AngleDiff == 0)
		{
			CriticalSideBonus = 30;
			EnemyDefence *= BackAttackModifier;
			UE_LOG(LogTemp, Warning, TEXT("Attack from back!"));
		}
		else if (AngleDiff == 90 || AngleDiff == 270)
		{
			CriticalSideBonus = 20;
			EnemyDefence *= SideAttackModifier;
			UE_LOG(LogTemp, Warning, TEXT("Attack from side!"));
		}

		CriticalChance += CriticalSideBonus;

		if (IsCriticalHit())
		{
			UE_LOG(LogTemp, Warning, TEXT("Critical hit!"));
			AttackAnimation = CriticalAttackAnimation;
			ComputedDamage *= CriticalDamageMultiplier;
		}
		else
		{
			AttackAnimation = FMath::RandBool() ? BaseAttackAnimation : AlternativeAttackAnimation;
		}
		CriticalChance -= CriticalSideBonus;
		ComputedDamage /= EnemyDefence;
	}

	PlayAnimationAndDoAction(AttackAnimation, [&]() { OnActionFinished(); });
}


void AGDUnit::OnActionBegin()
{
	AddToActiveEntities();
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
		if (Tile->HasGuardingUnits())
		{
			for (auto& Unit : Tile->GetGuardingUnits())
			{
				Unit->RemoveOutline();
			}
		}
	}
}

void AGDUnit::ResetAllHighlightedTiles()
{
	ResetHighlightedTilesInRange();

	ResetHighlightedActionTiles();
}

void AGDUnit::AddToActiveEntities()
{
	if (AGDPlayerPawn* PlayerPawn = Cast<AGDPlayerPawn>(GetWorld()->GetFirstPlayerController()->GetPawn()))
	{
		PlayerPawn->AddActiveEntity(this);
	}
}

void AGDUnit::RemoveFromActiveEntities()
{
	if (AGDPlayerPawn* PlayerPawn = Cast<AGDPlayerPawn>(GetWorld()->GetFirstPlayerController()->GetPawn()))
	{
		PlayerPawn->RemoveActiveEntity(this);
	}
}

void AGDUnit::SetDirection(const EDirection NewDirection)
{
	FRotator NewRotator{0.f, 0.f, 0.f};

	switch (NewDirection)
	{
	case EDirection::West:
		break;
	case EDirection::North:
		NewRotator.Yaw = 90.0f;
		break;
	case EDirection::East:
		NewRotator.Yaw = 180.0f;
		break;
	case EDirection::South:
		NewRotator.Yaw = -90.0f;
		break;
	}

	LookDirection = NewDirection;
	SetActorRotation(NewRotator);
}

void AGDUnit::HighlightMovementPath(AGDTile* TargetTile)
{
	TArray<AGDTile*> NewMovementPath = CurrentTile->GetGrid()->ComputePathBetweenTiles(
		CurrentTile, TargetTile, this);
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

				if (Tile->HasGuardingUnits())
				{
					bool bInRangeOfAnyGuardingUnit = false;

					for (auto& Unit : Tile->GetGuardingUnits())
					{
						if (Unit->IsTileInAttackRange(Tile))
						{
							Unit->AddOutline(FLinearColor::Red);
							bInRangeOfAnyGuardingUnit = true;
						}
					}

					if (bInRangeOfAnyGuardingUnit)
					{
						Tile->Highlight(EHighlightInfo::Enemy);
					}
				}
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
				const int PathLength = Grid->ComputePathBetweenTiles(CurrentTile, Tile, this).Num() - 1;
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

EDirection GetOppositeDirection(const EDirection Direction)
{
	switch (Direction)
	{
	case EDirection::North:
		return EDirection::South;

	case EDirection::East:
		return EDirection::West;

	case EDirection::South:
		return EDirection::North;

	case EDirection::West:
		return EDirection::East;
	}

	return EDirection::North; // Dummy return to suppress warning
}

void AGDUnit::OnActionFinished()
{
	RemoveFromActiveEntities();
	TargetToAttackAfterMove = nullptr;
}
