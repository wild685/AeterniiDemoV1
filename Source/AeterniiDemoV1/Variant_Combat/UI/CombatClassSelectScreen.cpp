// Copyright Epic Games, Inc. All Rights Reserved.


#include "CombatClassSelectScreen.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"

#define LOCTEXT_NAMESPACE "CombatClassSelect"

UCombatClassSelectScreen::UCombatClassSelectScreen(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	OrdoClassData = TSoftObjectPtr<UPrimaryDataAsset>(FSoftObjectPath(CombatClassSelectPaths::OrdoObject));
	IgnivarumClassData = TSoftObjectPtr<UPrimaryDataAsset>(FSoftObjectPath(CombatClassSelectPaths::IgnivarumObject));
	LuminarchClassData = TSoftObjectPtr<UPrimaryDataAsset>(FSoftObjectPath(CombatClassSelectPaths::LuminarchObject));

	OrdoDisplayName = LOCTEXT("Ordo_Name", "Igni Orthodoxia");
	IgnivarumDisplayName = LOCTEXT("Ignivarum_Name", "Genitorii Gothica");
	LuminarchDisplayName = LOCTEXT("Luminarch_Name", "Luminarch Collegium");
}

TArray<FCombatClassSelectOption> UCombatClassSelectScreen::GetClassOptions() const
{
	TArray<FCombatClassSelectOption> Options;
	Options.Reserve(3);

	FCombatClassSelectOption Ordo;
	Ordo.ClassId = CombatClassSelectIds::Ordo();
	Ordo.DisplayName = OrdoDisplayName;
	Ordo.ClassData = OrdoClassData;
	Options.Add(Ordo);

	FCombatClassSelectOption Ignivarum;
	Ignivarum.ClassId = CombatClassSelectIds::Ignivarum();
	Ignivarum.DisplayName = IgnivarumDisplayName;
	Ignivarum.ClassData = IgnivarumClassData;
	Options.Add(Ignivarum);

	FCombatClassSelectOption Luminarch;
	Luminarch.ClassId = CombatClassSelectIds::Luminarch();
	Luminarch.DisplayName = LuminarchDisplayName;
	Luminarch.ClassData = LuminarchClassData;
	Options.Add(Luminarch);

	return Options;
}

FCombatClassSelectOption UCombatClassSelectScreen::GetOptionById(FName ClassId) const
{
	for (const FCombatClassSelectOption& Option : GetClassOptions())
	{
		if (Option.ClassId == ClassId)
		{
			return Option;
		}
	}

	return FCombatClassSelectOption();
}

void UCombatClassSelectScreen::HighlightClass(FName ClassId)
{
	const FCombatClassSelectOption Option = GetOptionById(ClassId);
	if (Option.ClassId.IsNone())
	{
		return;
	}

	HighlightedClassId = Option.ClassId;
	UpdateSelectedClassName(Option.DisplayName);

	OnClassHighlighted.Broadcast(Option.ClassId, Option.DisplayName);
	HandleClassHighlighted(Option.ClassId, Option.DisplayName);
}

void UCombatClassSelectScreen::ConfirmSelection()
{
	if (HighlightedClassId.IsNone())
	{
		return;
	}

	const FCombatClassSelectOption Option = GetOptionById(HighlightedClassId);
	const FSoftObjectPath Path = Option.ClassData.ToSoftObjectPath();

	OnClassSelected.Broadcast(Option.ClassId, Path);
	HandleClassConfirmed(Option.ClassId, Path);
}

void UCombatClassSelectScreen::SelectClass(FName ClassId)
{
	HighlightClass(ClassId);
	ConfirmSelection();
}

void UCombatClassSelectScreen::NativeConstruct()
{
	Super::NativeConstruct();
	BindClassButtons();
}

void UCombatClassSelectScreen::HandleOrdoClicked()
{
	HandleClassButtonClicked(CombatClassSelectIds::Ordo());
}

void UCombatClassSelectScreen::HandleIgnivarumClicked()
{
	HandleClassButtonClicked(CombatClassSelectIds::Ignivarum());
}

void UCombatClassSelectScreen::HandleLuminarchClicked()
{
	HandleClassButtonClicked(CombatClassSelectIds::Luminarch());
}

void UCombatClassSelectScreen::HandleConfirmClicked()
{
	ConfirmSelection();
}

void UCombatClassSelectScreen::BindClassButtons()
{
	if (Button_Ordo)
	{
		Button_Ordo->OnClicked.AddDynamic(this, &UCombatClassSelectScreen::HandleOrdoClicked);
	}

	if (Button_Ignivarum)
	{
		Button_Ignivarum->OnClicked.AddDynamic(this, &UCombatClassSelectScreen::HandleIgnivarumClicked);
	}

	if (Button_Luminarch)
	{
		Button_Luminarch->OnClicked.AddDynamic(this, &UCombatClassSelectScreen::HandleLuminarchClicked);
	}

	if (Button_Confirm)
	{
		Button_Confirm->OnClicked.AddDynamic(this, &UCombatClassSelectScreen::HandleConfirmClicked);
	}
}

void UCombatClassSelectScreen::HandleClassButtonClicked(FName ClassId)
{
	HighlightClass(ClassId);

	// No confirm widget → one click commits, so a three-button UMG is enough.
	if (!Button_Confirm)
	{
		ConfirmSelection();
	}
}

void UCombatClassSelectScreen::UpdateSelectedClassName(const FText& DisplayName)
{
	if (Text_SelectedClassName)
	{
		Text_SelectedClassName->SetText(DisplayName);
	}
}

#undef LOCTEXT_NAMESPACE
