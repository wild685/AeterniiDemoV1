# Player classes — integration queue

C++ `UPlayerClassData` (`UPrimaryDataAsset`, PrimaryAssetType `PlayerClass`) for the three playable classes. **This PR does not contain any `.uasset` / `.umap` / `.umeta` instances.** Claude Code creates the Data Assets in the editor; Priya soft-refs them from class-select HUD; Gary fills GAS tags.

**Player-facing class title is `DisplayName` only** (Jacob via Nadia): Igni Orthodoxia, Genitorii Gothica, Luminarch Collegium. There is no separate order label or character subtitle. Combat kit is `EPlayerClassArchetype` (`Bulwark` / `Aggressor` / `NoeticRanged`) for code — do not show it as a second title on the select screen.

Numeric defaults and ability ids still come from `Docs/design-reference/Aeternii_Isometric_Demo.html` `CLASSES` (draft PR #3). Constructor / `ApplyCanonDefaults` hold those values in C++ — they are not baked into Content.

## Expected Content paths

Create three `UPlayerClassData` assets (Data Asset factory → Player Class Data):

| ClassId | Display name | Archetype | Asset |
| --- | --- | --- | --- |
| `ordo` | Igni Orthodoxia | Bulwark (melee) | `/Game/Variant_Combat/Player/DA_PlayerClass_Ordo` |
| `ignivarum` | Genitorii Gothica | Aggressor | `/Game/Variant_Combat/Player/DA_PlayerClass_Ignivarum` |
| `luminarch` | Luminarch Collegium | Noetic ranged | `/Game/Variant_Combat/Player/DA_PlayerClass_Luminarch` |

Object paths for `TSoftObjectPtr<UPlayerClassData>`:

- `/Game/Variant_Combat/Player/DA_PlayerClass_Ordo.DA_PlayerClass_Ordo`
- `/Game/Variant_Combat/Player/DA_PlayerClass_Ignivarum.DA_PlayerClass_Ignivarum`
- `/Game/Variant_Combat/Player/DA_PlayerClass_Luminarch.DA_PlayerClass_Luminarch`

Helpers: `UPlayerClassData::GetExpectedAssetPath(ClassId)`, `PlayerClassAsset::PathOrdo` / `PathIgnivarum` / `PathLuminarch`.

## Editor fill

Empty canon fields fill on their own. Which hook runs depends on how Class Id was written:

- **PostEditChangeProperty** — Class Id was changed from the Details panel or the editor property system.
- **PreSave** — the asset is saved. Property writes that never notify the editor (some Unreal MCP setters) still hit this. A blank Class Id is resolved from the asset name through `ResolveClassId()` (`DA_PlayerClass_Ordo`, `DA_PlayerClass_Ignivarum`, `DA_PlayerClass_Luminarch`). The save writes the filled values.
- **PostLoad** — the editor loads an asset that still has empty canon fields, including a canon asset name with a blank Class Id. The package is marked dirty so the fill can be saved.

Auto-fill writes a field only when it is still empty or zero: DisplayName, flavor, combat numbers, each stat, ability slots when every slot is still empty, and Class Id when it is None. A Class Id that is already set stays as it is. Numeric 0 and empty text count as unset, so a saved 0 is filled again on the next editor load or save. Archetype and ranged are written only when the rest of that payload is still blank, because Bulwark / melee are valid values and have no separate empty sentinel.

**Apply Canon Defaults From Editor** (and `ApplyCanonDefaults` / `ApplyCanonDefaultsForId`) is the full reset: it overwrites canon fields for the current ClassId, and it can still infer ClassId from a `DA_PlayerClass_*` asset name. Character Class, Preview Mesh, ability tags, and ability classes are left as they are.

1. Create the DA at the path above.
2. Set **Class Id** to `ordo` / `ignivarum` / `luminarch`, or leave it blank on a `DA_PlayerClass_*` asset and save. Empty fields fill on their own.
3. Use **Apply Canon Defaults From Editor** when you want canon values to replace existing ones.
4. Leave **Character Class** and **Preview Mesh** unset — Claude Code assigns the Combat character BP / mesh.
5. Leave **Ability Tag** / **Ability Class** empty on each slot — Gary owns real tags.

## Priya (class-select HUD)

Soft-ref these three DAs. Bind the class title to `DisplayName` only. Bind cooldown wells to `AbilitySlots[n].AbilityTag` once Gary lands tags (slots 0–2 are canon abilities; slot 3 is reserved empty). Do not hardcode display names or stats in UMG if the DA is available.

`UCombatClassSelectScreen` only broadcasts the pick. Confirm calls `OnClassSelected` with `ClassId` and the soft object path. It does not call `ApplyPlayerClassToAttributes`.

Suggested future tag names (not registered here): `Ability.Class.VigilStep`, `Ability.Class.AshenRecitation`, `Ability.Class.EffigyVow`, `Ability.Class.CenserLunge`, `Ability.Class.CensureLance`, `Ability.Class.SecondBreath`, `Ability.Class.Fold`, `Ability.Class.Unwriting`, `Ability.Class.Palindrome`. Placeholder **Ability Id** values on the DA already match the demo (`vigil_step`, `fold`, …).

## Runtime apply (`ACombatPlayerController`)

The combat player controller owns the selection.

1. **Pick.** When the class-select widget's owning player is an `ACombatPlayerController`, `NativeConstruct` binds `OnClassSelected` to `HandleClassSelected`. `NativeDestruct` removes that bind.
2. **Resolve.** `HandleClassSelected` loads a `UPlayerClassData` from the broadcast path. If that path does not load one, and `UPlayerClassData::GetExpectedAssetPath(ClassId)` is a different valid path, it tries that path. A failed load logs a warning and leaves the stored class unchanged.
3. **Store.** The loaded asset is kept on `SelectedPlayerClass` (`SetSelectedPlayerClass`). Passing null clears the stored class and does not rewrite attributes already on the pawn.
4. **Apply now.** If a pawn is already possessed, `SetSelectedPlayerClass` applies immediately.
5. **Apply on possess.** `OnPossess` calls the same apply after `Super`, so the respawn in `OnPawnDestroyed` (spawn, then `Possess`) gets the class again. `ACombatCharacter::PossessedBy` has already run `InitializeAbilitySystem` by then.
6. **Where the numbers go.** Apply calls the existing `ACombatCharacter::ApplyPlayerClassToAttributes`. That copies `BaseHealth` (onto Health and MaxHealth), `BaseArmor`, `BaseMoveSpeed`, `BaseDamage`, `AttackCooldown`, `AttackRange`, and `CorruptionRate`. This controller does not duplicate that write.

Null cases, all safe:

| Situation | Result |
| --- | --- |
| No class stored yet | `OnPossess` does not write attributes. Constructor / ability-system defaults stay. |
| Possessed pawn is not an `ACombatCharacter` | The pick stays stored. Attributes are not written. |
| Combat pawn has no ability system component or no attribute set | The pick stays stored. Attributes are not written. A warning is logged. |
| Broadcast does not load a `UPlayerClassData` | Stored class and current attributes stay as they were. |

## In-editor wiring still needed

No `.uasset` is created or edited by the controller hook.

1. `WBP_CombatClassSelect` (parent `CombatClassSelectScreen`) still has to be authored in UMG if it is not already. Widget names: `Button_Ordo`, `Button_Ignivarum`, `Button_Luminarch`, optional `Button_Confirm`, optional `Text_SelectedClassName`. Suggested path `/Game/Variant_Combat/UI/WBP_CombatClassSelect`. The C++ base is abstract, so the screen cannot be spawned without this Blueprint child.
2. Create that widget with the combat player controller as its owning player (`Create Widget`, Owning Player = the `ACombatPlayerController`, then add it to the player screen). `NativeConstruct` then binds `OnClassSelected` to `HandleClassSelected`. Do not also bind `OnClassSelected` in the widget Blueprint, or the class is applied twice.
3. If the widget cannot be created with that owner, bind `OnClassSelected` in the widget Blueprint to `ACombatPlayerController::HandleClassSelected` and pass Class Id and Class Data Path through.
4. The three `DA_PlayerClass_*` assets must exist at the object paths above. A missing asset does not change the pawn.
5. Do not call `ApplyPlayerClassToAttributes` from UMG. `BP_CombatPlayerController` does not need an `OnPossess` override; the C++ parent applies the class. If a Blueprint override of `OnPossess` is added later, it has to call the parent.

## Units

Isometric-demo combat numbers (`BaseHealth` 150–230, `BaseMoveSpeed` ~3.05–3.55, `AttackRange` ~1.15–6.4) are **not** Unreal centimetres / the current `ACombatCharacter` HP scale (demo MaxHP is 5). Convert when wiring movement, traces, and damage.

## Out of scope (original data-asset work)

The list below is what the `UPlayerClassData` change did not include. The class-select broadcast and the controller apply are described above.

- HUD / UMG (Priya)
- GAS ability implementations and native tags (Gary)
- `.uasset` Data Asset instances
- Enemy AI, boss, maps
