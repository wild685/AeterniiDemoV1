# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

**AeterniiDemoV1** is an Unreal Engine 5.8 C++ project containing three self-contained gameplay variants: Combat (third-person melee), Platforming (3D platformer), and Side Scrolling (2D side-scroller). The single runtime module is `AeterniiDemoV1`.

## Build Commands

Build via **Unreal Build Tool** from the command line (adjust UE install path as needed):

```
# Build game (Development)
"C:\Program Files\Epic Games\UE_5.8\Engine\Build\BatchFiles\Build.bat" AeterniiDemoV1 Win64 Development "D:\game-creation\UnrealPRJ\Aeternii\AeterniiDemoV1\AeterniiDemoV1.uproject"

# Build editor
"C:\Program Files\Epic Games\UE_5.8\Engine\Build\BatchFiles\Build.bat" AeterniiDemoV1Editor Win64 Development "D:\game-creation\UnrealPRJ\Aeternii\AeterniiDemoV1\AeterniiDemoV1.uproject"
```

Or open `AeterniiDemoV1.sln` in Visual Studio and build from there. The `.slnx` files are the newer Visual Studio solution format.

## Module Dependencies

Declared in `Source/AeterniiDemoV1/AeterniiDemoV1.Build.cs`:
- `EnhancedInput` — all input handling
- `AIModule` — AI controllers
- `StateTreeModule` + `GameplayStateTreeModule` — AI behavior (StateTree, not Behavior Trees)
- `UMG` + `Slate` — UI widgets

Enabled plugins: `StateTree`, `GameplayStateTree`, `ModelContextProtocol`, `ModelingToolsEditorMode`.

## Architecture

### Variant Structure

Each gameplay mode lives in its own subdirectory and is completely self-contained:

```
Source/AeterniiDemoV1/
├── AeterniiDemoV1Character/GameMode/PlayerController  ← abstract base classes
├── Variant_Combat/
│   ├── AI/           ← AIController, Enemy, Spawner, StateTree tasks, EQS contexts
│   ├── Gameplay/     ← ActivationVolume, CheckpointVolume, DamageableBox, LavaFloor, Dummy
│   ├── Interfaces/   ← ICombatAttacker, ICombatDamageable
│   └── UI/           ← CombatLifeBar widget
├── Variant_Platforming/
│   └── Animation/    ← AnimNotify_EndDash
└── Variant_SideScrolling/
    ├── AI/           ← AIController, NPC, StateTree utility
    ├── Gameplay/     ← JumpPad, MovingPlatform, Pickup, SoftPlatform
    └── UI/           ← SideScrollingUI
```

All variant include paths are registered in `Build.cs` so headers can be included without path prefixes.

### Combat Variant

- `ACombatCharacter` — player character implementing `ICombatAttacker` + `ICombatDamageable`. Handles combo attack strings (montage sections), press-and-hold charged attacks, HP/ragdoll death, and respawning.
- `ACombatEnemy` — AI enemy implementing the same two interfaces. Exposes `FOnEnemyAttackCompleted` and `FOnEnemyLanded` delegates for StateTree tasks to wait on.
- `ACombatAIController` — runs `UStateTreeAIComponent`; all AI logic lives in StateTree assets, not C++ tick.
- **Interfaces**: `ICombatAttacker` (`DoAttackTrace`, `CheckCombo`, `CheckChargedAttack`) and `ICombatDamageable` (`ApplyDamage`, `HandleDeath`, `ApplyHealing`, `NotifyDanger`) are the contract between player and enemy. Both player and enemy implement both interfaces.
- **StateTree tasks/conditions** in `CombatStateTreeUtility.h`: `FStateTreeComboAttackTask`, `FStateTreeChargedAttackTask`, `FStateTreeWaitForLandingTask`, `FStateTreeFaceActorTask`, `FStateTreeFaceLocationTask`, `FStateTreeSetCharacterSpeedTask`, `FStateTreeGetPlayerInfoTask`, `FStateTreeCharacterGroundedCondition`, `FStateTreeIsInDangerCondition`.
- **EQS**: `EnvQueryContext_Player` and `EnvQueryContext_Danger` provide query contexts for enemy spatial awareness.

### Platforming Variant

- `APlatformingCharacter` — 3D platformer with dash (montage-driven), double jump, wall jump (sphere trace), coyote time. Movement state is packed into a `uint8` bitfield.
- `AnimNotify_EndDash` — anim notify that calls `EndDash()` to cleanly terminate the dash state from the animation.

### Side Scrolling Variant

- `ASideScrollingCharacter` — 2D movement (single horizontal axis) with wall jump, double jump, coyote time, soft platform drop-through (per-object collision channel), and sphere-trace interaction.
- `ASideScrollingCameraManager` — custom `APlayerCameraManager` subclass for the fixed 2D camera rig.
- `ASideScrollingNPC` / `ASideScrollingAIController` — NPC with StateTree AI; `FStateTreeGetPlayerTask` resolves the player pawn and proximity check each state entry.
- Gameplay actors: `ASideScrollingJumpPad`, `ASideScrollingMovingPlatform`, `ASideScrollingPickup`, `ASideScrollingSoftPlatform`.

### Input Pattern

All characters expose `Do*` virtual methods (`DoMove`, `DoLook`, `DoJumpStart`, `DoJumpEnd`, `DoDash`, `DoInteract`, `DoComboAttackStart`, etc.) that are called both from Enhanced Input bindings and from UI widgets. This allows mobile/touch UI to drive the same logic without duplicating it.

### StateTree Pattern

AI tasks are `USTRUCT`s inheriting `FStateTreeTaskCommonBase`; conditions inherit `FStateTreeConditionCommonBase`. Each task/condition has a paired `*InstanceData` USTRUCT that holds context bindings and parameters. Tasks disable tick by default (`bShouldCallTick = false`) and use `EnterState`/`ExitState` for one-shot actions, using delegates on the enemy to signal completion back to the tree.
