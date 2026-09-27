// Copyright Epic Games, Inc. All Rights Reserved.


#include "Variant_Combat/CombatPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerStart.h"
#include "CombatCharacter.h"
#include "AbilitySystemComponent.h"
#include "AeterniiAttributeSet.h"
#include "PlayerClassData.h"
#include "Engine/LocalPlayer.h"
#include "Engine/World.h"
#include "Blueprint/UserWidget.h"
#include "AeterniiDemoV1.h"
#include "Widgets/Input/SVirtualJoystick.h"

namespace
{
	UPlayerClassData* LoadPlayerClass(const FSoftObjectPath& Path)
	{
		if (Path.IsNull())
		{
			return nullptr;
		}

		return TSoftObjectPtr<UPlayerClassData>(Path).LoadSynchronous();
	}

	UPlayerClassData* ResolvePlayerClass(FName InClassId, const FSoftObjectPath& InClassDataPath)
	{
		if (UPlayerClassData* FromBroadcast = LoadPlayerClass(InClassDataPath))
		{
			return FromBroadcast;
		}

		const FSoftObjectPath ExpectedPath = UPlayerClassData::GetExpectedAssetPath(InClassId);
		if (ExpectedPath.IsNull() || ExpectedPath == InClassDataPath)
		{
			return nullptr;
		}

		return LoadPlayerClass(ExpectedPath);
	}
}

void ACombatPlayerController::BeginPlay()
{
	Super::BeginPlay();
}

void ACombatPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	// only add IMCs for local player controllers
	if (IsLocalPlayerController())
	{
		// add the input mapping context
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
		{
			for (UInputMappingContext* CurrentContext : DefaultMappingContexts)
			{
				Subsystem->AddMappingContext(CurrentContext, 0);
			}

			// only add these IMCs if we're not using mobile touch input
			if (!ShouldUseTouchControls())
			{
				for (UInputMappingContext* CurrentContext : MobileExcludedMappingContexts)
				{
					Subsystem->AddMappingContext(CurrentContext, 0);
				}
			}
		}
	}

	// only spawn touch controls on local player controllers
	if (IsLocalPlayerController() && ShouldUseTouchControls())
	{
		// spawn the mobile controls widget
		MobileControlsWidget = CreateWidget<UUserWidget>(this, MobileControlsWidgetClass);

		if (MobileControlsWidget)
		{
			// add the controls to the player screen
			MobileControlsWidget->AddToPlayerScreen(0);

		} else {

			UE_LOG(LogAeterniiDemoV1, Error, TEXT("Could not spawn mobile controls widget."));

		}

	}
}

void ACombatPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	// subscribe to the pawn's OnDestroyed delegate
	if (IsValid(InPawn))
	{
		InPawn->OnDestroyed.AddDynamic(this, &ACombatPlayerController::OnPawnDestroyed);
	}

	// PossessedBy has already initialized the ability system. Reapply the
	// stored class so respawns keep the pick. No pick leaves defaults alone.
	ApplySelectedPlayerClassToPawn(InPawn);
}

void ACombatPlayerController::SetSelectedPlayerClass(UPlayerClassData* InPlayerClass)
{
	SelectedPlayerClass = IsValid(InPlayerClass) ? InPlayerClass : nullptr;
	ApplySelectedPlayerClassToPawn(GetPawn());
}

void ACombatPlayerController::HandleClassSelected(FName InClassId, FSoftObjectPath InClassDataPath)
{
	UPlayerClassData* ResolvedClass = ResolvePlayerClass(InClassId, InClassDataPath);
	if (!IsValid(ResolvedClass))
	{
		UE_LOG(LogAeterniiDemoV1, Warning,
			TEXT("%s: class select '%s' did not load a UPlayerClassData from '%s'. Selection unchanged."),
			*GetName(), *InClassId.ToString(), *InClassDataPath.ToString());
		return;
	}

	SetSelectedPlayerClass(ResolvedClass);
}

void ACombatPlayerController::ApplySelectedPlayerClassToPawn(APawn* InPawn)
{
	if (!IsValid(SelectedPlayerClass) || !IsValid(InPawn))
	{
		return;
	}

	ACombatCharacter* CombatCharacter = Cast<ACombatCharacter>(InPawn);
	if (!IsValid(CombatCharacter))
	{
		return;
	}

	if (!IsValid(CombatCharacter->GetAbilitySystemComponent()) || !IsValid(CombatCharacter->GetAeterniiAttributeSet()))
	{
		UE_LOG(LogAeterniiDemoV1, Warning,
			TEXT("%s: '%s' has no ability system or attribute set. Player class '%s' was not applied."),
			*GetName(), *CombatCharacter->GetName(), *SelectedPlayerClass->ClassId.ToString());
		return;
	}

	CombatCharacter->ApplyPlayerClassToAttributes(SelectedPlayerClass);

	UE_LOG(LogAeterniiDemoV1, Log,
		TEXT("%s: applied player class '%s' to '%s'."),
		*GetName(), *SelectedPlayerClass->ClassId.ToString(), *CombatCharacter->GetName());
}

void ACombatPlayerController::SetRespawnTransform(const FTransform& NewRespawn)
{
	// save the new respawn transform
	RespawnTransform = NewRespawn;
}

void ACombatPlayerController::OnPawnDestroyed(AActor* DestroyedActor)
{
	// spawn a new character at the respawn transform
	if (ACombatCharacter* RespawnedCharacter = GetWorld()->SpawnActor<ACombatCharacter>(CharacterClass, RespawnTransform))
	{
		// possess the character
		Possess(RespawnedCharacter);
	}
}

bool ACombatPlayerController::ShouldUseTouchControls() const
{
	// are we on a mobile platform? Should we force touch?
	return SVirtualJoystick::ShouldDisplayTouchInterface() || bForceTouchControls;
}
