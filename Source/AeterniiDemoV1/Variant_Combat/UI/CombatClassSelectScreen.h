// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Engine/DataAsset.h"
#include "UObject/SoftObjectPath.h"
#include "CombatClassSelectScreen.generated.h"

class UButton;
class UTextBlock;

/**
 *  Locked Content paths for Cole's UPlayerClassData assets.
 *  Assets may not exist yet — keep these as soft paths only.
 *
 *  UPlayerClassData is on master. This widget still soft-refs
 *  UPrimaryDataAsset and does not include PlayerClassData.h.
 */
namespace CombatClassSelectPaths
{
	inline const TCHAR* OrdoPackage = TEXT("/Game/Variant_Combat/Player/DA_PlayerClass_Ordo");
	inline const TCHAR* IgnivarumPackage = TEXT("/Game/Variant_Combat/Player/DA_PlayerClass_Ignivarum");
	inline const TCHAR* LuminarchPackage = TEXT("/Game/Variant_Combat/Player/DA_PlayerClass_Luminarch");

	inline const TCHAR* OrdoObject = TEXT("/Game/Variant_Combat/Player/DA_PlayerClass_Ordo.DA_PlayerClass_Ordo");
	inline const TCHAR* IgnivarumObject = TEXT("/Game/Variant_Combat/Player/DA_PlayerClass_Ignivarum.DA_PlayerClass_Ignivarum");
	inline const TCHAR* LuminarchObject = TEXT("/Game/Variant_Combat/Player/DA_PlayerClass_Luminarch.DA_PlayerClass_Luminarch");
}

namespace CombatClassSelectIds
{
	inline FName Ordo() { return FName(TEXT("ordo")); }
	inline FName Ignivarum() { return FName(TEXT("ignivarum")); }
	inline FName Luminarch() { return FName(TEXT("luminarch")); }
}

/** One class-select card. DisplayName is the only player-facing title. */
USTRUCT(BlueprintType)
struct FCombatClassSelectOption
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Class Select")
	FName ClassId;

	/** Igni Orthodoxia / Genitorii Gothica / Luminarch Collegium only. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Class Select")
	FText DisplayName;

	/**
	 *  Soft path to Cole's UPlayerClassData. Typed as UPrimaryDataAsset
	 *  so this widget does not include PlayerClassData.h.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Class Select")
	TSoftObjectPtr<UPrimaryDataAsset> ClassData;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnCombatClassSelected, FName, ClassId, FSoftObjectPath, ClassDataPath);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnCombatClassHighlighted, FName, ClassId, const FText&, DisplayName);

/**
 *  Class-select screen base. Soft-refs Cole's three DAs as
 *  UPrimaryDataAsset (does not include PlayerClassData.h) and exposes
 *  the locked display names only — Igni Orthodoxia, Genitorii Gothica,
 *  Luminarch Collegium. No order / faction subtitle widgets.
 */
UCLASS(abstract)
class UCombatClassSelectScreen : public UUserWidget
{
	GENERATED_BODY()

public:

	UCombatClassSelectScreen(const FObjectInitializer& ObjectInitializer);

	/** /Game/Variant_Combat/Player/DA_PlayerClass_Ordo — Igni Orthodoxia */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Class Select|Canon")
	TSoftObjectPtr<UPrimaryDataAsset> OrdoClassData;

	/** /Game/Variant_Combat/Player/DA_PlayerClass_Ignivarum — Genitorii Gothica */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Class Select|Canon")
	TSoftObjectPtr<UPrimaryDataAsset> IgnivarumClassData;

	/** /Game/Variant_Combat/Player/DA_PlayerClass_Luminarch — Luminarch Collegium */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Class Select|Canon")
	TSoftObjectPtr<UPrimaryDataAsset> LuminarchClassData;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Class Select|Canon")
	FText OrdoDisplayName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Class Select|Canon")
	FText IgnivarumDisplayName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Class Select|Canon")
	FText LuminarchDisplayName;

	/** Name this widget Button_Ordo in UMG. */
	UPROPERTY(BlueprintReadOnly, Category="Class Select", meta=(BindWidgetOptional))
	TObjectPtr<UButton> Button_Ordo;

	/** Name this widget Button_Ignivarum in UMG. */
	UPROPERTY(BlueprintReadOnly, Category="Class Select", meta=(BindWidgetOptional))
	TObjectPtr<UButton> Button_Ignivarum;

	/** Name this widget Button_Luminarch in UMG. */
	UPROPERTY(BlueprintReadOnly, Category="Class Select", meta=(BindWidgetOptional))
	TObjectPtr<UButton> Button_Luminarch;

	/**
	 *  Optional confirm. If unbound, clicking a class button commits
	 *  immediately so a one-click UMG still works.
	 */
	UPROPERTY(BlueprintReadOnly, Category="Class Select", meta=(BindWidgetOptional))
	TObjectPtr<UButton> Button_Confirm;

	/** Optional selected-name label. Name this widget Text_SelectedClassName. */
	UPROPERTY(BlueprintReadOnly, Category="Class Select", meta=(BindWidgetOptional))
	TObjectPtr<UTextBlock> Text_SelectedClassName;

	UPROPERTY(BlueprintAssignable, Category="Class Select")
	FOnCombatClassSelected OnClassSelected;

	UPROPERTY(BlueprintAssignable, Category="Class Select")
	FOnCombatClassHighlighted OnClassHighlighted;

	UFUNCTION(BlueprintPure, Category="Class Select")
	TArray<FCombatClassSelectOption> GetClassOptions() const;

	UFUNCTION(BlueprintPure, Category="Class Select")
	FCombatClassSelectOption GetOptionById(FName ClassId) const;

	UFUNCTION(BlueprintPure, Category="Class Select")
	FName GetHighlightedClassId() const { return HighlightedClassId; }

	UFUNCTION(BlueprintCallable, Category="Class Select")
	void HighlightClass(FName ClassId);

	UFUNCTION(BlueprintCallable, Category="Class Select")
	void ConfirmSelection();

	/** Highlight + commit in one call (touch UI / tests). */
	UFUNCTION(BlueprintCallable, Category="Class Select")
	void SelectClass(FName ClassId);

	UFUNCTION(BlueprintImplementableEvent, Category="Class Select")
	void HandleClassHighlighted(FName ClassId, const FText& DisplayName);

	UFUNCTION(BlueprintImplementableEvent, Category="Class Select")
	void HandleClassConfirmed(FName ClassId, FSoftObjectPath ClassDataPath);

protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Class Select")
	FName HighlightedClassId;

	virtual void NativeConstruct() override;

	UFUNCTION()
	void HandleOrdoClicked();

	UFUNCTION()
	void HandleIgnivarumClicked();

	UFUNCTION()
	void HandleLuminarchClicked();

	UFUNCTION()
	void HandleConfirmClicked();

	void BindClassButtons();
	void HandleClassButtonClicked(FName ClassId);
	void UpdateSelectedClassName(const FText& DisplayName);
};
