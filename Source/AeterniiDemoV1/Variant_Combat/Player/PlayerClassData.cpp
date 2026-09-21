// Copyright Epic Games, Inc. All Rights Reserved.

#include "PlayerClassData.h"
#include "Misc/PackageName.h"

DEFINE_LOG_CATEGORY(LogPlayerClassData);

#define LOCTEXT_NAMESPACE "PlayerClassData"

namespace PlayerClassCanon
{
	struct FAbilityDef
	{
		FName Id;
		FText DisplayName;
	};

	struct FClassDef
	{
		FName ClassId;
		FText OrderName;
		FText DisplayName;
		FText RoleLabel;
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
		Def.OrderName = LOCTEXT("Ordo_Order", "Ordo Primae Flammae");
		Def.DisplayName = LOCTEXT("Ordo_Name", "Sworn of the Black Sigil");
		Def.RoleLabel = LOCTEXT("Ordo_Role", "Bulwark · Melee");
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
		Def.Abilities.Add({ FName(TEXT("vigil_step")), LOCTEXT("Ordo_VigilStep", "Vigil Step") });
		Def.Abilities.Add({ FName(TEXT("ashen_recitation")), LOCTEXT("Ordo_AshenRecitation", "Ashen Recitation") });
		Def.Abilities.Add({ FName(TEXT("effigy_vow")), LOCTEXT("Ordo_EffigyVow", "Effigy of the Vow") });
		return Def;
	}

	static FClassDef MakeIgnivarum()
	{
		FClassDef Def;
		Def.ClassId = PlayerClassIds::Ignivarum();
		Def.OrderName = LOCTEXT("Ignivarum_Order", "Gothorum War-Breed");
		Def.DisplayName = LOCTEXT("Ignivarum_Name", "Ignivarum, Fire-Bearer");
		Def.RoleLabel = LOCTEXT("Ignivarum_Role", "Inquisitor · Aggressor");
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
		Def.Abilities.Add({ FName(TEXT("censer_lunge")), LOCTEXT("Ignivarum_CenserLunge", "Censer Lunge") });
		Def.Abilities.Add({ FName(TEXT("censure_lance")), LOCTEXT("Ignivarum_CensureLance", "Censure Lance") });
		Def.Abilities.Add({ FName(TEXT("second_breath")), LOCTEXT("Ignivarum_SecondBreath", "Second Breath") });
		return Def;
	}

	static FClassDef MakeLuminarch()
	{
		FClassDef Def;
		Def.ClassId = PlayerClassIds::Luminarch();
		Def.OrderName = LOCTEXT("Luminarch_Order", "Choir of the Quiet Star");
		Def.DisplayName = LOCTEXT("Luminarch_Name", "Luminarch Navigator-Priest");
		Def.RoleLabel = LOCTEXT("Luminarch_Role", "Noetic · Ranged");
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
		Def.Abilities.Add({ FName(TEXT("fold")), LOCTEXT("Luminarch_Fold", "Fold") });
		Def.Abilities.Add({ FName(TEXT("unwriting")), LOCTEXT("Luminarch_Unwriting", "Unwriting") });
		Def.Abilities.Add({ FName(TEXT("palindrome")), LOCTEXT("Luminarch_Palindrome", "Palindrome") });
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

	static void Apply(UPlayerClassData& Target, const FClassDef& Def)
	{
		Target.ClassId = Def.ClassId;
		Target.OrderName = Def.OrderName;
		Target.DisplayName = Def.DisplayName;
		Target.RoleLabel = Def.RoleLabel;
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

			if (const FPlayerClassAbilitySlot* Previous = PreviousSlots.Find(Ability.Id))
			{
				Slot.AbilityTag = Previous->AbilityTag;
				Slot.AbilityClass = Previous->AbilityClass;
			}

			Target.AbilitySlots.Add(Slot);
		}

		// Keep a reserved fourth slot so the HUD can bind four cooldown wells.
		while (Target.AbilitySlots.Num() < UPlayerClassData::MaxAbilitySlots)
		{
			Target.AbilitySlots.Add(FPlayerClassAbilitySlot());
		}

		// CharacterClass / PreviewMesh are not touched so in-editor mesh refs survive a re-apply.
	}
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
