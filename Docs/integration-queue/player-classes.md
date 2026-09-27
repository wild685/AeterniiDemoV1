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

Suggested future tag names (not registered here): `Ability.Class.VigilStep`, `Ability.Class.AshenRecitation`, `Ability.Class.EffigyVow`, `Ability.Class.CenserLunge`, `Ability.Class.CensureLance`, `Ability.Class.SecondBreath`, `Ability.Class.Fold`, `Ability.Class.Unwriting`, `Ability.Class.Palindrome`. Placeholder **Ability Id** values on the DA already match the demo (`vigil_step`, `fold`, …).

## Units

Isometric-demo combat numbers (`BaseHealth` 150–230, `BaseMoveSpeed` ~3.05–3.55, `AttackRange` ~1.15–6.4) are **not** Unreal centimetres / the current `ACombatCharacter` HP scale (demo MaxHP is 5). Convert when wiring movement, traces, and damage.

## Out of scope (this PR)

- HUD / UMG (Priya)
- GAS ability implementations and native tags (Gary)
- `.uasset` Data Asset instances
- Enemy AI, boss, maps
