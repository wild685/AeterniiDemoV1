// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CombatLifeBar.generated.h"

class UCombatAttributeSet;

/**
 *  A basic life bar user widget used by world-space WidgetComponents on
 *  ACombatCharacter and ACombatEnemy.
 *
 *  Screen HUD extends this type (see UCombatHUD). Keep this class for the
 *  floating bars; do not replace those components with the screen HUD.
 *
 *  SetLifePercentage / SetBarColor stay the contract. They are NativeEvents
 *  so C++ HUD subclasses can drive optional BindWidget progress bars while
 *  existing life-bar Blueprints can still override the same functions.
 *
 *  GAS health bind: UCombatAttributeSet Health / MaxHealth via
 *  SetLifeFromAttributeSet. Stamina is on that set (dodge stub) but this
 *  widget has no stamina meter.
 */
UCLASS(abstract)
class UCombatLifeBar : public UUserWidget
{
	GENERATED_BODY()

public:

	/** Sets the life bar to the provided 0-1 percentage value */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Life Bar")
	void SetLifePercentage(float Percent);

	/** Sets the life bar fill color */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Life Bar")
	void SetBarColor(FLinearColor Color);

	/**
	 *  Drive the bar from UCombatAttributeSet::Health / MaxHealth.
	 *  Does not read Stamina / MaxStamina.
	 */
	UFUNCTION(BlueprintCallable, Category="Life Bar")
	void SetLifeFromAttributeSet(UCombatAttributeSet* AttributeSet);
};
