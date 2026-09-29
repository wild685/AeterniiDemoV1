# AETERNII — Video Game Demo: Viability Assessment & Handoff Brief

*Prepared for handoff to a Fable / Opus 5 build session. Source: the "Ignivarum Testamentum" Obsidian vault (Jacob's Aeternii worldbuilding project), reviewed 2026-08-14.*

---

## 1. Verdict: Viable, with real prep work still needed

The universe is unusually deep for a pre-production project — deeper than most indie teams have before they start building. There is already:

- A **formal, one-page AAA game design brief** written for this exact universe (`ChatGPT-Worldbuild/Master Video Game Prompt.md`), specifying genre, core loop, combat system, magic system, character system, a signature mechanic (the Depth System), art direction, audio direction, and even a tagline.
- **~200 markdown files** of cosmology, faction, character, technology, and timeline lore, most written to a consistent template (Character Sheet, Technology Sheet, Civilization Sheet, Planet Sheet), which is exactly the kind of structured data a game needs (stat blocks, ability fluff, enemy rosters).
- A locked-down **visual style guide** (`aeternii_image_generation_guidelines.md`) with hard constraints (no particle spam, no bloom, controlled volumetric light, specific named materials like Aethersteel/Flameglass/Cindersilk) — this is genuinely production-ready art direction, not vibes.
- **~66 pieces of existing concept art** and **4 AI-generated cinematic video clips** already produced in-house, proving the visual identity works and giving a builder real reference material.
- A distinctive, non-generic core hook: the **Depth System** (Lux Stratum → Velum Unda → Abyssii Nihil → Ruptura Originii) ties narrative, combat difficulty, and visual distortion to one unifying mechanic. This is the single most game-ready idea in the whole vault — it's mechanically specific enough to prototype immediately.

What does **not** exist yet: any game engine project, any 3D/2D game assets (only concept-art stills), any audio, any UI mockups, any of the "3 playable classes / 2 sample missions" the design brief itself calls for, and no single consolidated canon document — the lore is spread across ~15 folders including raw ChatGPT/Copilot conversation exports that a fresh AI session would have to hunt through.

**Bottom line:** you have an unusually strong pre-production package, not a demo. The lore-to-mechanics translation and a first playable slice are the actual next steps, and both are buildable now.

---

## 2. What exists in the vault (inventory)

| Category | Status | Key files |
|---|---|---|
| Cosmology / creation myth | Strong, consistent | `Canon Aeternum.md` |
| Political landscape / factions | Strong, consistent | `Political Landscape.md` + `Macro - Civilizations/*` (matches) |
| Timeline / book-by-book plot outline | Strong | `Aeterni Timeline Libri.md` |
| Characters | ~25 sheets, uneven depth (some are 1-line stubs, a few are fully realized) | `VIII. Lore Management/.../Micro - Characters/*` |
| Technology / relics | Templated, several fully written (Eidolon Gates, Aetheric Beacons, Iron Reliquaries) | `4. Technology & Faith/*` |
| Planets | 12 planets stubbed with template only (no content filled in) | `World/Planets/*` |
| Military structure | Strong, detailed org chart | `Exercitus Solariensis - The Solarian War Machine.md` |
| Game design brief | Exists, high-level only | `Master Video Game Prompt.md` |
| Art direction | Production-ready, strict | `aeternii_image_generation_guidelines.md`, `Image Generation Instructions.md` |
| Concept art | ~66 images | vault root, `08 Image Generation/` |
| Cinematic clips | 4 AI video clips | vault root (`Prophecy_of_War_Cutscene.mp4`, etc.) |
| Playable classes | **Not written** | — |
| Sample missions | **Not written** | — |
| UI/UX concept | **Not written** | — |
| Game engine project | **None** | — |
| Audio/score | **None** (direction notes only) | — |
| Consolidated single-source canon | **None** | — |

---

## 3. Consolidated canon (for a fresh AI session)

### Cosmology
Before humanity: an Era of Makers. **The Forgers** (mathematical, divine engineers) stabilized reality and were joined by **The Empyrean** (emotionally divine beings born of observing the chaos, who longed for the Forgers' creative power). The Empyrean tried to make the Forgers unnecessary by building a singularity that forces convergence instead of stabilizing — this became the **Hungering Star**, and its birth tore reality, creating the **Aeonic Wound**. The Forgers vanished; the Empyrean's hubris is the universe's founding sin.

Three primordial forces define everything that follows:
- **The Flame** — order, memory, sacrifice, wholeness.
- **The Star** — hunger, ruin, dissolution, the seduction of oblivion.
- **The Aether** — knowledge, pattern, logic, the danger of "perfect" understanding.

### Human history (condensed timeline)
1. **Cradle of Humanity** — humans awaken on Solterran, discover a dormant Maker beacon during a war; a young scientist becomes the first Noetic Arts practitioner and receives a vision: abandon Solterran or stagnate.
2. **Aetas Lux Ordinata ("Age of Ordered Light")** — under **Solomir** (later deified as "the First Flame"), humanity expands via Noetic navigation, founds the Flame Orthodoxy, achieves FTL.
3. **Aetas Registrorum ("Age of the Registers")** — after Solomir's withdrawal, the **Nobilraad** (a Landsraad-like council of Great Houses) governs by record and precedent rather than living authority. Order calcifies into bureaucracy.
4. **Excidium Solterranae ("The Fall of Solterran")** — in the 60th millennium, an aeonic rupture called **Terra Maesgravia** destroys the homeworld. One ark-city, **Koloss Aeterna**, survives by accident, crash-embeds in the planet Atraxis Prime, and founds a new philosophy: knowledge itself is dangerous and must be rationed. This produces the **Reliquarium Ater (Black Reliquary)** — a containment doctrine, not a shrine — enforced by the **Custodes Molossi Ateris ("Grave Hounds")**.
5. **Post-Solterran era (current game setting)** — fragmented successor states, no unified truth, survival through restraint rather than progress.

### The Trinity of factions (present-day political map — this is the most current, internally consistent faction layer; use it as canon)

**Flame-Aligned** (order, memory, hierarchy — moral core, defensive/resilient combat identity, cathedral aesthetic)
- **Imperialis Daciana** — the original empire of Solomir's Flame; crusading, nostalgic, trying to reclaim lost glory.
- **Solarian Concordium** — militarized reformist rivals to Daciana (Napoleonic tone).
- **Wardens of Gothar** — isolationist zealot border guards (Byzantine frontier tone).
- **Venetiarum Guild** — merchant-lords secretly dealing with both Flame and Star.
- **Sacri Imperium Lumenis** — decadent noble houses clinging to ceremonial power.
- **Ordo Primae Flammae** — fanatical knight-monk order (Adeptus Astartes / Teutonic Order tone); this is the Fire Emblem-style "elite unit" faction.

**Star-Aligned** (entropy, hunger, transcendence — aggressive/corruptive combat identity, cosmic horror aesthetic)
- **Kriegstadt Dynasts** — godless, mechanized total-war state powered by Noctisium.
- **Ghurazi Ecclessiarches** — mad eldritch priesthood devoted to the Elder Star.
- **Ghurazi Horde** — nomadic war-bands, the militant arm of the Ecclessiarches.

**Neutral** (survival, industry, ancient knowledge — adaptive/morally ambiguous combat identity)
- **Obelisk-Kings of Amunari** — tomb-lord stargazers, neither Flame nor Star will engage them.
- **Virellium Choir** — pacifist harmonic isolationists, terrifying if provoked.
- **Seraphate of Cael Anthir** — semi-divine cloistered sovereign state; possibly holds one of the Seven Sigillum.
- **Hrimsulvar Kindr** — frozen-edge survivalist culture, Norse/Inuit tone.

> **Note on the "Stahlvolkh" civilization files** (`Macro - Civilizations/Stahlvolkh (The Steel People)/`): these three sub-states (Neuprusska Duchthron, Tartarr Volkbund, Victorian Khezschmied) do not appear in the current Political Landscape doc and read as an earlier draft of the same concept later consolidated into the Kriegstadt Dynasts. Treat as **superseded/legacy** unless Jacob says otherwise — flag this explicitly for the build session rather than silently merging it.

### The military
The **Exercitus Solariensis** (Flame-aligned war machine) has six branches, each fielding a distinct **Gothorum War-Breed** (genetically engineered super-soldiers, ~7ft/350lb, template: Warhammer 40K Space Marine crossed with Dune's Sardaukar): Strigidae (Owls — fleet command), Molossi (Mastiffs — planetary defense), Ursarum (Bears — expeditionary "last measure" force), Cervorum (Stags — beacon security), Aurochii (ancient reserve force), and **Ignivarum (Fire-Bearers — religious inquisitors, the series' namesake order)**. This is a ready-made unit/class roster for a tactical-RPG roster.

### Signature mechanic: The Depth System
| Layer | Translation | Zone analogy | Combat effect |
|---|---|---|---|
| Lux Stratum | Layer of Light | Surface | Stable combat |
| Velum Unda | Veiled Current | Midnight Zone | Minor distortions |
| Abyssii Nihil | Void Abyss | Abyssal Zone | Environmental chaos |
| Ruptura Originii | The First Rupture | Hadalpelagic Zone | Identity degradation |

Depth affects accuracy, skill outcomes, dialogue options, visual distortion, and enemy types. This is the one system specific and mechanical enough to build a vertical slice around immediately — it doubles as a difficulty curve and a narrative/thematic device (memory instability = the game's core horror).

### Magic system: Noetic Arts
Not elemental — cognitive manipulation of reality. Confirmed ability types: short-range "existence shift" (reposition), memory burn (damage tied to truth exposure), temporal echo (repeat previous action), false-reality projection (decoys that become real if believed enough).

### Key characters available as playable/NPC seeds
- **Cassian Vardemir V** — Praefectus Stellarum, protagonist thread, eventually becomes "the Hollow Throne."
- **Aedric Thorne (Dravurelii)** — Ember-Warden, scholar-knight, becomes "the Living Archive."
- **Lyraen Caelora** — Luminarch navigator-priest, fully fleshed sacrifice arc (strong "support/navigator" class seed).
- **Damaris of Veloria** — colossal War Breed "defect," saved Koloss Aeterna (strong "tank/juggernaut" class seed).
- **Zha'Nuir, the Lamenter of Stars** — herald of the Hungering Star, primary antagonist seed.
- **Solomir** — deified founder-emperor, mythic background figure.

---

## 4. Existing game design foundation (from `Master Video Game Prompt.md`)

- **Genre:** Tactical Strategy RPG + Cinematic Narrative RPG hybrid (Fire Emblem tactics/permadeath weight + Final Fantasy spectacle/story).
- **Core loop:** Narrative Hub (Ark Ship/Fortress/Beacon) → Mission Select → Tactical Grid Battle → Consequence Resolution (permanent) → Character Evolution.
- **Combat:** Turn-based tactical grid, 6–10 unit squads, elevation/terrain/hazards, plus the Depth System layered on top.
- **Character system:** Class + Allegiance (Flame/Star/Neutral) + Loyalty + Relationship bonds affecting combat synergy; optional permadeath; characters can die, corrupt, lose identity, or be rewritten by the Wound.
- **Player role:** "Commander-Observer" — the player doesn't just control units, they choose which version of reality becomes true.
- **Art direction:** Gothic sci-fi cathedrals, IMAX-scale megastructures, volumetric light at focal points only, worn sacred technology. (See the stricter, more actionable `aeternii_image_generation_guidelines.md` for actual production constraints — it overrides the looser references in the master prompt.)
- **Tagline:** *"You do not move through the stars. You decide which stars remain."*

The master prompt's own deliverables list (3 classes, 2 missions, 1 narrative arc, UI/UX, monetization, sequel hooks) has **not** been executed anywhere in the vault — this is the actual first-milestone worklist.

---

## 5. Gaps, risks, and what to fix before/while building

1. **No single canon file.** A cold AI session (or a new hire) has to read 15+ folders including raw chat exports to reconstruct the world. This brief is a stopgap — recommend eventually promoting it (or a longer version) into the vault as the canonical entry point, linked from `Overview.md`.
2. **Faction layer is solid; character/planet layer is thin.** Only ~6 of ~25 characters have real depth; all 12 planets are empty templates. A demo needs maybe 3–5 well-realized locations and units, not all 12+25 — scope down rather than fill in everything.
3. **No mechanical translation of lore into stats/rules.** "Memory burn," "Noetic Arts," permadeath, and the Depth System are all narrative descriptions, not numbers. Someone has to decide: what's the damage formula, what's the resource that fuels abilities, how does Depth numerically modify accuracy, etc. This is standard systems-design work, not a lore problem.
4. **`Rules for Scope Control.md` exists but is novel-scoped, not game-scoped** ("no new factions after 200K words," "every 100K volume ends with a philosophical decision"). Worth writing an equivalent scope-control rule for the game (e.g., "the demo ships with exactly 3 classes and 1 mission; no new systems until the vertical slice works").
5. **Visual style guide and the master prompt's "Reference: Lord of the Rings, Final Fantasy, Dune, Warhammer 40K, Star Wars, Battlestar Galactica, Starcraft" line are in mild tension** — the guidelines document is much stricter (no bloom, no particle spam, no magic aura) than "epic reference to 7 franchises" implies. Defer to the guidelines doc; it's the more recent and more production-usable of the two.
6. **Zero engine assets.** All "game" images in the vault (`Game FP-RPG Screenshot.png`, `Game-Witcher-4K.png`, etc.) are AI-generated style-reference stills, not engine renders. Nothing has been built in Unity, Unreal, or Godot.

---

## 6. Skills / connectors — what's missing to actually build this

Checked against this Cowork environment's installed skills and the MCP connector registry:

**Available now (good news):**
- A **Blender MCP connector is already connected** on Jacob's desktop (`mcp__remote-devices__Blender__*` tools) — usable today for concept 3D blockouts, environment test renders, or turntables of the "Iron Reliquaries" battlemechs, without installing anything new.
- **Windows-MCP / computer-use tools** are connected, so a Claude session can drive whatever's already installed on the desktop (e.g., open and click around in Unity/Unreal/Godot if one of those is installed locally) — but this is UI automation, not a native build pipeline.
- `web-artifacts-builder` skill is enabled — this is the right tool for a **browser-based playable vertical slice** (React/Tailwind, state management), which is the realistic "demo" format achievable inside Cowork today.
- `canvas-design` skill is enabled — usable for quick UI mockups/poster-style art within the constraints of the style guide.

**Missing / would need to be added:**
- **No game-engine MCP connector exists in the registry at all** (searched for Unity, Unreal, Godot, itch.io, Steamworks — none found). There is currently no way for a Claude session to author or build an actual Unity/Unreal/Godot project remotely. If the real target is a native engine build, that work has to happen on Jacob's machine directly in the engine, with Claude assisting via code/asset generation rather than driving the engine itself. Worth checking Anthropic's connector directory periodically — this could change.
- **No installed "game design" or "game development" skill** in this account (searched the skill catalog — none exist yet, only generic docx/pptx/xlsx/pdf/canvas-design/web-artifacts-builder). Worth using `skill-creator` to turn this brief + the image-generation guidelines + the scope-control rules into a standing "Aeternii house style" skill, so every future session (including Fable/Opus 5 runs) automatically inherits the canon and constraints instead of re-deriving them.
- **No audio/music generation connector** — the design calls for a "choir-driven orchestral score," and nothing in the registry search surfaced an audio-gen tool. Would need to be sourced externally (e.g., a human composer, or a separate tool outside this environment) for anything beyond placeholder/silent builds.
- **No 3D asset marketplace or texture-library connector** — if the demo needs anything beyond blockouts, Jacob would be sourcing/making textures and models manually or via Blender.

**Practical recommendation:** for a real "demo" in the sense of *something Jacob can click through and show someone*, the achievable near-term target inside this environment is a **browser-based vertical slice** (HTML/CSS/JS or React) of the tactical grid + Depth System + one Noetic Art ability + one Flame vs. Star encounter, skinned to the style guide using the existing concept art as reference. A native Unity/Unreal build is a separate, longer-term track that needs the engine running locally with Jacob (or a human/AI pair) at the keyboard.

---

## 7. Recommended staged plan

**Stage 1 (this session):** This brief — canon consolidation, viability call, gap list. Done.

**Stage 2 (handed to Fable/Opus 5, running now):** Build a small, self-contained, playable browser prototype demonstrating:
- A tactical grid battle (small squad vs. small squad)
- At least one unit from a Flame-aligned class and one Star-aligned enemy
- The Depth System affecting a visible stat (e.g., accuracy) and the scene's visual treatment
- One Noetic Art ability in action
- Visual treatment that respects the hard constraints in the style guide (no particle/bloom spam, controlled volumetric light, dark cathedral-sci-fi palette)

**Stage 3 (future, not started):** Scope and write the "3 playable classes / 2 sample missions / UI-UX concept" deliverables the master prompt calls for, formal systems design (damage math, resource economy), and a decision on native engine vs. continued web build for a larger vertical slice.
