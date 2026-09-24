# Combat HUD + class-select — integration queue

Priya's C++ `UUserWidget` bases for the Combat variant. **This PR does not contain any `.uasset` / `.umap` / `.umeta` / StateTree-graph assets.** Claude Code authors the UMG widgets; Beck assigns icons; Gary fills remaining GAS binds.

World-space life bars stay `UCombatLifeBar` on `ACombatCharacter` / `ACombatEnemy` `WidgetComponent`s. The screen HUD **extends** that type — do not rip those components out or invent a second health API.

## GAS binds (after PR #1)

PR #1 landed **`UCombatAttributeSet`** with **Health, MaxHealth, Stamina, MaxStamina**. There is no `UAeterniiAttributeSet`.

| Surface | Bind |
| --- | --- |
| Life / health | **`UCombatAttributeSet` Health / MaxHealth** via `SetLifeFromAttributeSet` (or `SetLifePercentage` from `OnHealthChanged`) |
| Stamina | Present on the set (dodge stub). **No HUD stamina meter** — do not invent one |
| Corruption | **Unbound stub.** Not on `UCombatAttributeSet`. Wait for Gary to add the attribute; do not invent a name. `SetCorruptionPercentage` / `SetCorruptionFromCurrentMax` stay no-ops until then |
| Ability cooldowns | Placeholder tags until Gary's class ability tags land |

## New C++ classes

| Class | Parent | File |
| --- | --- | --- |
| `UCombatLifeBar` | `UUserWidget` | existing; `SetLifePercentage` / `SetBarColor` are `BlueprintNativeEvent`; `SetLifeFromAttributeSet` reads Health / MaxHealth |
| `UCombatHUD` | `UCombatLifeBar` | `Source/AeterniiDemoV1/Variant_Combat/UI/CombatHUD.h` |
| `UCombatAbilityCooldownSlot` | `UUserWidget` | `Source/AeterniiDemoV1/Variant_Combat/UI/CombatAbilityCooldownSlot.h` |
| `UCombatClassSelectScreen` | `UUserWidget` | `Source/AeterniiDemoV1/Variant_Combat/UI/CombatClassSelectScreen.h` |

`GameplayTags` / `GameplayAbilities` are already on master from PR #1.

## BindWidget names (optional)

All binds are `BindWidgetOptional`. Missing widgets do not fail construct; C++ no-ops.

### `WBP_CombatHUD` (parent `CombatHUD`)

| Widget name | Type | Role |
| --- | --- | --- |
| `LifeMeter` | `ProgressBar` | Screen health fill — `UCombatAttributeSet` Health / MaxHealth |
| `CorruptionMeter` | `ProgressBar` | Unbound stub (`SetCorruptionPercentage`). Not on `UCombatAttributeSet` yet |
| `AbilitySlot_0` … `AbilitySlot_3` | `CombatAbilityCooldownSlot` (or WBP child) | Four ability wells |

C++ accessors: `SetLifePercentage`, `SetBarColor`, `SetLifeFromAttributeSet`, `SetCorruptionPercentage`, `SetCorruptionBarColor`, `SetCorruptionFromCurrentMax`, `SetAbilityCooldown(SlotIndex, TimeRemaining, Duration)`, `SetAbilityCooldownTag`.

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

Assets are **not** in this PR. Object paths stay `TSoftObjectPtr<UPrimaryDataAsset>` soft-refs (expected type **`UPlayerClassData`**, now on master). This widget does not include `PlayerClassData.h`:

| ClassId | Display name | Package | Object path |
| --- | --- | --- | --- |
| `ordo` | Igni Orthodoxia | `/Game/Variant_Combat/Player/DA_PlayerClass_Ordo` | `/Game/Variant_Combat/Player/DA_PlayerClass_Ordo.DA_PlayerClass_Ordo` |
| `ignivarum` | Genitorii Gothica | `/Game/Variant_Combat/Player/DA_PlayerClass_Ignivarum` | `/Game/Variant_Combat/Player/DA_PlayerClass_Ignivarum.DA_PlayerClass_Ignivarum` |
| `luminarch` | Luminarch Collegium | `/Game/Variant_Combat/Player/DA_PlayerClass_Luminarch` | `/Game/Variant_Combat/Player/DA_PlayerClass_Luminarch.DA_PlayerClass_Luminarch` |

Helpers: `CombatClassSelectPaths::*`, `CombatClassSelectIds::*`. Soft-refs stay `UPrimaryDataAsset` on purpose. Locked display names are unchanged: Igni Orthodoxia, Genitorii Gothica, Luminarch Collegium.

## What Claude Code must create in UMG

1. `WBP_CombatLifeBar` — keep / continue using the existing world-space life bar (parent `CombatLifeBar`) on character and enemy WidgetComponents. Drive from `SetLifeFromAttributeSet` / `SetLifePercentage` as today (`ACombatCharacter::HandleGASHealthChanged` already calls `SetLifePercentage`).
2. `WBP_CombatAbilityCooldownSlot` — parent `CombatAbilityCooldownSlot`; bind the three optional names; leave `IconTexture` for Beck.
3. `WBP_CombatHUD` — parent `CombatHUD`; bind `LifeMeter`, `CorruptionMeter` (layout only until Gary adds the attribute), four `AbilitySlot_*`. Suggested Content path: `/Game/Variant_Combat/UI/WBP_CombatHUD`. Add to player screen from `ACombatPlayerController` (same `CreateWidget` + `AddToPlayerScreen` pattern as mobile controls) — not wired in this C++ PR.
4. `WBP_CombatClassSelect` — parent `CombatClassSelectScreen`; three class buttons + optional confirm; selected name text only. Suggested path: `/Game/Variant_Combat/UI/WBP_CombatClassSelect`.
5. Cole's three `UPlayerClassData` DAs at the paths above (PR #4) — not this HUD PR.

## Gary — remaining binds

**Health (done on the C++ side).** Call `SetLifeFromAttributeSet(AttributeSet)` or `SetLifePercentage(Health / MaxHealth)` from `UCombatAttributeSet::OnHealthChanged`. World-space bars already do this from `ACombatCharacter` / `ACombatBoss`.

**Corruption (wait).** `CorruptionMeter` / `SetCorruptionPercentage` are stubs. **Corruption is not in `UCombatAttributeSet`.** Do not invent an attribute name in UMG. When Gary adds it to that set (or a later set), wire current/max through `SetCorruptionFromCurrentMax`.

**Ability cooldowns (wait).** Four wells, indices 0–3 (slot 3 reserved empty on the class DA). Placeholder names (not registered tags):

- `Ability.HUD.Cooldown.Slot0`
- `Ability.HUD.Cooldown.Slot1`
- `Ability.HUD.Cooldown.Slot2`
- `Ability.HUD.Cooldown.Slot3`

PR #1 tags (`Ability.Attack`, `Ability.Dodge`, `Status.Dodge.IFrames`, `Data.Damage`) are **not** class-ability cooldowns. PR #4 suggested future tags (not registered): `Ability.Class.VigilStep`, `Ability.Class.AshenRecitation`, `Ability.Class.EffigyVow`, `Ability.Class.CenserLunge`, `Ability.Class.CensureLance`, `Ability.Class.SecondBreath`, `Ability.Class.Fold`, `Ability.Class.Unwriting`, `Ability.Class.Palindrome`.

When tags exist, `SetAbilityCooldownTag(SlotIndex, Tag)` then drive `SetAbilityCooldown` from ASC cooldown remaining/duration. Until then, placeholders live on `UCombatAbilityCooldownSlot::PlaceholderCooldownTagName`.

## Out of scope (this PR)

- Any `.uasset` / `.umap` / UMG layout
- Player-controller spawn wiring
- Adding a corruption (or stamina) attribute to `UCombatAttributeSet`
- `UPlayerClassData` (Cole, PR #4)
- Enemy AI, boss, maps
