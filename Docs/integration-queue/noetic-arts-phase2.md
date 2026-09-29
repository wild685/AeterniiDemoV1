# Integration queue — Phase 2 Noetic Arts (GAS)

**PR:** https://github.com/wild685/AeterniiDemoV1/pull/10 (`feature/noetic-arts-gas`)
**Status: unblocked.** Jacob chose the isometric demo kits for all four arts. Prototype numbers below are reference only and are not in C++.

## Decision

| Canon art | Class | Isometric kits |
| --- | --- | --- |
| Existence Shift | `UGA_ExistenceShift` | `vigil_step` (ordo), `censer_lunge` (ignivarum), `fold` (luminarch) |
| Memory Burn | `UGA_MemoryBurn` | `ashen_recitation` (ordo), `censure_lance` (ignivarum), `unwriting` (luminarch) |
| Temporal Echo | `UGA_TemporalEcho` | `second_breath` (ignivarum), `palindrome` (luminarch). Ordo has no kit. |
| Aether Blight | `UGA_AetherBlight` | `effigy_vow` (ordo) only. HTML label is Effigy of the Vow / False Reality. Display art name stays **Aether Blight**. |

Source file: `Aeternii_Isometric_Demo.html` `ABIL` + `castAbility` on branch `cursor/add-design-reference-files-8aea` (`bb87882`, PR #3). Not merged to `master`.

One `UGameplayAbility` per art. The applied `ClassId` (`ACombatCharacter::AppliedPlayerClassId`) picks the kit. Keys `1`/`Q`, `2`/`E`, `3`/`R` are slots 0–2 via `DoNoeticSlot`.

Corruption writes go through `UAeterniiAttributeSet`:

- Second Breath purge decay calls `ApplyPurgeDecay` → `AdvancePassiveCorruption` with passive input 0 and `bPurge` (decay `PurgeDecayPerSecond` 6).
- Palindrome calls `CorruptionAfterPalindrome` (max(0, min(current, recorded) − 14)) then `UGE_AeterniiCorruption`. That 14 is not `AddCorruption` and not `CorruptionAfterUnmake` (refund 1).
- The other kits do not spend corruption. `castAbility` does not call `addCorr`.

Foe harm uses `ICombatDamageable::ApplyDamage` (pack `CurrentHP`). It does not write `UAeterniiAttributeSet::Health` and does not use `GE_Damage_Melee`.

## Landed constants (isometric)

| Kit | Art tag | Cooldown tag | Cooldown | Range / effect |
| --- | --- | --- | --- | --- |
| `vigil_step` | `Ability.Noetic.ExistenceShift` | `Cooldown.Noetic.VigilStep` | 7 s | range 3.4, blink, `Status.Noetic.VigilWard` 1.5 s, harm × (1 − 0.40) in `TakeDamage` |
| `censer_lunge` | same | `Cooldown.Noetic.CenserLunge` | 6 s | range 4.6, blink, 26 harm within 0.85 of the seam |
| `fold` | same | `Cooldown.Noetic.Fold` | 5 s | range 5.6, blink |
| `ashen_recitation` | `Ability.Noetic.MemoryBurn` | `Cooldown.Noetic.AshenRecitation` | 6.5 s | `ABIL.rng` 0; radius 2.9, 38 harm, slow 2.2 s × (1 − 0.45) |
| `censure_lance` | same | `Cooldown.Noetic.CensureLance` | 5 s | range 7.2, width 0.72, 46 harm, probe radius 0.1, step 0.25 |
| `unwriting` | same | `Cooldown.Noetic.Unwriting` | 4.2 s | range 7, 44 harm in radius 2.0 after 0.36 s |
| `second_breath` | `Ability.Noetic.TemporalEcho` | `Cooldown.Noetic.SecondBreath` | 18 s | 5 s: MoveSpeed × 1.45, AttackCooldown × 0.55, MaxWalkSpeed × 1.45, purge tag |
| `palindrome` | same | `Cooldown.Noetic.Palindrome` | 16 s | history every 0.12 s (cap 60), sample age ≥ 3 s, isometric health restored upward, corruption drop 14 |
| `effigy_vow` | `Ability.Noetic.AetherBlight` | `Cooldown.Noetic.EffigyVow` | 17 s | range 5, `ANoeticEffigy` 95 HP, life 7.5 s, aggro when `dd * 0.38 < dp` |

Blink step is 0.2 demo units and stops on `ECC_WorldStatic`. `clearSpot` uses ring step 0.5, max radius 4, 12 spokes, entity radius 0.30. The demo's nave fallback `(9.5, 20.5)` is not used.

`UPlayerClassData` canon slots now store the cooldown tag and `/Script/AeterniiDemoV1.GA_*` path. Grant path: `ACombatCharacter::GrantDefaultAbilities` adds the four classes next to `UGA_Dodge`.

## Still missing (editor / design)

- `AeterniiDemoV1Editor` was not compiled here. UE 5.8 is not installed.
- No `.uasset` / `.umap`. `IA_NoeticSlot1/2/3` are not created. Assign them on the combat character, or call `DoNoeticSlot` / `DoExistenceShift` / `DoMemoryBurn` / `DoTemporalEcho` / `DoAetherBlight` from UI.
- `DemoUnitsToCentimeters` defaults to **1** because neither HTML file defines centimeters per demo unit. Ranges then match the HTML numbers in Unreal units (a range of 3.4 is 3.4 cm). Set the property once a scale exists.
- Weapon montages do not read `AttackCooldown`, so Second Breath's × 0.55 lands on `UAeterniiAttributeSet::AttackCooldown` only. MaxWalkSpeed does change.
- Palindrome restores isometric `UAeterniiAttributeSet::Health`, not pack `CurrentHP`.
- `FStateTreeGetPlayerInfoTask` writes the effigy into `TargetPlayerLocation` when the 0.38 test passes. `TargetPlayerCharacter` stays the player. If `ST_CombatEnemy` moves toward the actor pin instead of the vector pin, rebind that pin. The asset was not edited.
- Level hazards do not damage the effigy. Enemy melee traces and horde chant overlaps do, through `ICombatDamageable::ApplyDamage`. The caster's own attacks do not.
- Cooldown blocking uses loose `Cooldown.Noetic.*` tags. `GetCooldownTimeRemainingAndDuration` only sees duration gameplay effects, so it stays 0. `UCombatAbilityCooldownSlot::SetCooldown` is not driven from those timers.
- No cast montages or VFX.

## Prototype reference only (not implemented)

Do not blend these into the kits above.

| Art | Prototype |
| --- | --- |
| Existence Shift | `existence_shift`, Chebyshev range 3, cost `ACT`, truth +8, flat `cor += 6` when depth ≥ 3. No second cooldown. |
| Memory Burn | `memory_burn`, range 3, cannot miss, damage `9 + floor(truth * 0.22) + noetic * 4`, truth +15 / +6, flat `cor += 8` when depth ≥ 3. |
| Temporal Echo | Absent. |
| False Reality Projection | `false_reality`, range 2, 2 charges, 1 HP, unmakes at `decoyAge >= 2`, intercepts the hit. Not `effigy_vow`. |

The handoff brief names the art types and gives no numbers.
