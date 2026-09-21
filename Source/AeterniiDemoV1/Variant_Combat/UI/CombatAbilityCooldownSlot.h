// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Engine/Texture2D.h"
#include "GameplayTagContainer.h"
#include "CombatAbilityCooldownSlot.generated.h"

class UImage;
class UProgressBar;
class UTextBlock;

/**
 *  One of four HUD ability wells.
 *
 *  Beck / Claude Code: subclass in UMG, bind AbilityIcon / CooldownFill /
 *  CooldownText, and assign IconTexture on the widget defaults. C++ does not
 *  ship icons.
 *
 *  CooldownTag is empty on purpose. Gary's class ability tags are not in
 *  master yet (PR #4 documents placeholders such as Ability.Class.VigilStep).
 *  Use PlaceholderCooldownTagName until those FGameplayTags exist.
 */
UCLASS(abstract)
class UCombatAbilityCooldownSlot : public UUserWidget
{
	GENERATED_BODY()

public:

	static constexpr int32 InvalidSlotIndex = INDEX_NONE;

	/** 0-3 when hosted by UCombatHUD. INDEX_NONE until assigned. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Cooldown Slot")
	int32 SlotIndex = InvalidSlotIndex;

	/**
	 *  Soft icon so Beck can wire a texture in-editor without a hard ref.
	 *  Applied to AbilityIcon in NativePreConstruct when the asset is loaded.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Cooldown Slot")
	TSoftObjectPtr<UTexture2D> IconTexture;

	/**
	 *  TODO(Gary): replace with the real GAS cooldown Gameplay Tag from
	 *  UPlayerClassData::AbilitySlots[n].AbilityTag once PR #4 + ability
	 *  implementations land. Suggested (not registered) names:
	 *  Ability.Class.VigilStep, Ability.Class.AshenRecitation,
	 *  Ability.Class.EffigyVow, Ability.Class.CenserLunge,
	 *  Ability.Class.CensureLance, Ability.Class.SecondBreath,
	 *  Ability.Class.Fold, Ability.Class.Unwriting, Ability.Class.Palindrome.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Cooldown Slot")
	FGameplayTag CooldownTag;

	/**
	 *  Slot-local placeholder, e.g. Ability.HUD.Cooldown.Slot0.
	 *  Not a registered Gameplay Tag — string identity until Gary's tags exist.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Cooldown Slot")
	FName PlaceholderCooldownTagName;

	/** Optional icon image. Name this widget AbilityIcon in UMG. */
	UPROPERTY(BlueprintReadOnly, Category="Cooldown Slot", meta=(BindWidgetOptional))
	TObjectPtr<UImage> AbilityIcon;

	/** Optional 0-1 cooldown fill. Name this widget CooldownFill in UMG. */
	UPROPERTY(BlueprintReadOnly, Category="Cooldown Slot", meta=(BindWidgetOptional))
	TObjectPtr<UProgressBar> CooldownFill;

	/** Optional remaining-time label. Name this widget CooldownText in UMG. */
	UPROPERTY(BlueprintReadOnly, Category="Cooldown Slot", meta=(BindWidgetOptional))
	TObjectPtr<UTextBlock> CooldownText;

	UFUNCTION(BlueprintCallable, Category="Cooldown Slot")
	void SetSlotIndex(int32 InSlotIndex);

	UFUNCTION(BlueprintCallable, Category="Cooldown Slot")
	void SetCooldownTag(FGameplayTag InCooldownTag);

	UFUNCTION(BlueprintCallable, Category="Cooldown Slot")
	void SetPlaceholderCooldownTagName(FName InTagName);

	UFUNCTION(BlueprintCallable, Category="Cooldown Slot")
	void SetIcon(UTexture2D* Texture);

	/** TimeRemaining / Duration as 0-1 remaining. Duration <= 0 clears the well. */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Cooldown Slot")
	void SetCooldown(float TimeRemaining, float Duration);

	UFUNCTION(BlueprintPure, Category="Cooldown Slot")
	float GetCooldownPercent() const { return CooldownPercent; }

protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Cooldown Slot")
	float CooldownPercent = 0.0f;

	virtual void NativePreConstruct() override;

	void ApplyIconTexture();
};
