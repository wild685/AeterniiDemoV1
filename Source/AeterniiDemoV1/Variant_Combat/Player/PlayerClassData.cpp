// Copyright Epic Games, Inc. All Rights Reserved.

#include "PlayerClassData.h"
#include "CombatGameplayTags.h"
#include "Misc/PackageName.h"

#if WITH_EDITOR
#include "Async/Async.h"
#endif

DEFINE_LOG_CATEGORY(LogPlayerClassData);

#define LOCTEXT_NAMESPACE "PlayerClassData"

namespace PlayerClassCanon
{
	struct FAbilityDef
	{
		FName Id;
		FText DisplayName;
		FGameplayTag AbilityTag;
		FSoftClassPath AbilityClass;
	};

	static FSoftClassPath NoeticAbilityClass(const TCHAR* NativeClassName)
	{
		return FSoftClassPath(FString::Printf(TEXT("/Script/AeterniiDemoV1.%s"), NativeClassName));
	}

	struct FClassDef
	{
		FName ClassId;
		FText DisplayName;
		EPlayerClassArchetype Archetype = EPlayerClassArchetype::Bulwark;
		FText FlavorText;
		float Health = 0.0f;
		float Armor = 0.0f;
		float Speed = 0.0f;
		float Damage = 0.0f;
		float AttackCooldown = 0.0f;
		float AttackRange = 0.0f;
		bool bIsRanged = false;
		float CorruptionRate = 0.0f;
		FPlayerClassStatBlock Stats;
		TArray<FAbilityDef, TInlineAllocator<3>> Abilities;
	};

	static FClassDef MakeOrdo()
	{
		FClassDef Def;
		Def.ClassId = PlayerClassIds::Ordo();
		Def.DisplayName = LOCTEXT("Ordo_Name", "Igni Orthodoxia");
		Def.Archetype = EPlayerClassArchetype::Bulwark;
		Def.FlavorText = LOCTEXT("Ordo_Flavor",
			"\"I was told my name once. I gave it to the Flame for safekeeping, and it has not returned it. This is the arrangement. This is the vow.\" — Knight-monk of the First Flame, relic-blade and sigil-plate.");
		Def.Health = 230.0f;
		Def.Armor = 0.32f;
		Def.Speed = 3.05f;
		Def.Damage = 24.0f;
		Def.AttackCooldown = 0.86f;
		Def.AttackRange = 1.25f;
		Def.bIsRanged = false;
		Def.CorruptionRate = 0.72f;
		Def.Stats.Vitae = 5;
		Def.Stats.Alacrity = 2;
		Def.Stats.Noesis = 2;
		Def.Stats.Aegis = 5;
		Def.Abilities.Add({ FName(TEXT("vigil_step")), LOCTEXT("Ordo_VigilStep", "Vigil Step"), TAG_Cooldown_Noetic_VigilStep, NoeticAbilityClass(TEXT("GA_ExistenceShift")) });
		Def.Abilities.Add({ FName(TEXT("ashen_recitation")), LOCTEXT("Ordo_AshenRecitation", "Ashen Recitation"), TAG_Cooldown_Noetic_AshenRecitation, NoeticAbilityClass(TEXT("GA_MemoryBurn")) });
		Def.Abilities.Add({ FName(TEXT("effigy_vow")), LOCTEXT("Ordo_EffigyVow", "Effigy of the Vow"), TAG_Cooldown_Noetic_EffigyVow, NoeticAbilityClass(TEXT("GA_AetherBlight")) });
		return Def;
	}

	static FClassDef MakeIgnivarum()
	{
		FClassDef Def;
		Def.ClassId = PlayerClassIds::Ignivarum();
		Def.DisplayName = LOCTEXT("Ignivarum_Name", "Genitorii Gothica");
		Def.Archetype = EPlayerClassArchetype::Aggressor;
		Def.FlavorText = LOCTEXT("Ignivarum_Flavor",
			"\"Doubt is a kind of rot. I am the answer to rot.\" — War-bred censer-bearer of the namesake order; burns the question and the questioner in one motion.");
		Def.Health = 170.0f;
		Def.Armor = 0.16f;
		Def.Speed = 3.55f;
		Def.Damage = 15.0f;
		Def.AttackCooldown = 0.44f;
		Def.AttackRange = 1.15f;
		Def.bIsRanged = false;
		Def.CorruptionRate = 1.0f;
		Def.Stats.Vitae = 3;
		Def.Stats.Alacrity = 5;
		Def.Stats.Noesis = 3;
		Def.Stats.Aegis = 2;
		Def.Abilities.Add({ FName(TEXT("censer_lunge")), LOCTEXT("Ignivarum_CenserLunge", "Censer Lunge"), TAG_Cooldown_Noetic_CenserLunge, NoeticAbilityClass(TEXT("GA_ExistenceShift")) });
		Def.Abilities.Add({ FName(TEXT("censure_lance")), LOCTEXT("Ignivarum_CensureLance", "Censure Lance"), TAG_Cooldown_Noetic_CensureLance, NoeticAbilityClass(TEXT("GA_MemoryBurn")) });
		Def.Abilities.Add({ FName(TEXT("second_breath")), LOCTEXT("Ignivarum_SecondBreath", "Second Breath"), TAG_Cooldown_Noetic_SecondBreath, NoeticAbilityClass(TEXT("GA_TemporalEcho")) });
		return Def;
	}

	static FClassDef MakeLuminarch()
	{
		FClassDef Def;
		Def.ClassId = PlayerClassIds::Luminarch();
		Def.DisplayName = LOCTEXT("Luminarch_Name", "Luminarch Collegium");
		Def.Archetype = EPlayerClassArchetype::NoeticRanged;
		Def.FlavorText = LOCTEXT("Luminarch_Flavor",
			"\"The pattern is not hidden. It is merely louder than you are.\" — Psyker-navigator, harmonic-sensitive; reads the strata directly, and pays for the reading.");
		Def.Health = 150.0f;
		Def.Armor = 0.10f;
		Def.Speed = 3.45f;
		Def.Damage = 18.0f;
		Def.AttackCooldown = 0.60f;
		Def.AttackRange = 6.4f;
		Def.bIsRanged = true;
		Def.CorruptionRate = 1.28f;
		Def.Stats.Vitae = 2;
		Def.Stats.Alacrity = 4;
		Def.Stats.Noesis = 5;
		Def.Stats.Aegis = 1;
		Def.Abilities.Add({ FName(TEXT("fold")), LOCTEXT("Luminarch_Fold", "Fold"), TAG_Cooldown_Noetic_Fold, NoeticAbilityClass(TEXT("GA_ExistenceShift")) });
		Def.Abilities.Add({ FName(TEXT("unwriting")), LOCTEXT("Luminarch_Unwriting", "Unwriting"), TAG_Cooldown_Noetic_Unwriting, NoeticAbilityClass(TEXT("GA_MemoryBurn")) });
		Def.Abilities.Add({ FName(TEXT("palindrome")), LOCTEXT("Luminarch_Palindrome", "Palindrome"), TAG_Cooldown_Noetic_Palindrome, NoeticAbilityClass(TEXT("GA_TemporalEcho")) });
		return Def;
	}

	static const FClassDef* Find(FName InClassId)
	{
		static const FClassDef Ordo = MakeOrdo();
		static const FClassDef Ignivarum = MakeIgnivarum();
		static const FClassDef Luminarch = MakeLuminarch();

		if (InClassId == PlayerClassIds::Ordo())
		{
			return &Ordo;
		}
		if (InClassId == PlayerClassIds::Ignivarum())
		{
			return &Ignivarum;
		}
		if (InClassId == PlayerClassIds::Luminarch())
		{
			return &Luminarch;
		}
		return nullptr;
	}

	static void RebuildAbilitySlots(UPlayerClassData& Target, const FClassDef& Def)
	{
		TMap<FName, FPlayerClassAbilitySlot> PreviousSlots;
		for (const FPlayerClassAbilitySlot& Slot : Target.AbilitySlots)
		{
			if (!Slot.AbilityId.IsNone())
			{
				PreviousSlots.Add(Slot.AbilityId, Slot);
			}
		}

		Target.AbilitySlots.Reset();
		for (const FAbilityDef& Ability : Def.Abilities)
		{
			FPlayerClassAbilitySlot Slot;
			Slot.AbilityId = Ability.Id;
			Slot.DisplayName = Ability.DisplayName;
			Slot.AbilityTag = Ability.AbilityTag;
			Slot.AbilityClass = Ability.AbilityClass;

			if (const FPlayerClassAbilitySlot* Previous = PreviousSlots.Find(Ability.Id))
			{
				if (Previous->AbilityTag.IsValid())
				{
					Slot.AbilityTag = Previous->AbilityTag;
				}
				if (!Previous->AbilityClass.IsNull())
				{
					Slot.AbilityClass = Previous->AbilityClass;
				}
			}

			Target.AbilitySlots.Add(Slot);
		}

		// Keep a reserved fourth slot so the HUD can bind four cooldown wells.
		while (Target.AbilitySlots.Num() < UPlayerClassData::MaxAbilitySlots)
		{
			Target.AbilitySlots.Add(FPlayerClassAbilitySlot());
		}
	}

	static void Apply(UPlayerClassData& Target, const FClassDef& Def)
	{
		Target.ClassId = Def.ClassId;
		Target.DisplayName = Def.DisplayName;
		Target.Archetype = Def.Archetype;
		Target.FlavorText = Def.FlavorText;
		Target.BaseHealth = Def.Health;
		Target.BaseArmor = Def.Armor;
		Target.BaseMoveSpeed = Def.Speed;
		Target.BaseDamage = Def.Damage;
		Target.AttackCooldown = Def.AttackCooldown;
		Target.AttackRange = Def.AttackRange;
		Target.bIsRanged = Def.bIsRanged;
		Target.CorruptionRate = Def.CorruptionRate;
		Target.Stats = Def.Stats;
		RebuildAbilitySlots(Target, Def);

		// CharacterClass / PreviewMesh are not touched so in-editor mesh refs survive a re-apply.
	}

#if WITH_EDITOR
	static bool IsAbilitySlotEmpty(const FPlayerClassAbilitySlot& Slot)
	{
		return Slot.AbilityId.IsNone()
			&& Slot.DisplayName.IsEmpty()
			&& !Slot.AbilityTag.IsValid()
			&& Slot.AbilityClass.IsNull();
	}

	static bool AreAbilitySlotsEmpty(const UPlayerClassData& Target)
	{
		for (const FPlayerClassAbilitySlot& Slot : Target.AbilitySlots)
		{
			if (!IsAbilitySlotEmpty(Slot))
			{
				return false;
			}
		}
		return true;
	}

	/** True when every canon field that has an empty/zero sentinel is still unset. */
	static bool IsUntouchedPayload(const UPlayerClassData& Target)
	{
		if (!Target.DisplayName.IsEmpty() || !Target.FlavorText.IsEmpty())
		{
			return false;
		}

		if (Target.BaseHealth != 0.0f
			|| Target.BaseArmor != 0.0f
			|| Target.BaseMoveSpeed != 0.0f
			|| Target.BaseDamage != 0.0f
			|| Target.AttackCooldown != 0.0f
			|| Target.AttackRange != 0.0f
			|| Target.CorruptionRate != 0.0f)
		{
			return false;
		}

		if (Target.Stats.Vitae != 0
			|| Target.Stats.Alacrity != 0
			|| Target.Stats.Noesis != 0
			|| Target.Stats.Aegis != 0)
		{
			return false;
		}

		return AreAbilitySlotsEmpty(Target);
	}

	static bool ApplyToEmptyFields(UPlayerClassData& Target, const FClassDef& Def)
	{
		bool bChanged = false;
		// Captured before writes. Archetype / bIsRanged use this so a later non-zero stat does not count as a hand edit.
		const bool bUntouched = IsUntouchedPayload(Target);

		auto FillText = [&bChanged](FText& Field, const FText& Canon)
		{
			if (Field.IsEmpty() && !Canon.IsEmpty())
			{
				Field = Canon;
				bChanged = true;
			}
		};
		auto FillFloat = [&bChanged](float& Field, float Canon)
		{
			if (Field == 0.0f && Canon != 0.0f)
			{
				Field = Canon;
				bChanged = true;
			}
		};
		auto FillInt = [&bChanged](int32& Field, int32 Canon)
		{
			if (Field == 0 && Canon != 0)
			{
				Field = Canon;
				bChanged = true;
			}
		};

		FillText(Target.DisplayName, Def.DisplayName);
		FillText(Target.FlavorText, Def.FlavorText);
		FillFloat(Target.BaseHealth, Def.Health);
		FillFloat(Target.BaseArmor, Def.Armor);
		FillFloat(Target.BaseMoveSpeed, Def.Speed);
		FillFloat(Target.BaseDamage, Def.Damage);
		FillFloat(Target.AttackCooldown, Def.AttackCooldown);
		FillFloat(Target.AttackRange, Def.AttackRange);
		FillFloat(Target.CorruptionRate, Def.CorruptionRate);
		FillInt(Target.Stats.Vitae, Def.Stats.Vitae);
		FillInt(Target.Stats.Alacrity, Def.Stats.Alacrity);
		FillInt(Target.Stats.Noesis, Def.Stats.Noesis);
		FillInt(Target.Stats.Aegis, Def.Stats.Aegis);

		// None is unset. A ClassId that is already set is a hand edit and stays.
		if (Target.ClassId.IsNone())
		{
			Target.ClassId = Def.ClassId;
			bChanged = true;
		}

		// Bulwark and bIsRanged=false are both the C++ default and valid canon. Only write them
		// while the rest of the asset is still blank, so a saved hand edit is left alone.
		if (bUntouched)
		{
			if (Target.Archetype != Def.Archetype)
			{
				Target.Archetype = Def.Archetype;
				bChanged = true;
			}
			if (Target.bIsRanged != Def.bIsRanged)
			{
				Target.bIsRanged = Def.bIsRanged;
				bChanged = true;
			}
		}

		if (AreAbilitySlotsEmpty(Target) && Def.Abilities.Num() > 0)
		{
			RebuildAbilitySlots(Target, Def);
			bChanged = true;
		}
		else
		{
			for (FPlayerClassAbilitySlot& Slot : Target.AbilitySlots)
			{
				if (Slot.AbilityId.IsNone())
				{
					continue;
				}

				for (const FAbilityDef& Ability : Def.Abilities)
				{
					if (Ability.Id != Slot.AbilityId)
					{
						continue;
					}

					if (Slot.DisplayName.IsEmpty())
					{
						Slot.DisplayName = Ability.DisplayName;
						bChanged = true;
					}
					if (!Slot.AbilityTag.IsValid() && Ability.AbilityTag.IsValid())
					{
						Slot.AbilityTag = Ability.AbilityTag;
						bChanged = true;
					}
					if (Slot.AbilityClass.IsNull() && !Ability.AbilityClass.IsNull())
					{
						Slot.AbilityClass = Ability.AbilityClass;
						bChanged = true;
					}
					break;
				}
			}
		}

		return bChanged;
	}
#endif
}

UPlayerClassData::UPlayerClassData()
{
	AbilitySlots.SetNum(MaxAbilitySlots);
}

FPrimaryAssetId UPlayerClassData::GetPrimaryAssetId() const
{
	return FPrimaryAssetId(GetPlayerClassAssetType(), GetFName());
}

FPrimaryAssetType UPlayerClassData::GetPlayerClassAssetType()
{
	return FPrimaryAssetType(PlayerClassAsset::TypeName());
}

FName UPlayerClassData::GetPlayerClassAssetTypeName()
{
	return PlayerClassAsset::TypeName();
}

TArray<FName> UPlayerClassData::GetCanonClassIds()
{
	return { PlayerClassIds::Ordo(), PlayerClassIds::Ignivarum(), PlayerClassIds::Luminarch() };
}

bool UPlayerClassData::IsCanonClassId(FName InClassId)
{
	return PlayerClassCanon::Find(InClassId) != nullptr;
}

FSoftObjectPath UPlayerClassData::GetExpectedAssetPath(FName InClassId)
{
	const TCHAR* PackagePath = nullptr;
	if (InClassId == PlayerClassIds::Ordo())
	{
		PackagePath = PlayerClassAsset::PathOrdo;
	}
	else if (InClassId == PlayerClassIds::Ignivarum())
	{
		PackagePath = PlayerClassAsset::PathIgnivarum;
	}
	else if (InClassId == PlayerClassIds::Luminarch())
	{
		PackagePath = PlayerClassAsset::PathLuminarch;
	}

	if (!PackagePath)
	{
		return FSoftObjectPath();
	}

	const FString AssetName = FPackageName::GetShortName(PackagePath);
	return FSoftObjectPath(FString::Printf(TEXT("%s.%s"), PackagePath, *AssetName));
}

bool UPlayerClassData::ApplyCanonDefaults()
{
	const FName ResolvedId = ResolveClassId();
	const PlayerClassCanon::FClassDef* Def = PlayerClassCanon::Find(ResolvedId);
	if (!Def)
	{
		UE_LOG(LogPlayerClassData, Warning,
			TEXT("%s: ApplyCanonDefaults failed. ClassId '%s' is not ordo, ignivarum, or luminarch."),
			*GetName(), *ClassId.ToString());
		return false;
	}

	PlayerClassCanon::Apply(*this, *Def);
	UE_LOG(LogPlayerClassData, Log, TEXT("%s: applied canon defaults for '%s'."), *GetName(), *Def->ClassId.ToString());
	return true;
}

bool UPlayerClassData::ApplyCanonDefaultsForId(UPlayerClassData* Target, FName InClassId)
{
	if (!Target)
	{
		UE_LOG(LogPlayerClassData, Warning, TEXT("ApplyCanonDefaultsForId: Target is null."));
		return false;
	}

	Target->ClassId = InClassId;
	return Target->ApplyCanonDefaults();
}

void UPlayerClassData::ApplyCanonDefaultsFromEditor()
{
	ApplyCanonDefaults();
}

#if WITH_EDITOR
bool UPlayerClassData::ApplyCanonDefaultsToEmptyFields()
{
	const PlayerClassCanon::FClassDef* Def = PlayerClassCanon::Find(ResolveClassId());
	if (!Def)
	{
		return false;
	}

	const bool bChanged = PlayerClassCanon::ApplyToEmptyFields(*this, *Def);
	if (bChanged)
	{
		UE_LOG(LogPlayerClassData, Log, TEXT("%s: filled empty canon fields for '%s'."), *GetName(), *Def->ClassId.ToString());
	}
	return bChanged;
}

void UPlayerClassData::PostLoad()
{
	Super::PostLoad();

	if (IsTemplate() || HasAnyFlags(RF_Transient))
	{
		return;
	}

	const UPackage* Package = GetOutermost();
	if (!Package
		|| Package == GetTransientPackage()
		|| Package->HasAnyPackageFlags(PKG_CompiledIn | PKG_PlayInEditor | PKG_ForDiffing))
	{
		return;
	}

	TWeakObjectPtr<UPlayerClassData> WeakThis(this);
	auto MarkDirtyOnGameThread = [WeakThis]()
	{
		if (UPlayerClassData* Asset = WeakThis.Get())
		{
			Asset->MarkPackageDirty();
		}
	};

	// Canon names are FText, so a loader thread must not build them. Heal on the game thread instead.
	if (!IsInGameThread())
	{
		if (!IsRunningCommandlet())
		{
			AsyncTask(ENamedThreads::GameThread, [WeakThis]()
			{
				UPlayerClassData* Asset = WeakThis.Get();
				if (!Asset || !Asset->ApplyCanonDefaultsToEmptyFields())
				{
					return;
				}
				if (GIsEditor && !IsRunningCommandlet())
				{
					Asset->MarkPackageDirty();
				}
			});
		}
		return;
	}

	// Fill before PostLoad returns so a cooker serializes the healed values.
	// The loader clears a dirty flag set in PostLoad, so mark it on a later turn.
	if (ApplyCanonDefaultsToEmptyFields() && GIsEditor && !IsRunningCommandlet())
	{
		AsyncTask(ENamedThreads::GameThread, MarkDirtyOnGameThread);
	}
}

void UPlayerClassData::PreSave(FObjectPreSaveContext ObjectSaveContext)
{
	Super::PreSave(ObjectSaveContext);

	// Direct property writes never reach PostEditChangeProperty. Fill before serialization
	// so the saved package contains the canon values. Skip worker-thread saves: canon names are FText.
	if (!IsInGameThread() || IsTemplate() || HasAnyFlags(RF_Transient))
	{
		return;
	}

	const UPackage* Package = GetOutermost();
	if (!Package
		|| Package == GetTransientPackage()
		|| Package->HasAnyPackageFlags(PKG_CompiledIn | PKG_PlayInEditor | PKG_ForDiffing))
	{
		return;
	}

	ApplyCanonDefaultsToEmptyFields();
}

void UPlayerClassData::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	if (GIsTransacting || IsTemplate())
	{
		return;
	}

	if (PropertyChangedEvent.ChangeType == EPropertyChangeType::Interactive)
	{
		return;
	}

	const FName ClassIdName = GET_MEMBER_NAME_CHECKED(UPlayerClassData, ClassId);
	const bool bClassIdEdited = PropertyChangedEvent.GetMemberPropertyName() == ClassIdName
		|| PropertyChangedEvent.GetPropertyName() == ClassIdName;
	if (!bClassIdEdited)
	{
		return;
	}

	if (!IsCanonClassId(ClassId))
	{
		return;
	}

	Modify();
	ApplyCanonDefaultsToEmptyFields();
}
#endif

FName UPlayerClassData::ResolveClassId() const
{
	if (IsCanonClassId(ClassId))
	{
		return ClassId;
	}

	const FString AssetName = GetName().ToLower();
	if (AssetName.Contains(TEXT("ordo")))
	{
		return PlayerClassIds::Ordo();
	}
	if (AssetName.Contains(TEXT("ignivarum")))
	{
		return PlayerClassIds::Ignivarum();
	}
	if (AssetName.Contains(TEXT("luminarch")))
	{
		return PlayerClassIds::Luminarch();
	}

	return ClassId;
}

#undef LOCTEXT_NAMESPACE
