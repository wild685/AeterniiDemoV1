// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "UObject/SoftObjectPath.h"
#include "PlayerClassData.generated.h"

class ACombatCharacter;
class USkeletalMesh;

/** Canon ClassId values from the isometric demo CLASSES table. */
namespace PlayerClassIds
{
	inline FName Ordo() { return FName(TEXT("ordo")); }
	inline FName Ignivarum() { return FName(TEXT("ignivarum")); }
	inline FName Luminarch() { return FName(TEXT("luminarch")); }
}

/** PrimaryAssetType used by UPlayerClassData::GetPrimaryAssetId and Asset Manager. */
namespace PlayerClassAsset
{
	inline FName TypeName() { return FName(TEXT("PlayerClass")); }

	/** Package paths Claude Code should create later. No .uasset ships in this C++ PR. */
	inline const TCHAR* PathOrdo = TEXT("/Game/Variant_Combat/Player/DA_PlayerClass_Ordo");
	inline const TCHAR* PathIgnivarum = TEXT("/Game/Variant_Combat/Player/DA_PlayerClass_Ignivarum");
	inline const TCHAR* PathLuminarch = TEXT("/Game/Variant_Combat/Player/DA_PlayerClass_Luminarch");
}

/**
 *  Vitae / Alacrity / Noesis / Aegis block from the isometric demo.
 *  Integers match the design-reference stat diamonds.
 */
USTRUCT(BlueprintType)
struct FPlayerClassStatBlock
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Stats", meta=(ClampMin=0))
	int32 Vitae = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Stats", meta=(ClampMin=0))
	int32 Alacrity = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Stats", meta=(ClampMin=0))
	int32 Noesis = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Stats", meta=(ClampMin=0))
	int32 Aegis = 0;
};

/**
 *  One of up to four GAS ability slots.
 *
 *  AbilityId is the design-reference placeholder (vigil_step, fold, …).
 *  AbilityTag stays empty until Gary lands real Gameplay Tags; Priya binds
 *  HUD cooldown slots to those tags once GAS is available.
 *  AbilityClass is an optional soft path so this type does not depend on
 *  the GameplayAbilities plugin.
 */
USTRUCT(BlueprintType)
struct FPlayerClassAbilitySlot
{
	GENERATED_BODY()

	/** Design-reference ability id (e.g. vigil_step). Not a Gameplay Tag. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Ability")
	FName AbilityId;

	/** Display name from the isometric demo (e.g. Vigil Step). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Ability")
	FText DisplayName;

	/**
	 *  GAS gameplay tag for this slot. Leave empty — Gary owns real tags.
	 *  Priya binds class-select HUD cooldown slots to these once they exist.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Ability")
	FGameplayTag AbilityTag;

	/**
	 *  Optional soft class path for a future UGameplayAbility (or a BP child).
	 *  Unset in C++; Claude Code / Gary fill this in-editor.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Ability")
	FSoftClassPath AbilityClass;
};

DECLARE_LOG_CATEGORY_EXTERN(LogPlayerClassData, Log, All);

/**
 *  Internal combat archetype. Not a player-facing class title — HUD should
 *  show DisplayName only (Igni Orthodoxia / Genitorii Gothica / Luminarch Collegium).
 */
UENUM(BlueprintType)
enum class EPlayerClassArchetype : uint8
{
	Bulwark UMETA(DisplayName = "Bulwark"),
	Aggressor UMETA(DisplayName = "Aggressor"),
	NoeticRanged UMETA(DisplayName = "Noetic Ranged")
};

/**
 *  Primary data asset for a playable player class (ClassId ordo / ignivarum / luminarch).
 *
 *  Create editor instances at the Content paths in PlayerClassAsset (see
 *  Docs/integration-queue/player-classes.md). This C++ type ships without
 *  .uasset instances — call ApplyCanonDefaults after setting ClassId to
 *  fill isometric-demo numbers without baking them into BP defaults.
 *
 *  PrimaryAssetType: PlayerClass. Priya should soft-ref the three DAs by path
 *  and bind the select screen to DisplayName only.
 */
UCLASS(BlueprintType)
class UPlayerClassData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:

	static constexpr int32 MaxAbilitySlots = 4;

	UPlayerClassData();

	/** Stable id: ordo, ignivarum, or luminarch. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Identity")
	FName ClassId;

	/**
	 *  Sole player-facing class title for the select screen:
	 *  Igni Orthodoxia / Genitorii Gothica / Luminarch Collegium.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Identity")
	FText DisplayName;

	/** Combat kit: bulwark / aggressor / noetic-ranged. For code, not a HUD title. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Combat")
	EPlayerClassArchetype Archetype = EPlayerClassArchetype::Bulwark;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Identity", meta=(MultiLine=true))
	FText FlavorText;

	/** Isometric-demo HP. Combat currently uses a different HP scale — convert when wiring ACombatCharacter. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Combat", meta=(ClampMin=0))
	float BaseHealth = 0.0f;

	/** Fraction in 0–1 from the isometric demo (e.g. 0.32). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Combat", meta=(ClampMin=0, ClampMax=1))
	float BaseArmor = 0.0f;

	/** Isometric-demo move speed (not Unreal cm/s). Convert when applying to CharacterMovement. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Combat", meta=(ClampMin=0))
	float BaseMoveSpeed = 0.0f;

	/** Melee or ranged attack damage, selected by bIsRanged. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Combat", meta=(ClampMin=0))
	float BaseDamage = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Combat", meta=(ClampMin=0, Units="s"))
	float AttackCooldown = 0.0f;

	/** Isometric-demo attack range (not Unreal cm). Convert when wiring traces. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Combat", meta=(ClampMin=0))
	float AttackRange = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Combat")
	bool bIsRanged = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Combat", meta=(ClampMin=0))
	float CorruptionRate = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Stats")
	FPlayerClassStatBlock Stats;

	/**
	 *  Up to four GAS ability slots. Canon fills three; a fourth slot is reserved.
	 *  Tags stay empty until Gary lands GAS tags.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Abilities", meta=(TitleProperty="AbilityId"))
	TArray<FPlayerClassAbilitySlot> AbilitySlots;

	/** Optional character BP. Unset in C++; Claude Code fills this in-editor. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Presentation")
	TSoftClassPtr<ACombatCharacter> CharacterClass;

	/** Optional preview / body mesh. Unset in C++; Claude Code fills this in-editor. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Presentation")
	TSoftObjectPtr<USkeletalMesh> PreviewMesh;

	/** PrimaryAssetType is "PlayerClass"; name is the asset short name (e.g. DA_PlayerClass_Ordo). */
	virtual FPrimaryAssetId GetPrimaryAssetId() const override;

	static FPrimaryAssetType GetPlayerClassAssetType();

	UFUNCTION(BlueprintPure, Category="Player Class")
	static FName GetPlayerClassAssetTypeName();

	/** Canon ids: ordo, ignivarum, luminarch. */
	UFUNCTION(BlueprintPure, Category="Player Class")
	static TArray<FName> GetCanonClassIds();

	UFUNCTION(BlueprintPure, Category="Player Class")
	static bool IsCanonClassId(FName InClassId);

	/**
	 *  Expected Content object path for a canon id, e.g.
	 *  /Game/Variant_Combat/Player/DA_PlayerClass_Ordo.DA_PlayerClass_Ordo
	 */
	UFUNCTION(BlueprintPure, Category="Player Class")
	static FSoftObjectPath GetExpectedAssetPath(FName InClassId);

	/**
	 *  Overwrites identity, combat stats, stat block, and ability slot ids
	 *  from the isometric-demo CLASSES table for ClassId.
	 *  Returns false if ClassId is not ordo, ignivarum, or luminarch.
	 */
	UFUNCTION(BlueprintCallable, Category="Player Class")
	bool ApplyCanonDefaults();

	/**
	 *  Static helper so editor / BP tooling can fill a DA without hardcoding
	 *  magic numbers. Sets Target->ClassId then applies canon defaults.
	 */
	UFUNCTION(BlueprintCallable, Category="Player Class")
	static bool ApplyCanonDefaultsForId(UPlayerClassData* Target, FName InClassId);

	/** Details-panel button. Uses ClassId (or infers it from the asset name). */
	UFUNCTION(CallInEditor, Category="Player Class")
	void ApplyCanonDefaultsFromEditor();

protected:

	FName ResolveClassId() const;
};
