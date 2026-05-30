#include "BaseMonster.h"

#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
#include "Combat/HealthComponent.h"
#include "Combat/MonsterMovementZone.h"
#include "Lanes/LaneGrid.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Engine/World.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "NiagaraFunctionLibrary.h"
#include "Sound/SoundBase.h"
#include "Weapons/Bullet.h"

ABaseMonster::ABaseMonster()
{
	PrimaryActorTick.bCanEverTick = true;

	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	if (UCharacterMovementComponent* Move = GetCharacterMovement())
	{
		Move->bOrientRotationToMovement = true;
		Move->RotationRate = FRotator(0.f, 540.f, 0.f);
		Move->MaxWalkSpeed = MovementSpeed;
	}

	if (UCapsuleComponent* Capsule = GetCapsuleComponent())
	{
		Capsule->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Overlap);
		Capsule->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
		Capsule->SetGenerateOverlapEvents(true);
	}

	if (UCharacterMovementComponent* Move = GetCharacterMovement())
	{
		Move->bUseRVOAvoidance = false;
	}

	Health = CreateDefaultSubobject<UHealthComponent>(TEXT("Health"));
	Health->MaxHealth = 50.f;
	Health->Defense = 0.f;
}

void ABaseMonster::BeginPlay()
{
	Super::BeginPlay();

	if (!Controller)
	{
		SpawnDefaultController();
	}
	if (!Controller)
	{
		UE_LOG(LogTemp, Warning, TEXT("[%s] No AI controller — monster won't move. Check AIControllerClass."), *GetName());
	}

	if (UCharacterMovementComponent* Move = GetCharacterMovement())
	{
		Move->MaxWalkSpeed = MovementSpeed;
	}

	InitialSpawnLocation = GetActorLocation();

	if (USkeletalMeshComponent* SkelMesh = GetMesh())
	{
		MeshBaseRelativeLocation = SkelMesh->GetRelativeLocation();

		HitFlashMIDs.Reset();
		const int32 NumMats = SkelMesh->GetNumMaterials();
		for (int32 i = 0; i < NumMats; ++i)
		{
			if (UMaterialInstanceDynamic* MID = SkelMesh->CreateAndSetMaterialInstanceDynamic(i))
			{
				HitFlashMIDs.Add(MID);
			}
		}
		UE_LOG(LogTemp, Log, TEXT("[%s] Hit-flash MIDs created on %d material slot(s)."), *GetName(), HitFlashMIDs.Num());
	}

	if (Health)
	{
		Health->OnDamageTaken.AddDynamic(this, &ABaseMonster::HandleDamageTaken);
		Health->OnDied.AddDynamic(this, &ABaseMonster::HandleDied);
	}
}

void ABaseMonster::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (bDead)
	{
		UpdateHitReact(DeltaSeconds);
		return;
	}

	TimeAlive += DeltaSeconds;
	UpdateMovement(DeltaSeconds);
	UpdateAttack(DeltaSeconds);
	UpdateHitReact(DeltaSeconds);
}

void ABaseMonster::SetTarget(AActor* InTarget)
{
	CurrentTarget = InTarget;
}

void ABaseMonster::SetMovementZone(AMonsterMovementZone* InZone)
{
	MovementZone = InZone;
}

void ABaseMonster::SetLane(ALaneGrid* InLaneGrid, int32 InLaneIndex)
{
	LaneGrid = InLaneGrid;
	if (InLaneGrid)
	{
		LaneIndex = FMath::Clamp(InLaneIndex, 0, InLaneGrid->GetNumLanes() - 1);

		// Snap onto the lane centerline, preserving current along-distance.
		const FVector Forward = InLaneGrid->GetLaneForwardVector();
		const FVector LaneCenter = InLaneGrid->GetLaneCenterWorld(LaneIndex);
		const float Along = FVector::DotProduct(GetActorLocation() - LaneCenter, Forward);
		const FVector Snap = InLaneGrid->GetLanePointWorld(LaneIndex, Along);
		SetActorLocation(FVector(Snap.X, Snap.Y, GetActorLocation().Z));
	}
}

void ABaseMonster::UpdateMovement(float DeltaSeconds)
{
	if (!CurrentTarget.IsValid())
	{
		return;
	}

	// When bound to a lane (MMBN-style), advance along the lane instead of free pathing.
	if (LaneGrid.IsValid())
	{
		UpdateLaneMovement(DeltaSeconds);
		return;
	}

	const FVector SelfLoc = GetActorLocation();
	const FVector TargetLoc = CurrentTarget->GetActorLocation();

	FVector EffectiveTarget = TargetLoc;
	switch (Pathing)
	{
	case EMonsterPathing::DirectToTarget:
		break;
	case EMonsterPathing::LaneAlongX:
		EffectiveTarget.Y = InitialSpawnLocation.Y;
		break;
	case EMonsterPathing::LaneAlongY:
		EffectiveTarget.X = InitialSpawnLocation.X;
		break;
	case EMonsterPathing::LaneFromSpawn:
	{
		const FVector S2T = TargetLoc - InitialSpawnLocation;
		if (FMath::Abs(S2T.X) >= FMath::Abs(S2T.Y))
		{
			EffectiveTarget.Y = InitialSpawnLocation.Y;
		}
		else
		{
			EffectiveTarget.X = InitialSpawnLocation.X;
		}
		break;
	}
	}

	const FVector Flat(EffectiveTarget.X - SelfLoc.X, EffectiveTarget.Y - SelfLoc.Y, 0.f);
	const float DistToEffective = Flat.Size();
	const float DistToActualTarget = FVector::Dist2D(SelfLoc, TargetLoc);

	if (DistToActualTarget <= AttackRange || DistToEffective <= AttackRange)
	{
		return;
	}

	const FVector Direction = Flat.GetSafeNormal();
	AddMovementInput(Direction, 1.f);

	if (MovementPattern == EMonsterMovementPattern::Sinusoidal && SinusoidalLateralStrength > KINDA_SMALL_NUMBER)
	{
		const FVector Perp = FVector::CrossProduct(Direction, FVector::UpVector).GetSafeNormal();
		const float Phase = TimeAlive * SinusoidalFrequency * 2.f * PI;
		AddMovementInput(Perp, SinusoidalLateralStrength * FMath::Sin(Phase));
	}

	if (AMonsterMovementZone* Zone = MovementZone.Get())
	{
		if (!Zone->IsLocationInside(SelfLoc))
		{
			FVector Clamped = Zone->ClampToZone(SelfLoc);
			Clamped.Z = SelfLoc.Z;
			SetActorLocation(Clamped, false);
		}
	}
}

void ABaseMonster::UpdateLaneMovement(float /*DeltaSeconds*/)
{
	ALaneGrid* Grid = LaneGrid.Get();
	if (!Grid || !CurrentTarget.IsValid())
	{
		return;
	}

	const FVector Forward = Grid->GetLaneForwardVector();
	const FVector Right = Grid->GetLaneRightVector();
	const FVector LaneCenter = Grid->GetLaneCenterWorld(LaneIndex);

	const FVector SelfLoc = GetActorLocation();
	const FVector TargetLoc = CurrentTarget->GetActorLocation();

	// Project self and target onto the lane's forward axis.
	const float SelfAlong = FVector::DotProduct(SelfLoc - LaneCenter, Forward);
	const float TargetAlong = FVector::DotProduct(TargetLoc - LaneCenter, Forward);
	const float AlongDelta = TargetAlong - SelfAlong;

	// Stop advancing once within attack range along the lane, but keep recentering laterally.
	if (FMath::Abs(AlongDelta) <= AttackRange)
	{
		const float Lateral0 = FVector::DotProduct(SelfLoc - LaneCenter, Right);
		AddMovementInput(Right, FMath::Clamp(-Lateral0 / 100.f, -1.f, 1.f));
		return;
	}

	// Forward drive along the lane toward the target.
	const float AlongSign = (AlongDelta >= 0.f) ? 1.f : -1.f;
	AddMovementInput(Forward, AlongSign);

	// Desired lateral weave (sinusoidal stays bounded within the lane via SinusoidalLateralStrength).
	float DesiredLateral = 0.f;
	if (MovementPattern == EMonsterMovementPattern::Sinusoidal && SinusoidalLateralStrength > KINDA_SMALL_NUMBER)
	{
		const float Phase = TimeAlive * SinusoidalFrequency * 2.f * PI;
		DesiredLateral = FMath::Sin(Phase) * SinusoidalLateralStrength * (Grid->GetLaneSpacing() * 0.5f);
	}

	const float CurrentLateral = FVector::DotProduct(SelfLoc - LaneCenter, Right);
	const float LateralError = DesiredLateral - CurrentLateral;
	AddMovementInput(Right, FMath::Clamp(LateralError / 100.f, -1.f, 1.f));
}

void ABaseMonster::UpdateAttack(float DeltaSeconds)
{
	TimeSinceLastAttack += DeltaSeconds;

	if (!CurrentTarget.IsValid())
	{
		return;
	}

	const float Dist = FVector::Dist2D(GetActorLocation(), CurrentTarget->GetActorLocation());
	if (Dist > AttackRange)
	{
		return;
	}

	if (TimeSinceLastAttack < AttackCooldown)
	{
		return;
	}

	TimeSinceLastAttack = 0.f;

	if (AttackMontage)
	{
		if (UAnimInstance* AnimInstance = GetMesh() ? GetMesh()->GetAnimInstance() : nullptr)
		{
			bIsAttacking = true;
			const float MontageLen = AnimInstance->Montage_Play(AttackMontage);
			if (MontageLen > 0.f)
			{
				FOnMontageEnded EndedDelegate;
				EndedDelegate.BindLambda([this](UAnimMontage*, bool) { bIsAttacking = false; });
				AnimInstance->Montage_SetEndDelegate(EndedDelegate, AttackMontage);
			}
			else
			{
				bIsAttacking = false;
			}
		}
	}

	if (AttackMode == EMonsterAttackMode::Melee)
	{
		DoMeleeAttack();
	}
	else
	{
		DoRangedAttack();
	}
}

void ABaseMonster::DoMeleeAttack()
{
	if (!CurrentTarget.IsValid())
	{
		return;
	}
	UGameplayStatics::ApplyDamage(CurrentTarget.Get(), Strength, GetController(), this, nullptr);
}

void ABaseMonster::DoRangedAttack()
{
	if (!CurrentTarget.IsValid() || !BulletClass)
	{
		return;
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	const FTransform Xform = GetActorTransform();
	const FVector SpawnLoc = Xform.TransformPosition(BulletSpawnOffset);

	FActorSpawnParameters Params;
	Params.Owner = this;
	Params.Instigator = this;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	ABullet* Bullet = World->SpawnActor<ABullet>(BulletClass, SpawnLoc, GetActorRotation(), Params);
	if (!Bullet)
	{
		return;
	}

	Bullet->bDestroyOnDeactivate = true;
	if (BulletMesh)
	{
		Bullet->SetBulletMesh(BulletMesh);
	}

	const FVector Dir = (CurrentTarget->GetActorLocation() - SpawnLoc).GetSafeNormal();
	Bullet->ActivateBullet(SpawnLoc, Dir, BulletSpeed, Strength, BulletRange, 1, 0.f, this);
}

void ABaseMonster::UpdateHitReact(float DeltaSeconds)
{
	USkeletalMeshComponent* SkelMesh = GetMesh();
	if (!SkelMesh)
	{
		return;
	}

	if (BlinkTimer > 0.f)
	{
		BlinkTimer -= DeltaSeconds;
		const float Alpha = FMath::Clamp(BlinkTimer / FMath::Max(HitBlinkDuration, KINDA_SMALL_NUMBER), 0.f, 1.f);
		const float FinalAlpha = (BlinkTimer <= 0.f) ? 0.f : Alpha;
		for (UMaterialInstanceDynamic* MID : HitFlashMIDs)
		{
			if (MID)
			{
				MID->SetScalarParameterValue(TEXT("HitFlash"), FinalAlpha);
				MID->SetVectorParameterValue(TEXT("HitColor"), HitBlinkColor);
			}
		}

		if (BlinkTimer <= 0.f && HitFlashOverlayMaterial && SkelMesh)
		{
			SkelMesh->SetOverlayMaterial(nullptr);
		}
	}

	if (ShakeTimer > 0.f)
	{
		ShakeTimer -= DeltaSeconds;
		if (ShakeTimer > 0.f)
		{
			const FVector Jitter(
				FMath::FRandRange(-HitShakeIntensity, HitShakeIntensity),
				FMath::FRandRange(-HitShakeIntensity, HitShakeIntensity),
				0.f);
			SkelMesh->SetRelativeLocation(MeshBaseRelativeLocation + Jitter);
		}
		else
		{
			SkelMesh->SetRelativeLocation(MeshBaseRelativeLocation);
		}
	}
}

void ABaseMonster::HandleDamageTaken(float IncomingDamage, float ActualDamage, AActor* DamageInstigator)
{
	if (bDead || ActualDamage <= 0.f)
	{
		return;
	}

	BlinkTimer = HitBlinkDuration;
	ShakeTimer = HitShakeDuration;

	if (HitFlashOverlayMaterial && GetMesh())
	{
		GetMesh()->SetOverlayMaterial(HitFlashOverlayMaterial);
	}

	if (HitEffect)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, HitEffect, GetActorLocation(), GetActorRotation());
	}
	if (HitSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, HitSound, GetActorLocation());
	}

	if (HitReactMontage)
	{
		if (UAnimInstance* AnimInstance = GetMesh() ? GetMesh()->GetAnimInstance() : nullptr)
		{
			AnimInstance->Montage_Play(HitReactMontage);
		}
	}

	OnHitReact(ActualDamage, DamageInstigator);
}

void ABaseMonster::OnHitReact_Implementation(float /*DamageDealt*/, AActor* /*DamageInstigator*/)
{
}

void ABaseMonster::HandleDied(AActor* /*Killer*/)
{
	if (bDead)
	{
		return;
	}
	bDead = true;
	bIsAttacking = false;

	if (DeathEffect)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, DeathEffect, GetActorLocation(), GetActorRotation());
	}
	if (DeathSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, DeathSound, GetActorLocation());
	}

	if (UCharacterMovementComponent* Move = GetCharacterMovement())
	{
		Move->StopMovementImmediately();
		Move->DisableMovement();
	}
	if (UCapsuleComponent* Capsule = GetCapsuleComponent())
	{
		Capsule->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	if (DeathMontage)
	{
		if (UAnimInstance* AnimInstance = GetMesh() ? GetMesh()->GetAnimInstance() : nullptr)
		{
			AnimInstance->Montage_Play(DeathMontage);
		}
	}

	SetActorEnableCollision(false);
	SetLifeSpan(DeathLifeSpan);
}

float ABaseMonster::GetSpeed() const
{
	return GetVelocity().Size2D();
}

bool ABaseMonster::IsMoving() const
{
	return GetSpeed() > IdleSpeedThreshold;
}
