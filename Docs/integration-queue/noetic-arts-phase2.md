# Integration queue — Phase 2 Noetic Arts (GAS)

**PR:** https://github.com/wild685/AeterniiDemoV1/pull/10 (`feature/noetic-arts-gas`)
**Status: blocked on Jacob.** No `UGameplayAbility` subclasses were added.

Trigger, cooldown, effect, and cost do not agree between the two HTML demos for any of the four locked arts. This ticket records both sides. It does not blend them and it does not pick a winner.

## Sources

Design files are not on `master`. They were read from branch `cursor/add-design-reference-files-8aea` (PR #3, head `bb87882`), which adds them unchanged:

| File | Role |
| --- | --- |
| `Docs/design-reference/Aeternii_Isometric_Demo.html` | Realtime action demo. Nine class-kit spells tagged with four art labels. `const ABIL` and `castAbility`. |
| `Docs/design-reference/Aeternii_Prototype_Demo.html` | Turn-based grid demo. Three shared Noetic Arts. `const ABIL`, `tileClick`, `resolveMemoryBurn`, `spawnDecoy`. |
| `Docs/design-reference/Aeternii_Demo_Handoff_Brief.md` | Narrative types only (reposition, truth-exposure damage, repeat previous action, decoys). Section 5 says these are not numbers yet. Not used as a numeric source. |
| `Docs/TODO-lore.md` (on `master`) | Locks the fourth display string as **Aether Blight**. That string does not appear in either HTML file. |

Depth / Corruption **meter math** on `UAeterniiAttributeSet` (`AeterniiCorruptionMeter`, `AddCorruption`, `CorruptionAfterUnmake`) stays the isometric meter. This phase did not call those helpers, because the arts' spend and refund amounts themselves disagree (and the prototype adds flat `cor`, not `addCorr`).

Pack / horde / weapon damage stays on `ICombatDamageable::ApplyDamage`. Weapon combos stay on `ICombatAttacker`. Aura stays `Enabled: false`. No `.uasset` / `.umap` edits.

## Name sheet

These classes and tags are the names to use **after** Jacob unblocks a source. They are **not** in `Source/` on this branch. Do not grant them, and do not fill `FPlayerClassAbilitySlot::AbilityClass`.

| Canon art | Class (not added) | Ability tag (not added) | Cooldown tag (not added) |
| --- | --- | --- | --- |
| Existence Shift | `UGA_ExistenceShift` | `Ability.Noetic.ExistenceShift` | `Cooldown.Noetic.ExistenceShift` |
| Memory Burn | `UGA_MemoryBurn` | `Ability.Noetic.MemoryBurn` | `Cooldown.Noetic.MemoryBurn` |
| Temporal Echo | `UGA_TemporalEcho` | `Ability.Noetic.TemporalEcho` | `Cooldown.Noetic.TemporalEcho` |
| Aether Blight | `UGA_AetherBlight` | `Ability.Noetic.AetherBlight` | `Cooldown.Noetic.AetherBlight` |

Intended folder, once unblocked: `Source/AeterniiDemoV1/Variant_Combat/GAS/Abilities/`. Grant path, once unblocked: `ACombatCharacter::GrantDefaultAbilities` via `UCombatAbilitySystemComponent::GrantAbilityIfMissing`, same pattern as `UGA_Dodge`. `Build.cs` already exposes `Variant_Combat/GAS` on the include path; a new `Abilities` folder would need its own include entry.

`UPlayerClassData` on `master` already stores the **isometric kit ids** (`vigil_step`, `ashen_recitation`, `effigy_vow`, `censer_lunge`, `censure_lance`, `second_breath`, `fold`, `unwriting`, `palindrome`) with empty `AbilityTag` / `AbilityClass`. Those nine kits are not the four GAS arts. Filling the slots now would silently choose the isometric kits.

## Why this stopped

The isometric demo is one realtime character with three slots (`1`/`Q`, `2`/`E`, `3`/`R`), aimed at the cursor, each slot a different kit per class. The prototype is a turn-based grid: arm an ability, click a tile, and the action ends the unit's turn (`cost: "ACT"`). Cooldowns, ranges, damage, and corruption are different systems, not the same numbers with different labels.

Per-art detail follows. Every row is taken from the file named in the source column.

### 1. Existence Shift — blocked

| | Isometric (`castAbility` / `ABIL`) | Prototype (`existence_shift`) |
| --- | --- | --- |
| Ids | `vigil_step` (Ordo), `censer_lunge` (Ignivarum), `fold` (Luminarch). Art label `Existence Shift`. | One id: `existence_shift`. Name `Existence Shift`. |
| Trigger | Key `1` or `Q` (slot 0). Destination is cursor world position `IN.wx` / `IN.wy`. | Arm the ability, then click an empty tile. Sets `acted` and `moved`. |
| Cooldown | `vigil_step` 7s, `censer_lunge` 6s, `fold` 5s. `G.cd[slot] = A.cd`. | No second cooldown. Cost is one action (`cost: "ACT"`). |
| Range | `vigil_step` 3.4, `censer_lunge` 4.6, `fold` 5.6 (world units). Blink walks in 0.2 steps and **stops at collision** (`fits`). | Chebyshev range **3** tiles. Landing tile must be empty. Path ignores intervening bodies. Rupture tiles are legal landings (`blinkTiles` does not reject them; `decoyTiles` does). |
| Effect | `vigil_step`: blink, then `buffs.dr = {t:1.5, amt:0.40}` (40% of incoming harm refused for 1.5s). `fold`: blink only. `censer_lunge`: blink, and `hurtFoe(f, 26)` for foes within 0.85 of the segment. | Teleport. `truth += 8` (clamped 0–100). If `G.depth >= 3`, `cor += 6` (flat, not `addCorr`). Then `applyRupture`. |
| Corruption cost on cast | None. `castAbility` does not call `addCorr`. | Flat `+6` corruption only when depth index is 3 or 4 (prototype depth is 1–4). No `corrRate` scale. |

### 2. Memory Burn — blocked

| | Isometric | Prototype (`memory_burn`) |
| --- | --- | --- |
| Ids | `ashen_recitation` (Ordo), `censure_lance` (Ignivarum), `unwriting` (Luminarch). Art label `Memory Burn`. | One id: `memory_burn`. Name `Memory Burn`. `kind: "burn"`. |
| Trigger | Key `2` or `E` (slot 1), aimed at cursor when the kit uses a direction. | Arm, then click an enemy within range. |
| Cooldown | `ashen_recitation` 6.5s, `censure_lance` 5s, `unwriting` 4.2s. | No second cooldown. Cost is one action. |
| Range / shape | `ashen_recitation`: `rng` 0 in `ABIL`; cast radius **2.9** around the player. `censure_lance`: line length **7.2**, width **0.72**, stops at collision. `unwriting`: place up to **7** along the cursor; damage radius **2.0**, applied when the fx reaches t ≥ 0.36 (duration 0.72). | Chebyshev range **3**. Single target. |
| Effect | `ashen_recitation`: `hurtFoe(f, 38)` and `slow = {t:2.2, amt:0.45}` (foe speed × 0.55). `censure_lance`: `hurtFoe(f, 46)`, pierces. `unwriting`: `hurtFoe(f, 44)` inside radius 2.0. `hurtFoe` is flat HP, not the armor / depth formula. | Cannot miss (`acc: null`). Damage = `9 + floor(target.truth * 0.22) + stratum.noetic * 4`. Stratum `noetic` is 0 / 1 / 2 / 3 for Lux / Velum / Abyssii / Ruptura. Attacker `truth += 15`. Target `truth += 6`. If the target is a decoy, the decoy dies and the attacker still gains 15 truth; no HP formula. |
| Corruption cost on cast | None. | If `G.depth >= 3`, attacker `cor += 8` (flat). |

### 3. Temporal Echo — blocked

The prototype catalogue has no Temporal Echo, palindrome, second breath, or "repeat previous action" ability. The handoff brief names the type and gives no numbers.

| | Isometric | Prototype |
| --- | --- | --- |
| Ids | `second_breath` (Ignivarum), `palindrome` (Luminarch). Art label `Temporal Echo`. Ordo has no Temporal Echo slot. | **Absent.** |
| Trigger | Key `3` or `R` (slot 2). `rng` 0. Not aimed. | — |
| Cooldown | `second_breath` 18s, `palindrome` 16s. | — |
| Effect | `second_breath`: `buffs.haste = {t:5, spd:1.45, atk:0.55}` and `buffs.purge = {t:5}`. Haste multiplies move speed by 1.45 and sets attack cooldown multiplier to 0.55 for 5s. Description string: "Replay 5s of yourself. Corruption burns off." It does not replay a stored action. `palindrome`: history sampled every 0.12s (cap 60). Picks the newest sample with `G.t - r.t >= 3.0`, else the oldest sample. Teleport to that position (`clearSpot`), `hp = max(current, recorded)` clamped to max, `corr = max(0, min(current, recorded) - 14)`. Description: "Be where and what you were 3s ago." | — |
| Corruption | `second_breath` does not call `addCorr`. Purge makes `addCorr` scale the incoming amount by **0.15**, and the update loop subtracts **6 * dt** while purge is up (same constants as `AeterniiCorruptionMeter::PurgeIncomingScale` and `PurgeDecayPerSecond`). `palindrome` writes corruption directly: `min(current, recorded) - 14`, floored at 0. That `-14` is not `CorruptionAfterUnmake` (which refunds 1 when depth > 0). | — |

### 4. Aether Blight — blocked

Locked display name on `master` is **Aether Blight** (`Docs/TODO-lore.md`). Neither demo uses that string.

| | Isometric `effigy_vow` | Prototype `false_reality` |
| --- | --- | --- |
| Name in file | `Effigy of the Vow`, art label `False Reality`. Ordo only. Ignivarum and Luminarch have no decoy slot. | `False Reality Projection`. |
| Trigger | Key `3` or `R` (slot 2). Aimed at cursor. | Arm, then click an empty non-rupture tile. |
| Cooldown / charges | Cooldown **17s**. One decoy (`G.decoy`). A new cast replaces the previous object. | **2 charges** (`charges: 2`), spent one per cast. No second cooldown. Also costs the action. |
| Range | **5** world units along the cursor, then `clearSpot`. | Chebyshev **2**. Tile must be empty and not a rupture. |
| Effect | Decoy `{hp:95, hpMax:95, life:7.5}`. Removed when `t > 7.5` or `hp <= 0`. Foes retarget it when `distanceToDecoy * 0.38 < distanceToPlayer`. Hits on it subtract `f.def.dmg` from decoy HP. Hazards in range also damage it. It does not intercept by swapping the hit onto itself as a 1-HP lie. | Decoy `hp: 1`, `maxHp: 1`. At end of round, `decoyAge++`; at `decoyAge >= 2` it unmakes itself ("unbelieved"). If a Star attack selects it, the likeness dies and the owner is untouched. Star targeting sorts decoys ahead of real units. |
| Corruption cost on cast | None. | None. Charge spend only. |

## What Jacob needs to pick

One source per art, for all four of trigger, cooldown, effect, and cost. Open questions that block the GAS classes:

1. Are the UE abilities the four shared art names, or the nine isometric class kits (`vigil_step`, `fold`, …)?
2. Realtime seconds cooldowns (isometric) or turn action / charges (prototype)?
3. For Aether Blight: aggro effigy (95 HP, 7.5s, cooldown 17, range 5) or intercept decoy (1 HP, 2 rounds, 2 charges, range 2)? And is the display string `Aether Blight` while the mechanical source stays one of the demo names?
4. For Temporal Echo: `second_breath` (5s haste + purge), `palindrome` (3s state rewind, corruption `min - 14`), the brief's "repeat previous action", or something else? The prototype has no vote.
5. Corruption spends that are flat prototype `cor += N` are not `UAeterniiAttributeSet::AddCorruption`. If those costs are chosen, say whether they should go through `AddCorruption` (isometric curve, `amount * CorruptionRate`, purge scale 0.15) or stay flat.

Until that lands, do not add the ability classes with placeholder constants.
