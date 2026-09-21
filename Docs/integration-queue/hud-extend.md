# Combat HUD + class-select — integration queue

Priya's C++ `UUserWidget` bases for the Combat variant. **This PR does not contain any `.uasset` / `.umap` / `.umeta` / StateTree-graph assets.** Claude Code authors the UMG widgets; Beck assigns icons; Gary fills GAS binds.

World-space life bars stay `UCombatLifeBar` on `ACombatCharacter` / `ACombatEnemy` `WidgetComponent`s. The screen HUD **extends** that type — do not rip those components out or invent a second health API.

## New C++ classes

| Class | Parent | File |
| --- | --- | --- |
| `UCombatLifeBar` | `UUserWidget` | existing; `SetLifePercentage` / `SetBarColor` are now `BlueprintNativeEvent` so the HUD can fill optional progress bars |
| `UCombatHUD` | `UCombatLifeBar` | `Source/AeterniiDemoV1/Variant_Combat/UI/CombatHUD.h` |
| `UCombatAbilityCooldownSlot` | `UUserWidget` | `Source/AeterniiDemoV1/Variant_Combat/UI/CombatAbilityCooldownSlot.h` |
| `UCombatClassSelectScreen` | `UUserWidget` | `Source/AeterniiDemoV1/Variant_Combat/UI/CombatClassSelectScreen.h` |

`GameplayTags` is added to `AeterniiDemoV1.Build.cs` so cooldown slots can hold `FGameplayTag`. The GameplayAbilities plugin is **not** enabled here (that is PR #1).

## BindWidget names (optional)

All binds are `BindWidgetOptional`. Missing widgets do not fail construct; C++ no-ops.

### `WBP_CombatHUD` (parent `CombatHUD`)

| Widget name | Type | Role |
| --- | --- | --- |
| `LifeMeter` | `ProgressBar` | Screen health fill (`SetLifePercentage` / `SetBarColor`) |
| `CorruptionMeter` | `ProgressBar` | Corruption fill (`SetCorruptionPercentage`) |
| `AbilitySlot_0` … `AbilitySlot_3` | `CombatAbilityCooldownSlot` (or WBP child) | Four ability wells |

C++ accessors: `SetLifePercentage`, `SetBarColor`, `SetCorruptionPercentage`, `SetCorruptionBarColor`, `SetCorruptionFromCurrentMax`, `SetAbilityCooldown(SlotIndex, TimeRemaining, Duration)`, `SetAbilityCooldownTag`.

### `WBP_CombatAbilityCooldownSlot` (parent `CombatAbilityCooldownSlot`)

| Widget name | Type | Role |
| --- | --- | --- |
| `AbilityIcon` | `Image` | Icon Beck assigns via `IconTexture` |
| `CooldownFill` | `ProgressBar` | 0–1 remaining cooldown |
| `CooldownText` | `TextBlock` | Remaining seconds (cleared at 0) |

Assign `IconTexture` (`TSoftObjectPtr<UTexture2D>`) on the slot widget or HUD child slots. Do not hard-ref textures from C++.

### `WBP_CombatClassSelect` (parent `CombatClassSelectScreen`)

| Widget name | Type | Role |
| --- | --- | --- |
| `Button_Ordo` | `Button` | Igni Orthodoxia |
| `Button_Ignivarum` | `Button` | Genitorii Gothica |
| `Button_Luminarch` | `Button` | Luminarch Collegium |
| `Button_Confirm` | `Button` | Optional. If omitted, a class button click commits immediately |
| `Text_SelectedClassName` | `TextBlock` | Selected `DisplayName` only — no subtitle |

Player-facing title is **`DisplayName` only** (Jacob via Nadia). Do not add order / faction subtitle widgets. After PR #4 + DAs exist, prefer `UPlayerClassData::DisplayName` over duplicating strings in UMG.

Delegates: `OnClassHighlighted(ClassId, DisplayName)`, `OnClassSelected(ClassId, ClassDataPath)`.

## Soft-ref paths (class select)

Assets are **not** in this PR. Object paths used by `TSoftObjectPtr<UPrimaryDataAsset>` (expected type **`UPlayerClassData`** on PR #4):

| ClassId | Display name | Package | Object path |
| --- | --- | --- | --- |
| `ordo` | Igni Orthodoxia | `/Game/Variant_Combat/Player/DA_PlayerClass_Ordo` | `/Game/Variant_Combat/Player/DA_PlayerClass_Ordo.DA_PlayerClass_Ordo` |
| `ignivarum` | Genitorii Gothica | `/Game/Variant_Combat/Player/DA_PlayerClass_Ignivarum` | `/Game/Variant_Combat/Player/DA_PlayerClass_Ignivarum.DA_PlayerClass_Ignivarum` |
| `luminarch` | Luminarch Collegium | `/Game/Variant_Combat/Player/DA_PlayerClass_Luminarch` | `/Game/Variant_Combat/Player/DA_PlayerClass_Luminarch.DA_PlayerClass_Luminarch` |

Helpers: `CombatClassSelectPaths::*`, `CombatClassSelectIds::*`. Typed as `UPrimaryDataAsset` so this lane compiles on master before `UPlayerClassData` exists. **Optional merge order:** land PR #4 first so the DAs resolve to the real type; not required to compile this PR.

## What Claude Code must create in UMG

1. `WBP_CombatLifeBar` — keep / continue using the existing world-space life bar (parent `CombatLifeBar`) on character and enemy WidgetComponents. Implement `SetLifePercentage` / `SetBarColor` as today.
2. `WBP_CombatAbilityCooldownSlot` — parent `CombatAbilityCooldownSlot`; bind the three optional names; leave `IconTexture` for Beck.
3. `WBP_CombatHUD` — parent `CombatHUD`; bind `LifeMeter`, `CorruptionMeter`, four `AbilitySlot_*`. Suggested Content path: `/Game/Variant_Combat/UI/WBP_CombatHUD`. Add to player screen from `ACombatPlayerController` (same `CreateWidget` + `AddToPlayerScreen` pattern as mobile controls) — not wired in this C++ PR.
4. `WBP_CombatClassSelect` — parent `CombatClassSelectScreen`; three class buttons + optional confirm; selected name text only. Suggested path: `/Game/Variant_Combat/UI/WBP_CombatClassSelect`.
5. Cole's three `UPlayerClassData` DAs at the paths above (PR #4) — not this HUD PR.

## Gary — corruption + cooldown binds (wait)

**Corruption.** Call `SetCorruptionPercentage` (0–1) or `SetCorruptionFromCurrentMax`. Target type is **`UAeterniiAttributeSet`**. That class is **not in the tree**. PR #1 (`cursor/boss-combat-p1-df1b`) currently has `UCombatAttributeSet` with **Health, MaxHealth, Stamina, MaxStamina** only — no Depth/Corruption attribute. Do not invent a final attribute name in UMG; keep the HUD on the percentage accessors until Gary names it.

**Ability cooldowns.** Four wells, indices 0–3 (slot 3 reserved empty on the class DA). Placeholder names (not registered tags):

- `Ability.HUD.Cooldown.Slot0`
- `Ability.HUD.Cooldown.Slot1`
- `Ability.HUD.Cooldown.Slot2`
- `Ability.HUD.Cooldown.Slot3`

PR #1 tags (`Ability.Attack`, `Ability.Dodge`, `Status.Dodge.IFrames`, `Data.Damage`) are **not** class-ability cooldowns. PR #4 suggested future tags (not registered): `Ability.Class.VigilStep`, `Ability.Class.AshenRecitation`, `Ability.Class.EffigyVow`, `Ability.Class.CenserLunge`, `Ability.Class.CensureLance`, `Ability.Class.SecondBreath`, `Ability.Class.Fold`, `Ability.Class.Unwriting`, `Ability.Class.Palindrome`.

When tags exist, `SetAbilityCooldownTag(SlotIndex, Tag)` then drive `SetAbilityCooldown` from ASC cooldown remaining/duration. Until then, placeholders live on `UCombatAbilityCooldownSlot::PlaceholderCooldownTagName`.

## Out of scope (this PR)

- Any `.uasset` / `.umap` / UMG layout
- Player-controller spawn wiring
- GAS attribute set / ability implementations
- `UPlayerClassData` (Cole, PR #4)
- Enemy AI, boss, maps
