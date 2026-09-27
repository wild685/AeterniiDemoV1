# Combat HUD + class-select — integration queue

Priya's C++ `UUserWidget` bases for the Combat variant. **This PR does not contain any `.uasset` / `.umap` / `.umeta` / StateTree-graph assets.** Claude Code authors the UMG widgets; Beck assigns icons. Corruption is bound in C++ (see below). Cooldown tags are still placeholders.

World-space life bars stay `UCombatLifeBar` on `ACombatCharacter` / `ACombatEnemy` `WidgetComponent`s. The screen HUD **extends** that type — do not rip those components out or invent a second health API.

## GAS binds

PR #1 landed **`UCombatAttributeSet`** (Health, MaxHealth, Stamina, MaxStamina) for legacy boss life. PR #6 landed **`UAeterniiAttributeSet`**, which also has Health / MaxHealth (isometric hp) plus Corruption / MaxCorruption and Depth. The screen life meter stays on **`UCombatAttributeSet`**.

| Surface | Bind |
| --- | --- |
| Life / health | **`UCombatAttributeSet` Health / MaxHealth** via `SetLifeFromAttributeSet` (or `SetLifePercentage` from `OnHealthChanged`). Do not point LifeMeter at `UAeterniiAttributeSet::Health` |
| Stamina | Present on `UCombatAttributeSet` (dodge stub). **No HUD stamina meter** — do not invent one |
| Corruption | **`UAeterniiAttributeSet` Corruption / MaxCorruption** via `OnCorruptionChanged`. See [Corruption bind](#corruption-bind) |
| Depth | **`UAeterniiAttributeSet` Depth** (0–3) via `OnDepthChanged`. Read-only `Depth` int on the HUD. No depth widget |
| Ability cooldowns | Placeholder tags `Ability.HUD.Cooldown.Slot0`–`Slot3` until class ability tags land. This bind does not change them |

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
| `CorruptionMeter` | `ProgressBar` | `UAeterniiAttributeSet` Corruption / MaxCorruption (`SetCorruptionPercentage`) |
| `AbilitySlot_0` … `AbilitySlot_3` | `CombatAbilityCooldownSlot` (or WBP child) | Four ability wells |

C++ accessors: `SetLifePercentage`, `SetBarColor`, `SetLifeFromAttributeSet`, `SetCorruptionPercentage`, `SetCorruptionBarColor`, `SetCorruptionFromCurrentMax`, `SetCorruptionFromAttributeSet`, `BindToAbilitySystem`, `SetAbilityCooldown(SlotIndex, TimeRemaining, Duration)`, `SetAbilityCooldownTag`. `Depth` is BlueprintReadOnly. `OnDepthChanged(DepthIndex)` is a BlueprintImplementableEvent.

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
3. `WBP_CombatHUD` — parent `CombatHUD`; bind `LifeMeter`, `CorruptionMeter`, four `AbilitySlot_*`. Suggested Content path: `/Game/Variant_Combat/UI/WBP_CombatHUD`. After the widget is constructed, call `BindToAbilitySystem` or `SetCorruptionFromAttributeSet` (see below). Add to player screen from `ACombatPlayerController` (same `CreateWidget` + `AddToPlayerScreen` pattern as mobile controls) — spawn is not wired in C++.
4. `WBP_CombatClassSelect` — parent `CombatClassSelectScreen`; three class buttons + optional confirm; selected name text only. Suggested path: `/Game/Variant_Combat/UI/WBP_CombatClassSelect`.
5. Cole's three `UPlayerClassData` DAs at the paths above (PR #4) — not this HUD PR.

## Corruption bind

`UCombatHUD` subscribes to **`UAeterniiAttributeSet::OnCorruptionChanged`** (`FOnAeterniiCorruptionChanged`: `NewCorruption`, `NewMaxCorruption`) and **`UAeterniiAttributeSet::OnDepthChanged`** (`FOnAeterniiDepthChanged`: `NewDepth`). The bind also reads current values immediately. `NativeDestruct` removes both delegates.

After `WBP_CombatHUD` is constructed (and the pawn exists), the Blueprint calls:

1. `SetCorruptionFromAttributeSet(CombatCharacter->GetAeterniiAttributeSet())` — `GetAeterniiAttributeSet` is `BlueprintPure`. This is the call the HUD widget should make.
2. `BindToAbilitySystem(AbilitySystemComponent)` — same subscription when the ability system component is already in hand. Resolves the set with `GetSet<UAeterniiAttributeSet>()`, then the owner's default subobject named `AttributeSet` (player) or `AeterniiAttributeSet` (enemy).

Either path writes `CorruptionMeter` through `SetCorruptionPercentage` (`Corruption / MaxCorruption`, or 0 when max is <= 0). `SetCorruptionPercentage` and `SetCorruptionFromCurrentMax` still work for a manual push.

**Depth.** `Depth` is a BlueprintReadOnly int, stratum index **0–3** (same clamp as `UAeterniiAttributeSet::GetDepthIndex`). `OnDepthChanged(DepthIndex)` is a BlueprintImplementableEvent fired on bind and whenever `OnDepthChanged` broadcasts. There is no depth progress-bar widget; UMG can implement the event later.

**Life.** `LifeMeter` stays on **`UCombatAttributeSet` Health / MaxHealth** via `SetLifeFromAttributeSet`. `UAeterniiAttributeSet::Health` is the isometric hp field and is not this meter.

**Class select.** `UCombatClassSelectScreen` only broadcasts the pick (`OnClassHighlighted` while browsing, `OnClassSelected` on confirm: `ClassId` and the soft class-data path). It does not call `ApplyPlayerClassToAttributes`. `ACombatPlayerController` binds `OnClassSelected` when it owns the widget, stores the `UPlayerClassData`, and calls `ApplyPlayerClassToAttributes` immediately if a combat pawn is already possessed and again from `OnPossess` (respawn). See `Docs/integration-queue/player-classes.md`.

**Cooldowns.** Placeholder tags `Ability.HUD.Cooldown.Slot0`–`Slot3` are unchanged.

## Gary — remaining binds

**Health (done on the C++ side).** Call `SetLifeFromAttributeSet(AttributeSet)` or `SetLifePercentage(Health / MaxHealth)` from `UCombatAttributeSet::OnHealthChanged`. World-space bars already do this from `ACombatCharacter` / `ACombatBoss`. Screen life stays on that legacy set even though `UAeterniiAttributeSet` also has Health / MaxHealth.

**Corruption (done on the C++ side).** Call `BindToAbilitySystem` or `SetCorruptionFromAttributeSet` from the HUD Blueprint. Attributes are `UAeterniiAttributeSet::Corruption` and `MaxCorruption`. Delegate is `OnCorruptionChanged`.

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
