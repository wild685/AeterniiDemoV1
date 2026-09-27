// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CombatAbilityCooldownSlot.h"
#include "CombatLifeBar.h"
#include "GameplayTagContainer.h"
#include "CombatHUD.generated.h"

class UAbilitySystemComponent;
class UAeterniiAttributeSet;
class UProgressBar;

/**
 *  Placeholder cooldown tag names for the four HUD wells.
 *
 *  TODO(Gary): these are not registered Gameplay Tags. Rebind each well to
 *  UPlayerClassData::AbilitySlots[n].AbilityTag (PR #4) once class ability
 *  cooldown tags exist. Master (PR #1) currently registers Ability.Attack,
 *  Ability.Dodge, Status.Dodge.IFrames, Data.Damage only.
 */
namespace CombatHUDCooldownTags
{
	inline const TCHAR* Slot0 = TEXT("Ability.HUD.Cooldown.Slot0");
	inline const TCHAR* Slot1 = TEXT("Ability.HUD.Cooldown.Slot1");
	inline const TCHAR* Slot2 = TEXT("Ability.HUD.Cooldown.Slot2");
	inline const TCHAR* Slot3 = TEXT("Ability.HUD.Cooldown.Slot3");

	inline FName SlotName(int32 SlotIndex)
	{
		switch (SlotIndex)
		{
		case 0: return FName(Slot0);
		case 1: return FName(Slot1);
		case 2: return FName(Slot2);
		case 3: return FName(Slot3);
		default: return NAME_None;
		}
	}
}

/**
 *  Screen-space combat HUD. Extends UCombatLifeBar so health still uses
 *  SetLifePercentage / SetBarColor and SetLifeFromAttributeSet
 *  (UCombatAttributeSet Health / MaxHealth).
 *
 *  UAeterniiAttributeSet also has Health / MaxHealth (isometric hp). Life
 *  stays on UCombatAttributeSet. Corruption / MaxCorruption and Depth bind
 *  to UAeterniiAttributeSet via SetCorruptionFromAttributeSet or
 *  BindToAbilitySystem.
 *
 *  Claude Code: WBP parent this class. Bind optional widgets listed in
 *  Docs/integration-queue/hud-extend.md. Do not replace the character
 *  WidgetComponent life bars — those stay UCombatLifeBar.
 *
 *  Stamina exists on UCombatAttributeSet (dodge stub) but this HUD has no
 *  stamina meter — do not add one here.
 *
 *  Cooldown tags stay placeholders until class ability tags exist.
 */
UCLASS(abstract)
class UCombatHUD : public UCombatLifeBar
{
	GENERATED_BODY()

public:

	static constexpr int32 AbilitySlotCount = 4;

	UCombatHUD();

	/**
	 *  Optional screen life fill. Name this widget LifeMeter in UMG.
	 *  Life stays on UCombatAttributeSet Health / MaxHealth via
	 *  SetLifeFromAttributeSet. UAeterniiAttributeSet also has Health /
	 *  MaxHealth; do not retarget this meter to that set.
	 */
	UPROPERTY(BlueprintReadOnly, Category="HUD|Life", meta=(BindWidgetOptional))
	TObjectPtr<UProgressBar> LifeMeter;

	/**
	 *  Corruption fill. Name this widget CorruptionMeter in UMG.
	 *  Driven by UAeterniiAttributeSet Corruption / MaxCorruption.
	 */
	UPROPERTY(BlueprintReadOnly, Category="HUD|Corruption", meta=(BindWidgetOptional))
	TObjectPtr<UProgressBar> CorruptionMeter;

	/**
	 *  Stratum index 0–3 from UAeterniiAttributeSet::Depth
	 *  (Lux, Velum, Abyssii, Ruptura). No depth meter widget.
	 */
	UPROPERTY(BlueprintReadOnly, Category="HUD|Depth")
	int32 Depth = 0;

	/** Ability wells 0-3. Name widgets AbilitySlot_0 .. AbilitySlot_3 in UMG. */
	UPROPERTY(BlueprintReadOnly, Category="HUD|Abilities", meta=(BindWidgetOptional))
	TObjectPtr<UCombatAbilityCooldownSlot> AbilitySlot_0;

	UPROPERTY(BlueprintReadOnly, Category="HUD|Abilities", meta=(BindWidgetOptional))
	TObjectPtr<UCombatAbilityCooldownSlot> AbilitySlot_1;

	UPROPERTY(BlueprintReadOnly, Category="HUD|Abilities", meta=(BindWidgetOptional))
	TObjectPtr<UCombatAbilityCooldownSlot> AbilitySlot_2;

	UPROPERTY(BlueprintReadOnly, Category="HUD|Abilities", meta=(BindWidgetOptional))
	TObjectPtr<UCombatAbilityCooldownSlot> AbilitySlot_3;

	/**
	 *  Per-slot GAS cooldown tags. Size 4, empty until Gary / class data
	 *  fills them. Placeholder names live on each slot widget.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="HUD|Abilities")
	TArray<FGameplayTag> AbilityCooldownTags;

	virtual void SetLifePercentage_Implementation(float Percent) override;
	virtual void SetBarColor_Implementation(FLinearColor Color) override;

	/** 0-1 fill for CorruptionMeter. Bind paths call this after reading the attribute. */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="HUD|Corruption")
	void SetCorruptionPercentage(float Percent);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="HUD|Corruption")
	void SetCorruptionBarColor(FLinearColor Color);

	/** Current/max helper. MaxValue <= 0 clears the meter. */
	UFUNCTION(BlueprintCallable, Category="HUD|Corruption")
	void SetCorruptionFromCurrentMax(float Current, float MaxValue);

	/**
	 *  Subscribe to InAttributeSet->OnCorruptionChanged and OnDepthChanged,
	 *  then read Corruption / MaxCorruption and Depth. Replaces any previous
	 *  bind. Null unbinds and clears the meter.
	 */
	UFUNCTION(BlueprintCallable, Category="HUD|Corruption")
	void SetCorruptionFromAttributeSet(UAeterniiAttributeSet* InAttributeSet);

	/**
	 *  Resolve UAeterniiAttributeSet from InAbilitySystem (GetSet, then the
	 *  owner's AttributeSet / AeterniiAttributeSet subobject) and call
	 *  SetCorruptionFromAttributeSet.
	 */
	UFUNCTION(BlueprintCallable, Category="HUD|Corruption")
	void BindToAbilitySystem(UAbilitySystemComponent* InAbilitySystem);

	/** Fired when Depth changes, including the initial read on bind. */
	UFUNCTION(BlueprintImplementableEvent, Category="HUD|Depth")
	void OnDepthChanged(int32 DepthIndex);

	UFUNCTION(BlueprintCallable, Category="HUD|Abilities")
	void SetAbilityCooldown(int32 SlotIndex, float TimeRemaining, float Duration);

	UFUNCTION(BlueprintCallable, Category="HUD|Abilities")
	void SetAbilityCooldownTag(int32 SlotIndex, FGameplayTag CooldownTag);

	UFUNCTION(BlueprintPure, Category="HUD|Abilities")
	UCombatAbilityCooldownSlot* GetAbilitySlot(int32 SlotIndex) const;

	UFUNCTION(BlueprintPure, Category="HUD|Abilities")
	FGameplayTag GetAbilityCooldownTag(int32 SlotIndex) const;

	UFUNCTION(BlueprintPure, Category="HUD|Abilities")
	FName GetAbilityCooldownPlaceholderName(int32 SlotIndex) const;

protected:

	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	UFUNCTION()
	void HandleAeterniiCorruptionChanged(float NewCorruption, float NewMaxCorruption);

	UFUNCTION()
	void HandleAeterniiDepthChanged(float NewDepth);

	void InitializeAbilitySlots();
	void UnbindAeterniiAttributes();
	void ApplyDepthIndex(int32 Index);

	TWeakObjectPtr<UAeterniiAttributeSet> BoundAeterniiAttributes;
};
