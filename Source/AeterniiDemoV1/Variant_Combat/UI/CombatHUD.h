// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CombatAbilityCooldownSlot.h"
#include "CombatLifeBar.h"
#include "GameplayTagContainer.h"
#include "CombatHUD.generated.h"

class UProgressBar;

/**
 *  Placeholder cooldown tag names for the four HUD wells.
 *
 *  TODO(Gary): these are not registered Gameplay Tags. Rebind each well to
 *  UPlayerClassData::AbilitySlots[n].AbilityTag (PR #4) once class ability
 *  cooldown tags exist. PR #1 currently registers Ability.Attack,
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
 *  SetLifePercentage / SetBarColor.
 *
 *  Claude Code: WBP parent this class. Bind optional widgets listed in
 *  Docs/integration-queue/hud-extend.md. Do not replace the character
 *  WidgetComponent life bars — those stay UCombatLifeBar.
 *
 *  Corruption and cooldown GAS binds wait on Gary:
 *  - UAeterniiAttributeSet is not in the tree. PR #1 has UCombatAttributeSet
 *    with Health / MaxHealth / Stamina / MaxStamina only.
 *  - Class ability cooldown tags are not in the tree.
 */
UCLASS(abstract)
class UCombatHUD : public UCombatLifeBar
{
	GENERATED_BODY()

public:

	static constexpr int32 AbilitySlotCount = 4;

	UCombatHUD();

	/** Optional screen life fill. Name this widget LifeMeter in UMG. */
	UPROPERTY(BlueprintReadOnly, Category="HUD|Life", meta=(BindWidgetOptional))
	TObjectPtr<UProgressBar> LifeMeter;

	/**
	 *  Optional corruption fill. Name this widget CorruptionMeter in UMG.
	 *
	 *  TODO(Gary): bind to UAeterniiAttributeSet once that type lands.
	 *  Do not assume a final attribute name — PR #1 has no Depth/Corruption
	 *  attribute on UCombatAttributeSet.
	 */
	UPROPERTY(BlueprintReadOnly, Category="HUD|Corruption", meta=(BindWidgetOptional))
	TObjectPtr<UProgressBar> CorruptionMeter;

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

	/** 0-1 corruption. Placeholder until UAeterniiAttributeSet exists. */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="HUD|Corruption")
	void SetCorruptionPercentage(float Percent);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="HUD|Corruption")
	void SetCorruptionBarColor(FLinearColor Color);

	/**
	 *  Convenience for a later current/max pair from UAeterniiAttributeSet.
	 *  Does not name a GAS attribute.
	 */
	UFUNCTION(BlueprintCallable, Category="HUD|Corruption")
	void SetCorruptionFromCurrentMax(float Current, float MaxValue);

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

	void InitializeAbilitySlots();
};
