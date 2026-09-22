# TODO — lore / flavor (deferred)

Playable demo first. Do **not** block combat / HUD / class-select wiring on final lore.
Jacob 2026-09-21: class info is close but not final; update when 3D assets and lore embedding start.

## Open (Nadia — mismatches already flagged)

- [ ] Rewrite player-class flavor blurbs in `UPlayerClassData` / isometric-demo copy that still say “First Flame knight-monk”, “war-bred censer-bearer”, “psyker-navigator” — replace with Brief-aligned blurbs for **Igni Orthodoxia / Genitorii Gothica / Luminarch Collegium** when lore pass starts.
- [ ] Handoff Brief still marks playable classes as “Not written”; promote the three locked DisplayNames into the Brief (or replace that row) so Brief is no longer behind code.
- [ ] Align Ghurazi placeholder naming and any demo HTML wording that still uses isometric-prototype copy (Raider / Chanter / Mauler role labels can stay).
- [ ] Drop leftover **Stahlvolkh** and **Kriegstadt Dynasts** framing from docs/UI if any remain — Jacob 2026-09-20: both out of demo scope; demo antagonist is **Ghurazi** (space barbarians).
- [ ] Embed fourth Noetic Art display string **Aether Blight** (false-reality / decoy) into Brief + ability UI when Gary wires the 4 GAS names (Existence Shift, Memory Burn, Temporal Echo, Aether Blight).

## Locked for playable (do not churn)

- Player-facing class DisplayNames only: **Igni Orthodoxia**, **Genitorii Gothica**, **Luminarch Collegium** (no order/faction subtitle).
- Demo enemy faction: **Ghurazi** — archetypes Raider / Chanter / Mauler.
- Fourth Noetic Art: **Aether Blight**.
