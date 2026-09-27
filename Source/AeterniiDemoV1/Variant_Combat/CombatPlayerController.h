// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "UObject/SoftObjectPath.h"
#include "CombatPlayerController.generated.h"

class UInputMappingContext;
class ACombatCharacter;
class UPlayerClassData;

/**
 *  Simple Player Controller for a third person combat game
 *  Manages input mappings
 *  Respawns the player character at the checkpoint when it's destroyed
 *  Stores the class-select pick and reapplies it whenever a pawn is possessed
 */
UCLASS(abstract, Config="Game")
class ACombatPlayerController : public APlayerController
{
	GENERATED_BODY()
	
protected:

	/** Input mapping context for this player */
	UPROPERTY(EditAnywhere, Category="Input|Input Mappings")
	TArray<UInputMappingContext*> DefaultMappingContexts;

	/** Input Mapping Contexts */
	UPROPERTY(EditAnywhere, Category="Input|Input Mappings")
	TArray<UInputMappingContext*> MobileExcludedMappingContexts;

	/** Mobile controls widget to spawn */
	UPROPERTY(EditAnywhere, Category="Input|Touch Controls")
	TSubclassOf<UUserWidget> MobileControlsWidgetClass;

	/** Pointer to the mobile controls widget */
	UPROPERTY()
	TObjectPtr<UUserWidget> MobileControlsWidget;

	/** If true, the player will use UMG touch controls even if not playing on mobile platforms */
	UPROPERTY(EditAnywhere, Config, Category = "Input|Touch Controls")
	bool bForceTouchControls = false;

	/** Character class to respawn when the possessed pawn is destroyed */
	UPROPERTY(EditAnywhere, Category="Respawn")
	TSubclassOf<ACombatCharacter> CharacterClass;

	/**
	 *  Class confirmed on the class-select screen. Null until a pick loads.
	 *  OnPossess reapplies this to the new pawn.
	 */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category="Class Select")
	TObjectPtr<UPlayerClassData> SelectedPlayerClass;

	/** Transform to respawn the character at. Can be set to create checkpoints */
	FTransform RespawnTransform;

protected:

	/** Gameplay initialization */
	virtual void BeginPlay() override;

	/** Initialize input bindings */
	virtual void SetupInputComponent() override;

	/** Pawn initialization */
	virtual void OnPossess(APawn* InPawn) override;

public:

	/** Updates the character respawn transform */
	void SetRespawnTransform(const FTransform& NewRespawn);

	/**
	 *  Stores the picked class. When a combat pawn with an ability system
	 *  is already possessed, applies it immediately. Null clears the pick
	 *  and leaves attributes already on the pawn unchanged.
	 */
	UFUNCTION(BlueprintCallable, Category="Class Select")
	void SetSelectedPlayerClass(UPlayerClassData* InPlayerClass);

	/**
	 *  OnClassSelected from UCombatClassSelectScreen. Loads the
	 *  UPlayerClassData at InClassDataPath (canon path for InClassId if
	 *  that path does not resolve) and stores it.
	 */
	UFUNCTION(BlueprintCallable, Category="Class Select")
	void HandleClassSelected(FName InClassId, FSoftObjectPath InClassDataPath);

protected:

	/** Called if the possessed pawn is destroyed */
	UFUNCTION()
	void OnPawnDestroyed(AActor* DestroyedActor);

	/** Applies SelectedPlayerClass through ACombatCharacter::ApplyPlayerClassToAttributes. */
	void ApplySelectedPlayerClassToPawn(APawn* InPawn);

	/** Returns true if the player should use UMG touch controls */
	bool ShouldUseTouchControls() const;

};
