# Combat P1 — GAS-lite boss kit

C++ scaffolding for readable boss combat. This does **not** ship art, montage binaries, or a cooked State Tree asset. Create those in the editor (or Aura) and assign them to the new C++ types.

Module: `AeterniiDemoV1` (UE 5.8). Extends the existing Combat variant — `ACombatBoss` is an `ACombatEnemy`, `ACombatBossAIController` is an `ACombatAIController`. Regular combo enemies are unchanged.

## What landed in C++

| Area | Types |
| --- | --- |
| GAS | `UCombatAbilitySystemComponent`, `UCombatAttributeSet` (Health, MaxHealth, Stamina stub), `UGE_Damage_Melee` |
| Tags | `Ability.Attack`, `Ability.Dodge`, `Status.Dodge.IFrames`, `Data.Damage` |
| Boss GAs | `UGA_Boss_Sweep` / `UGA_Boss_Slam` / `UGA_Boss_Closer` — `PlayMontageAndWait`, own `Ability.Attack` |
| Player GA | `UGA_Dodge` — `PlayMontageAndWait`, own `Ability.Dodge` |
| Notifies | `AN_TellStart`, `ANS_DamageWindow`, `AN_RecoverStart`, `AN_AttackEnd`, `ANS_DodgeIFrames` |
| Pawns | `ACombatBoss` (`Hitbox_Melee`, `Hitbox_AOE`), ASC on `ACombatCharacter` + boss |
| AI | `ACombatBossAIController`, State Tree tasks below |

AI **must** call `TryActivateAbility` (via **Try Activate Boss GA**). It does not `PlayMontage` the attack kit.

## Player dodge acceptance

- Input: `DoDodge()` → `TryActivateAbilityByClass(UGA_Dodge)`.
- Expected dodge montage length: **~0.55s**.
- I-frames: **~0.30s** starting **~0.05s** after dodge starts. Place `ANS_DodgeIFrames` from **0.05s → 0.35s**.
- That notify adds/removes loose tag `Status.Dodge.IFrames`. Hitboxes and `ApplyDamage` / `TakeDamage` skip the target while the tag is present.
- Stamina is **stubbed**: `UCombatAttributeSet::Stamina` exists and is initialized; P1 does not spend it.

## Attack montage layout

Sections (in order): `Tell` → `Commit` → `Active` → `Recover` → `End`

| Attack | Tell | Commit | Active | Recover | Total |
| --- | --- | --- | --- | --- | --- |
| A Sweep | 0.70 | 0.15 | 0.25 | 0.90 | ~2.0s |
| B Slam | 0.90 | 0.20 | 0.20 | 1.20 | ~2.5s |
| C Closer | 0.35 | 0.10 | 0.30 | 0.55 | ~1.3s |

Notify placement:

1. `AN_TellStart` at Tell start (VFX/audio: implement `BP_OnTellStart` on the boss BP).
2. `ANS_DamageWindow` spanning **Active only**. Sweep/Closer: `Hitbox_Melee`. Slam: `Hitbox_AOE`.
3. `AN_RecoverStart` at Recover.
4. `AN_AttackEnd` at End. Montage complete on the GA also `EndAbility` and clears `Ability.Attack`.

Hitboxes are capsules on the boss, **disabled by default**. Opening a window resets a one-hit-per-actor set for that swing.

## Editor setup

Suggested Content paths (create if missing):

```
/Game/Variant_Combat/GAS/Montages/AM_Boss_Sweep
/Game/Variant_Combat/GAS/Montages/AM_Boss_Slam
/Game/Variant_Combat/GAS/Montages/AM_Boss_Closer
/Game/Variant_Combat/GAS/Montages/AM_Dodge
/Game/Variant_Combat/GAS/Abilities/GA_Boss_Sweep   (parent: GA_Boss_Sweep)
/Game/Variant_Combat/GAS/Abilities/GA_Boss_Slam
/Game/Variant_Combat/GAS/Abilities/GA_Boss_Closer
/Game/Variant_Combat/GAS/Abilities/GA_Dodge
/Game/Variant_Combat/GAS/GE_Damage_Melee           (optional BP child of GE_Damage_Melee)
/Game/Variant_Combat/AI/ST_BossCombat
/Game/Variant_Combat/Blueprints/BP_CombatBoss      (parent: CombatBoss)
/Game/Variant_Combat/Blueprints/AIC_Boss           (parent: CombatBossAIController)
```

### 1. Montages (Aura or Persona)

For each boss attack montage:

- Add sections named exactly `Tell`, `Commit`, `Active`, `Recover`, `End` with the timings above.
- Add the four notifies listed under **Attack montage layout**.
- Assign the montage on the matching GA CDO or BP child (`Montage To Play`).

For dodge:

- Length ~0.55s.
- Add `ANS_DodgeIFrames` from 0.05–0.35s.
- Assign on `GA_Dodge` / `UGA_Dodge`.

Until montages are assigned, abilities still activate then immediately `EndAbility` (warning in `LogCombatGAS`) so the State Tree can keep looping.

### 2. Grant abilities

C++ already grants:

- Player: `UGA_Dodge` if `DefaultAbilities` is empty.
- Boss: Sweep / Slam / Closer if `DefaultAbilities` is empty.

On BP children you may replace those with your GA Blueprint classes (so montage refs live on the BP). If you fill `Default Abilities`, the empty-array fallback is skipped — include every attack you want granted.

### 3. Hitboxes

`ACombatBoss` already creates `Hitbox_Melee` and `Hitbox_AOE`. On `BP_CombatBoss`:

- Attach / place `Hitbox_Melee` in front of the weapon/hands.
- Place `Hitbox_AOE` at the slam impact (usually around the root).
- Leave collision disabled; the notify state opens Query-only overlap on Pawns.

Give the boss the same Life Bar widget setup as `ACombatEnemy` (the parent `BeginPlay` still `check()`s it).

### 4. State Tree `ST_BossCombat`

Create a State Tree with schema **StateTree AI Component** (same as existing combat enemies) and assign it on `AIC_Boss` → `StateTreeAI`.

Recommended states and tasks (all under category **Combat|Boss**):

```
Idle_AcquireTarget    task: Acquire Boss Target
        success → ChooseAttack
        fail    → Idle_AcquireTarget (or a short Delay)

ChooseAttack          task: Choose Boss Attack
        success → ActivateAbility
        fail    → MoveCloser          (None / no candidates)

ActivateAbility       task: Try Activate Boss GA
        success → WaitAttackEnd
        fail    → PostAttackGap

WaitAttackEnd         task: Wait While Attacking (timeout 5s)
        → PostAttackGap

PostAttackGap         built-in Delay 1.0s
        → ChooseAttack

MoveCloser            task: Move Closer (duration 0.75s)
        → ChooseAttack
```

Bind **Controller** from the AI context. **Boss** can be left unbound; tasks `Cast<ACombatBoss>(Controller->GetPawn())`.

`Choose Boss Attack` writes `NextAttack` / `LastAttack` on the pawn. Fail means `None` (out of every gate, or only zero-weight leftovers).

Chooser rules (also documented on `ACombatBoss::ChooseAttack`):

- Gates: Sweep 150–400, Slam 100–300, Closer >400 (2D).
- Base weights: 0.50 / 0.30 / 0.20.
- Last Sweep → Sweep×0.4; last Slam → Slam×0.5; never Closer twice.
- Dist>450 → Closer×2, Sweep×0.5; Dist<180 → Slam×1.5, Closer×0.
- Weighted roll; `None` if no candidates.

### 5. Player input

1. Create `IA_Dodge` (Digital / Down).
2. Add it to the combat IMC (e.g. Space is already Jump — use Left Shift, Q, or gamepad East).
3. Assign `IA_Dodge` to `Dodge Action` on `BP_CombatCharacter`.
4. Touch UI can call `DoDodge()` the same way other `Do*` methods work.

## PIE test

1. Compile the editor (`AeterniiDemoV1Editor`). Enable plugin **GameplayAbilities** if the editor asks (already on in `AeterniiDemoV1.uproject`).
2. Open a Combat variant map (or the Third Person map) and place `BP_CombatBoss` with `AIC_Boss` + `ST_BossCombat`.
3. PIE. Confirm `LogCombatGAS` shows `ChooseAttack Dist2D=… → Sweep/Slam/Closer` and `TryActivateAbility … -> ok`.
4. With montages assigned: Tell → Active hitbox → Recover. Stand in `Hitbox_Melee` during Active and lose Health; dodge through Active and take **no** damage (`Status.Dodge.IFrames`).
5. Walk beyond 400 cm and confirm Closers; stay inside 180 cm and confirm Slams are favored and Closer is suppressed.
6. Without montages: abilities end immediately and the tree still loops Choose → Activate → Wait → 1s gap.

## Out of scope (not in this PR)

Full montage/art content, ships, dialogue, open world, inventory, P0 checkpoint/retry loop.
