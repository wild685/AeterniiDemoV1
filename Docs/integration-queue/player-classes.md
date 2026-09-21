# Player classes — integration queue

C++ `UPlayerClassData` (`UPrimaryDataAsset`, PrimaryAssetType `PlayerClass`) for the three playable classes. **This PR does not contain any `.uasset` / `.umap` / `.umeta` instances.** Claude Code creates the Data Assets in the editor; Priya soft-refs them from class-select HUD; Gary fills GAS tags.

Numeric defaults, display names, and roles come from `Docs/design-reference/Aeternii_Isometric_Demo.html` `CLASSES` (draft PR #3). **Order names** follow Jacob’s canon correction (via Nadia): Igni Orthodoxia, Genitorii Gothica, Luminarch Collegium — not the isometric demo’s older order strings. Constructor / `ApplyCanonDefaults` hold those values in C++ — they are not baked into Content.

## Expected Content paths

Create three `UPlayerClassData` assets (Data Asset factory → Player Class Data):

| ClassId | Order | Display name | Role | Asset |
| --- | --- | --- | --- | --- |
| `ordo` | Igni Orthodoxia | Sworn of the Black Sigil | Bulwark · Melee | `/Game/Variant_Combat/Player/DA_PlayerClass_Ordo` |
| `ignivarum` | Genitorii Gothica | Ignivarum, Fire-Bearer | Inquisitor · Aggressor | `/Game/Variant_Combat/Player/DA_PlayerClass_Ignivarum` |
| `luminarch` | Luminarch Collegium | Luminarch Navigator-Priest | Noetic · Ranged | `/Game/Variant_Combat/Player/DA_PlayerClass_Luminarch` |

Object paths for `TSoftObjectPtr<UPlayerClassData>`:

- `/Game/Variant_Combat/Player/DA_PlayerClass_Ordo.DA_PlayerClass_Ordo`
- `/Game/Variant_Combat/Player/DA_PlayerClass_Ignivarum.DA_PlayerClass_Ignivarum`
- `/Game/Variant_Combat/Player/DA_PlayerClass_Luminarch.DA_PlayerClass_Luminarch`

Helpers: `UPlayerClassData::GetExpectedAssetPath(ClassId)`, `PlayerClassAsset::PathOrdo` / `PathIgnivarum` / `PathLuminarch`.

## Editor fill

1. Create the DA at the path above.
2. Set **Class Id** to `ordo` / `ignivarum` / `luminarch` (or name the asset `DA_PlayerClass_*` so it can be inferred).
3. Click **Apply Canon Defaults From Editor** (or call `ApplyCanonDefaults` / `ApplyCanonDefaultsForId`).
4. Leave **Character Class** and **Preview Mesh** unset — Claude Code assigns the Combat character BP / mesh.
5. Leave **Ability Tag** / **Ability Class** empty on each slot — Gary owns real tags.

## Priya (class-select HUD)

Soft-ref these three DAs. Bind cooldown wells to `AbilitySlots[n].AbilityTag` once Gary lands tags (slots 0–2 are canon abilities; slot 3 is reserved empty). Do not hardcode display names or stats in UMG if the DA is available.

Suggested future tag names (not registered here): `Ability.Class.VigilStep`, `Ability.Class.AshenRecitation`, `Ability.Class.EffigyVow`, `Ability.Class.CenserLunge`, `Ability.Class.CensureLance`, `Ability.Class.SecondBreath`, `Ability.Class.Fold`, `Ability.Class.Unwriting`, `Ability.Class.Palindrome`. Placeholder **Ability Id** values on the DA already match the demo (`vigil_step`, `fold`, …).

## Units

Isometric-demo combat numbers (`BaseHealth` 150–230, `BaseMoveSpeed` ~3.05–3.55, `AttackRange` ~1.15–6.4) are **not** Unreal centimetres / the current `ACombatCharacter` HP scale (demo MaxHP is 5). Convert when wiring movement, traces, and damage.

## Out of scope (this PR)

- HUD / UMG (Priya)
- GAS ability implementations and native tags (Gary)
- `.uasset` Data Asset instances
- Enemy AI, boss, maps
