// Copyright Epic Games, Inc. All Rights Reserved.


#include "CombatCharacter.h"

DEFINE_LOG_CATEGORY(LogCombatCharacter);
#include "Components/CapsuleComponent.h"
  #include "Components/WidgetComponent.h"
  #include "GameFramework/CharacterMovementComponent.h"
  #include "GameFramework/ProjectileMovementComponent.h"
  #include "GameFramework/SpringArmComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "CombatLifeBar.h"
#include "CombatEnemy.h"
#include "Engine/DamageEvents.h"
#include "EngineUtils.h"
#include "TimerManager.h"
#include "Engine/LocalPlayer.h"
#include "CombatPlayerController.h"
#include "UObject/ConstructorHelpers.h"

ACombatCharacter::ACombatCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	static ConstructorHelpers::FObjectFinder<UInputAction> LockOnActionFinder(TEXT("/Game/Variant_Combat/Input/Actions/IA_LockOn.IA_LockOn"));
	if (LockOnActionFinder.Succeeded())
	{
		LockOnAction = LockOnActionFinder.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> MagicCastActionFinder(TEXT("/Game/Variant_Combat/Input/Actions/IA_MagicCast.IA_MagicCast"));
	if (MagicCastActionFinder.Succeeded())
	{
		MagicCastAction = MagicCastActionFinder.Object;
	}

	static ConstructorHelpers::FClassFinder<AActor> MagicProjectileClassFinder(TEXT("/Game/Variant_Combat/Magic/BP_MagicProjectile"));
	if (MagicProjectileClassFinder.Succeeded())
	{
		MagicProjectileClass = MagicProjectileClassFinder.Class;
	}

	// bind the attack montage ended delegate
	OnAttackMontageEnded.BindUObject(this, &ACombatCharacter::AttackMontageEnded);

	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(35.0f, 90.0f);

	// Configure character movement
	GetCharacterMovement()->MaxWalkSpeed = 400.0f;

	// create the camera boom
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);

	CameraBoom->TargetArmLength = DefaultCameraDistance;
	CameraBoom->bUsePawnControlRotation = true;
	CameraBoom->bEnableCameraLag = true;
	CameraBoom->bEnableCameraRotationLag = true;

	// create the orbiting camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	// create the life bar widget component
	LifeBar = CreateDefaultSubobject<UWidgetComponent>(TEXT("LifeBar"));
	LifeBar->SetupAttachment(RootComponent);

	// set the player tag
	Tags.Add(FName("Player"));
}

void ACombatCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	// route the input
	DoMove(MovementVector.X, MovementVector.Y);
}

void ACombatCharacter::Look(const FInputActionValue& Value)
{
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	// route the input
	DoLook(LookAxisVector.X, LookAxisVector.Y);
}

void ACombatCharacter::ComboAttackPressed()
{
	// route the input
	DoComboAttackStart();
}

void ACombatCharacter::ChargedAttackPressed()
{
	// route the input
	DoChargedAttackStart();
}

void ACombatCharacter::ChargedAttackReleased()
{
	// route the input
	DoChargedAttackEnd();
}

void ACombatCharacter::ToggleCamera()
{
	// call the BP hook
	BP_ToggleCamera();
}

void ACombatCharacter::ToggleLockOn()
{
	if (IsLockedTargetValid())
	{
		LockedTarget = nullptr;
		UE_LOG(LogCombatCharacter, Log, TEXT("Lock-on cleared."));
		return;
	}

	LockedTarget = FindBestLockOnTarget();
	UE_LOG(LogCombatCharacter, Log, TEXT("Lock-on %s."), LockedTarget ? TEXT("acquired") : TEXT("failed to acquire target"));
  }

	void ACombatCharacter::CastMagic()
	{
		if (!GetWorld() || !MagicProjectileClass)
		{
			UE_LOG(LogCombatCharacter, Warning, TEXT("Magic cast blocked: projectile class is not configured."));
			return;
		}

		const FVector SpawnLocation = GetActorLocation() + GetActorForwardVector() * 100.0f + FVector(0.0f, 0.0f, 50.0f);
		FVector Direction = GetActorForwardVector();

		if (IsLockedTargetValid())
		{
			Direction = (LockedTarget->GetActorLocation() - SpawnLocation).GetSafeNormal();
		}

		if (Direction.IsNearlyZero())
		{
			return;
		}

		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		SpawnParams.Instigator = this;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		const FRotator SpawnRotation = Direction.Rotation();
		AActor* Projectile = GetWorld()->SpawnActor<AActor>(MagicProjectileClass, SpawnLocation, SpawnRotation, SpawnParams);
		if (Projectile)
		{
			if (UProjectileMovementComponent* ProjectileMovement = Projectile->FindComponentByClass<UProjectileMovementComponent>())
			{
				ProjectileMovement->Velocity = Direction * ProjectileMovement->InitialSpeed;
			}

			UE_LOG(LogCombatCharacter, Log, TEXT("Magic cast spawned toward %s."), IsLockedTargetValid() ? *LockedTarget->GetName() : TEXT("forward"));
		}
	}

void ACombatCharacter::DoMove(float Right, float Forward)
{
	if (GetController() != nullptr)
	{
		FVector ForwardDirection;
		FVector RightDirection;

		if (IsLockedTargetValid())
		{
			ForwardDirection = LockedTarget->GetActorLocation() - GetActorLocation();
			ForwardDirection.Z = 0.0f;
			ForwardDirection.Normalize();
			RightDirection = FVector::CrossProduct(FVector::UpVector, ForwardDirection).GetSafeNormal();
		}
		else
		{
			// find out which way is forward
			const FRotator Rotation = GetController()->GetControlRotation();
			const FRotator YawRotation(0, Rotation.Yaw, 0);

			// get forward vector
			ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

			// get right vector
			RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		}

		// add movement 
		AddMovementInput(ForwardDirection, Forward);
		AddMovementInput(RightDirection, Right);
	}
}

void ACombatCharacter::DoLook(float Yaw, float Pitch)
{
	if (GetController() != nullptr)
	{
		// While locked, target-facing rotation owns yaw; retain pitch input for camera framing.
		if (!IsLockedTargetValid())
		{
			AddControllerYawInput(Yaw);
		}

		AddControllerPitchInput(Pitch);
	}
}

ACombatEnemy* ACombatCharacter::FindBestLockOnTarget() const
{
	UWorld* World = GetWorld();
	if (World == nullptr)
	{
		return nullptr;
	}

	FVector ForwardDirection = GetControlRotation().Vector();
	ForwardDirection.Z = 0.0f;
	ForwardDirection.Normalize();

	const FVector Origin = GetActorLocation();
	const float MinimumDot = FMath::Cos(FMath::DegreesToRadians(TargetLockHalfAngle));
	const float MaximumDistanceSquared = FMath::Square(TargetLockRange);
	float BestDistanceSquared = TNumericLimits<float>::Max();
	ACombatEnemy* BestTarget = nullptr;

	for (TActorIterator<ACombatEnemy> It(World); It; ++It)
	{
		ACombatEnemy* Candidate = *It;
		if (!IsValid(Candidate) || Candidate->CurrentHP <= 0.0f)
		{
			continue;
		}

		FVector ToCandidate = Candidate->GetActorLocation() - Origin;
		ToCandidate.Z = 0.0f;
		const float DistanceSquared = ToCandidate.SizeSquared();
		if (DistanceSquared <= KINDA_SMALL_NUMBER || DistanceSquared > MaximumDistanceSquared)
		{
			continue;
		}

		ToCandidate.Normalize();
		if (FVector::DotProduct(ForwardDirection, ToCandidate) < MinimumDot)
		{
			continue;
		}

		if (DistanceSquared < BestDistanceSquared)
		{
			BestDistanceSquared = DistanceSquared;
			BestTarget = Candidate;
		}
	}

	return BestTarget;
}

bool ACombatCharacter::IsLockedTargetValid() const
{
	if (!IsValid(LockedTarget) || LockedTarget->CurrentHP <= 0.0f)
	{
		return false;
	}

	return FVector::DistSquared(GetActorLocation(), LockedTarget->GetActorLocation()) <= FMath::Square(TargetLockRange);
}

void ACombatCharacter::UpdateLockOn(float DeltaSeconds)
{
	if (!IsLockedTargetValid() || GetController() == nullptr)
	{
		LockedTarget = nullptr;
		return;
	}

	FVector ToTarget = LockedTarget->GetActorLocation() - GetActorLocation();
	ToTarget.Z = 0.0f;
	if (!ToTarget.Normalize())
	{
		return;
	}

	const FRotator CurrentRotation = GetController()->GetControlRotation();
	const FRotator DesiredRotation = ToTarget.Rotation();
	FRotator NewRotation = FMath::RInterpTo(CurrentRotation, DesiredRotation, DeltaSeconds, TargetLockRotationSpeed);
	NewRotation.Pitch = CurrentRotation.Pitch;
	NewRotation.Roll = 0.0f;
	GetController()->SetControlRotation(NewRotation);
}

void ACombatCharacter::DoComboAttackStart()
{
	// are we already playing an attack animation?
	if (bIsAttacking)
	{
		// cache the input time so we can check it later
		CachedAttackInputTime = GetWorld()->GetTimeSeconds();

		return;
	}

	// perform a combo attack
	ComboAttack();
}

void ACombatCharacter::DoComboAttackEnd()
{
	// stub
}

void ACombatCharacter::DoChargedAttackStart()
{
	// raise the charging attack flag
	bIsChargingAttack = true;

	if (bIsAttacking)
	{
		// do not attack if the charge animation hasn't looped at least once
		if (!bHasLoopedChargedAttack)
		{
			bHasReleasedChargedAttack = false;
		}

		// cache the input time so we can check it later
		CachedAttackInputTime = GetWorld()->GetTimeSeconds();

		return;
	}

	ChargedAttack();
}

void ACombatCharacter::DoChargedAttackEnd()
{
	// lower the charging attack flag
	bIsChargingAttack = false;

	// have we done the charge loop at least once and haven't released the button yet?
	if (bHasLoopedChargedAttack && !bHasReleasedChargedAttack)
	{
		// release the charge and resolve the attack
		bHasReleasedChargedAttack = true;

		LoopOrResolveChargedAttack();
	}
}

void ACombatCharacter::ResetHP()
{
	// reset the current HP total
	CurrentHP = MaxHP;

	// update the life bar
	LifeBarWidget->SetLifePercentage(1.0f);
}

void ACombatCharacter::ComboAttack()
{
	// raise the attacking flag
	bIsAttacking = true;

	// reset the combo count
	ComboCount = 0;

	// notify enemies they are about to be attacked
	NotifyEnemiesOfIncomingAttack();

	// play the attack montage
	if (UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance())
	{
		const float MontageLength = AnimInstance->Montage_Play(ComboAttackMontage, 1.0f, EMontagePlayReturnType::MontageLength, 0.0f, true);

		// subscribe to montage completed and interrupted events
		if (MontageLength > 0.0f)
		{
			// set the end delegate for the montage
			AnimInstance->Montage_SetEndDelegate(OnAttackMontageEnded, ComboAttackMontage);
		}
	}

}

void ACombatCharacter::ChargedAttack()
{
	// raise the attacking flag
	bIsAttacking = true;

	// reset the charge loop flag
	bHasLoopedChargedAttack = false;

	// reset the charge release flag
	bHasReleasedChargedAttack = false;

	// notify enemies they are about to be attacked
	NotifyEnemiesOfIncomingAttack();

	// play the charged attack montage
	if (UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance())
	{
		const float MontageLength = AnimInstance->Montage_Play(ChargedAttackMontage, 1.0f, EMontagePlayReturnType::MontageLength, 0.0f, true);

		// subscribe to montage completed and interrupted events
		if (MontageLength > 0.0f)
		{
			// set the end delegate for the montage
			AnimInstance->Montage_SetEndDelegate(OnAttackMontageEnded, ChargedAttackMontage);
		}
	}
}

void ACombatCharacter::AttackMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	// reset the attacking flag
	bIsAttacking = false;

	// check if we have a non-stale cached input
	if (GetWorld()->GetTimeSeconds() - CachedAttackInputTime <= AttackInputCacheTimeTolerance)
	{
		// are we holding the charged attack button?
		if (bIsChargingAttack)
		{
			// do a charged attack
			ChargedAttack();
		}
		else
		{
			// do a regular attack
			ComboAttack();
		}
	}
}

void ACombatCharacter::DoAttackTrace(FName DamageSourceBone)
{
	// sweep for objects in front of the character to be hit by the attack
	TArray<FHitResult> OutHits;

	// start at the provided socket location, sweep forward
	const FVector TraceStart = GetMesh()->GetSocketLocation(DamageSourceBone);
	const FVector TraceEnd = TraceStart + (GetActorForwardVector() * MeleeTraceDistance);

	// check for pawn and world dynamic collision object types
	FCollisionObjectQueryParams ObjectParams;
	ObjectParams.AddObjectTypesToQuery(ECC_Pawn);
	ObjectParams.AddObjectTypesToQuery(ECC_WorldDynamic);

	// use a sphere shape for the sweep
	FCollisionShape CollisionShape;
	CollisionShape.SetSphere(MeleeTraceRadius);

	// ignore self
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);

	if (GetWorld()->SweepMultiByObjectType(OutHits, TraceStart, TraceEnd, FQuat::Identity, ObjectParams, CollisionShape, QueryParams))
	{
		// iterate over each object hit
		for (const FHitResult& CurrentHit : OutHits)
		{
			// check if we've hit a damageable actor
			ICombatDamageable* Damageable = Cast<ICombatDamageable>(CurrentHit.GetActor());

			if (Damageable)
			{
				// knock upwards and away from the impact normal
				const FVector Impulse = (CurrentHit.ImpactNormal * -MeleeKnockbackImpulse) + (FVector::UpVector * MeleeLaunchImpulse);

				// pass the damage event to the actor
				Damageable->ApplyDamage(MeleeDamage, this, CurrentHit.ImpactPoint, Impulse);

				// call the BP handler to play effects, etc.
				DealtDamage(MeleeDamage, CurrentHit.ImpactPoint);
			}
		}
	}
}

void ACombatCharacter::CheckCombo()
{
	// are we playing a non-charge attack animation?
	if (bIsAttacking && !bIsChargingAttack)
	{
		// is the last attack input not stale?
		if (GetWorld()->GetTimeSeconds() - CachedAttackInputTime <= ComboInputCacheTimeTolerance)
		{
			// consume the attack input so we don't accidentally trigger it twice
			CachedAttackInputTime = 0.0f;

			// increase the combo counter
			++ComboCount;

			// do we still have a combo section to play?
			if (ComboCount < ComboSectionNames.Num())
			{
				// notify enemies they are about to be attacked
				NotifyEnemiesOfIncomingAttack();

				// jump to the next combo section
				if (UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance())
				{
					AnimInstance->Montage_JumpToSection(ComboSectionNames[ComboCount], ComboAttackMontage);
				}
			}
		}
	}
}

void ACombatCharacter::CheckChargedAttack()
{
	// raise the looped charged attack flag
	bHasLoopedChargedAttack = true;

	// set the input release flag from the input. This will determine if we loop or resolve
	bHasReleasedChargedAttack = !bIsChargingAttack;

	// resolve the charge loop
	LoopOrResolveChargedAttack();
}

void ACombatCharacter::LoopOrResolveChargedAttack()
{
	// jump to either the loop or the attack section depending on whether we've released the charge
	if (UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance())
	{
		AnimInstance->Montage_JumpToSection(bHasReleasedChargedAttack ? ChargeAttackSection : ChargeLoopSection , ChargedAttackMontage);
	}
}

void ACombatCharacter::NotifyEnemiesOfIncomingAttack()
{
	// sweep for objects in front of the character to be hit by the attack
	TArray<FHitResult> OutHits;

	// start at the actor location, sweep forward
	const FVector TraceStart = GetActorLocation();
	const FVector TraceEnd = TraceStart + (GetActorForwardVector() * DangerTraceDistance);

	// check for pawn object types only
	FCollisionObjectQueryParams ObjectParams;
	ObjectParams.AddObjectTypesToQuery(ECC_Pawn);

	// use a sphere shape for the sweep
	FCollisionShape CollisionShape;
	CollisionShape.SetSphere(DangerTraceRadius);

	// ignore self
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);

	if (GetWorld()->SweepMultiByObjectType(OutHits, TraceStart, TraceEnd, FQuat::Identity, ObjectParams, CollisionShape, QueryParams))
	{
		// iterate over each object hit
		for (const FHitResult& CurrentHit : OutHits)
		{
			// check if we've hit a damageable actor
			ICombatDamageable* Damageable = Cast<ICombatDamageable>(CurrentHit.GetActor());

			if (Damageable)
			{
				// notify the enemy
				Damageable->NotifyDanger(GetActorLocation(), this);
			}
		}
	}
}

void ACombatCharacter::ApplyDamage(float Damage, AActor* DamageCauser, const FVector& DamageLocation, const FVector& DamageImpulse)
{
	// pass the damage event to the actor
	FDamageEvent DamageEvent;
	const float ActualDamage = TakeDamage(Damage, DamageEvent, nullptr, DamageCauser);

	// only process knockback and effects if we received nonzero damage
	if (ActualDamage > 0.0f)
	{
		// apply the knockback impulse
		GetCharacterMovement()->AddImpulse(DamageImpulse, true);

		// is the character ragdolling?
		if (GetMesh()->IsSimulatingPhysics())
		{
			// apply an impulse to the ragdoll
			GetMesh()->AddImpulseAtLocation(DamageImpulse * GetMesh()->GetMass(), DamageLocation);
		}

		// pass control to BP to play effects, etc.
		ReceivedDamage(ActualDamage, DamageLocation, DamageImpulse.GetSafeNormal());
	}

}

void ACombatCharacter::HandleDeath()
{
	LockedTarget = nullptr;

	// disable movement while we're dead
	GetCharacterMovement()->DisableMovement();

	// enable full ragdoll physics
	GetMesh()->SetSimulatePhysics(true);

	// hide the life bar
	LifeBar->SetHiddenInGame(true);

	// pull back the camera
	GetCameraBoom()->TargetArmLength = DeathCameraDistance;

	// schedule respawning
	GetWorld()->GetTimerManager().SetTimer(RespawnTimer, this, &ACombatCharacter::RespawnCharacter, RespawnTime, false);
}

void ACombatCharacter::ApplyHealing(float Healing, AActor* Healer)
{
	// stub
}

void ACombatCharacter::NotifyDanger(const FVector& DangerLocation, AActor* DangerSource)
{
	// stub
}

void ACombatCharacter::RespawnCharacter()
{
	// destroy the character and let it be respawned by the Player Controller
	Destroy();
}

float ACombatCharacter::TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	// only process damage if the character is still alive
	if (CurrentHP <= 0.0f)
	{
		return 0.0f;
	}

	// reduce the current HP
	CurrentHP -= Damage;

	// have we run out of HP?
	if (CurrentHP <= 0.0f)
	{
		// die
		HandleDeath();
	}
	else
	{
		// update the life bar
		LifeBarWidget->SetLifePercentage(CurrentHP / MaxHP);

		// enable partial ragdoll physics, but keep the pelvis vertical
		GetMesh()->SetPhysicsBlendWeight(0.5f);
		GetMesh()->SetBodySimulatePhysics(PelvisBoneName, false);
	}

	// return the received damage amount
	return Damage;
}

void ACombatCharacter::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);

	// is the character still alive?
	if (CurrentHP >= 0.0f)
	{
		// disable ragdoll physics
		GetMesh()->SetPhysicsBlendWeight(0.0f);
	}
}

void ACombatCharacter::BeginPlay()
{
	Super::BeginPlay();

	// get the life bar from the widget component
	LifeBarWidget = Cast<UCombatLifeBar>(LifeBar->GetUserWidgetObject());
	check(LifeBarWidget);

	// initialize the camera
	GetCameraBoom()->TargetArmLength = DefaultCameraDistance;

	// save the relative transform for the mesh so we can reset the ragdoll later
	MeshStartingTransform = GetMesh()->GetRelativeTransform();

	// set the life bar color
	LifeBarWidget->SetBarColor(LifeBarColor);

	// reset HP to maximum
	ResetHP();
}

void ACombatCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (IsLockedTargetValid())
	{
		UpdateLockOn(DeltaSeconds);
	}
	else if (LockedTarget != nullptr)
	{
		LockedTarget = nullptr;
	}
}

void ACombatCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	// clear the respawn timer
	GetWorld()->GetTimerManager().ClearTimer(RespawnTimer);
}

void ACombatCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ACombatCharacter::Move);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ACombatCharacter::Look);
		EnhancedInputComponent->BindAction(MouseLookAction, ETriggerEvent::Triggered, this, &ACombatCharacter::Look);

		// Combo Attack
		EnhancedInputComponent->BindAction(ComboAttackAction, ETriggerEvent::Started, this, &ACombatCharacter::ComboAttackPressed);

		// Charged Attack
		EnhancedInputComponent->BindAction(ChargedAttackAction, ETriggerEvent::Started, this, &ACombatCharacter::ChargedAttackPressed);
		EnhancedInputComponent->BindAction(ChargedAttackAction, ETriggerEvent::Completed, this, &ACombatCharacter::ChargedAttackReleased);

		// Camera Side Toggle
		EnhancedInputComponent->BindAction(ToggleCameraAction, ETriggerEvent::Triggered, this, &ACombatCharacter::ToggleCamera);

  		// Target Lock-On
  		EnhancedInputComponent->BindAction(LockOnAction, ETriggerEvent::Started, this, &ACombatCharacter::ToggleLockOn);

		// Magic Cast
		EnhancedInputComponent->BindAction(MagicCastAction, ETriggerEvent::Started, this, &ACombatCharacter::CastMagic);
  	}
}

void ACombatCharacter::NotifyControllerChanged()
{
	Super::NotifyControllerChanged();

	// update the respawn transform on the Player Controller
	if (ACombatPlayerController* PC = Cast<ACombatPlayerController>(GetController()))
	{
		PC->SetRespawnTransform(GetActorTransform());
	}
}

