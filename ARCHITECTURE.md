# NullBattle — Architecture & System Design

## Table of Contents

1. [Project Overview](#1-project-overview)
2. [Technology Stack](#2-technology-stack)
3. [Repository Structure](#3-repository-structure)
4. [Dependency Graph](#4-dependency-graph)
5. [Module Breakdown](#5-module-breakdown)
   - 5.1 [Executable Layer (`NullBattle.exe`)](#51-executable-layer)
   - 5.2 [Core Library (`nullbattle_core`)](#52-core-library-nullbattle_core)
   - 5.3 [Data Library (`nullbattle_data`)](#53-data-library-nullbattle_data)
   - 5.4 [Common Utilities](#54-common-utilities)
6. [Domain Model](#6-domain-model)
7. [Battle FSM](#7-battle-fsm)
8. [Turn Resolution Pipeline](#8-turn-resolution-pipeline)
9. [Condition System](#9-condition-system)
10. [Data Pipeline](#10-data-pipeline)
11. [Build System](#11-build-system)
12. [Persistence & Session Model](#12-persistence--session-model)
13. [Known Issues & Technical Debt](#13-known-issues--technical-debt)
14. [Planned Features (Not Implemented)](#14-planned-features-not-implemented)

---

## 1. Project Overview

NullBattle is a turn-based battle simulator modeled after the Generation 3 Pokémon mechanics, written in C++20. It is structured as a CLI application backed by three independently compiled static libraries. The core battle logic is entirely decoupled from data loading and UI concerns.

The project is in active development. The turn resolution, damage calculation, and status condition systems are functional; STAB, type effectiveness, critical hits, and stat growth formulas are not yet applied.

---

## 2. Technology Stack

| Component | Technology | Details |
|---|---|---|
| Language | C++20 | `set_property(TARGET NullBattle PROPERTY CXX_STANDARD 20)` |
| Build system | CMake 3.10+ | Multi-target with `FetchContent` |
| Build generator | Ninja | Configured via `CMakePresets.json` |
| Compiler | MSVC (`cl.exe`) | Hot Reload via `EditAndContinue` (Debug) |
| JSON library | nlohmann/json 3.12.0 | Header-only, fetched via `FetchContent` |
| Data source | PokeAPI v2 | Offline — consumed only by Python scripts |
| IDE | Visual Studio 2022+ | `.slnx` solution format |
| Planned UI | Dear ImGui | Placeholder directory, not implemented |
| Planned testing | Catch2 | Not implemented |

---

## 3. Repository Structure

```
NullBattle/
├── CMakeLists.txt                  # Root build — defines exe, FetchContent, subdirs
├── CMakePresets.json               # Build presets: x64/x86 × Debug/Release
├── NullBattle.cpp                  # Entry point: main(), game loop, top-level orchestration
├── NullBattle.h                    # Root header — re-exports all exe-level includes
│
├── common/                         # Cross-cutting utilities (compiled into the exe directly)
│   ├── LogQueue.cpp                # Logging queue + file export
│   ├── UuidV4.cpp                  # RFC 4122 UUID v4 generator
│   ├── pokemon_print_helpers.hpp   # Console UI declarations
│   └── pokemon_print_helpers.cpp   # Console UI implementations
│
├── core/                           # nullbattle_core static library — pure battle logic
│   ├── CMakeLists.txt
│   ├── battle/
│   │   ├── BattleAction.hpp        # ActionType enum + BattleAction struct
│   │   ├── BattleState.hpp         # BattleState enum (8 states) + BattleStateMap
│   │   ├── BattleStateMachine.hpp  # FSM interface
│   │   ├── BattleStateMachine.cpp  # FSM implementation
│   │   ├── TurnEngine.hpp          # Turn resolution interface
│   │   ├── TurnEngine.cpp          # Turn resolution implementation
│   │   ├── ConditionEngine.hpp     # Battle condition engine interface
│   │   └── ConditionEngine.cpp     # Battle condition engine implementation
│   └── models/
│       ├── Types.hpp               # Types enum (19 values) + typeFromString()
│       ├── Move.hpp                # MoveCategory enum + Move struct
│       ├── Pokemon.hpp             # BattleCondition enum + Pokemon struct
│       ├── Team.hpp                # Team struct — party[6] + active slot management
│       ├── Player.hpp              # Player struct — id + Team + Bag
│       ├── Item.hpp                # ItemType/HealEffect enums + Item struct
│       └── Bag.hpp                 # Bag struct — vector<Item>
│
├── data/                           # nullbattle_data static library — JSON loading
│   ├── CMakeLists.txt              # Defines DATA_PATH compile-time macro
│   ├── DataLoader.hpp
│   ├── DataLoader.cpp              # Parses moves.json + pokemon.json
│   ├── models/
│   │   ├── PokemonTemplate.hpp     # PokemonTemplate struct + build() factory
│   │   └── LearnsetEntry.hpp       # LearnsetEntry DTO {move, level}
│   ├── moves.json                  # 372 moves (Gen 1–3)
│   ├── pokemon.json                # 386 Pokémon with base stats and learnsets
│   └── typechart.json              # 18×18 type effectiveness matrix (Gen 3)
│
├── ui/                             # Placeholder for Dear ImGui (empty)
│   └── CMakeLists.txt
│
└── python/                         # Offline data pipeline — not a runtime dependency
    ├── fetch_pokemon.py            # PokeAPI fetcher → generates the project's JSON files
    └── data/                       # Local copies of generated JSONs (not versioned)
```

---

## 4. Dependency Graph

```
NullBattle.exe
    │
    ├── nullbattle_core  (static lib)
    │       ├── No external dependencies
    │       └── C++ STL only
    │
    ├── nullbattle_data  (static lib)
    │       ├── nullbattle_core  (PUBLIC link — consumers of data also get core)
    │       └── nlohmann_json::nlohmann_json  (PRIVATE — not propagated)
    │
    ├── nlohmann_json::nlohmann_json  (header-only, FetchContent)
    │
    └── common/  (source files compiled directly into the exe)
            ├── LogQueue.cpp
            ├── UuidV4.cpp
            └── pokemon_print_helpers.cpp
```

Dependency direction is strictly one-way: `core` has zero knowledge of `data`, `ui`, or `common`. The `data` layer consumes `core` types and extends them with JSON hydration. `common` is a utility layer with no domain knowledge.

---

## 5. Module Breakdown

### 5.1 Executable Layer

**Files:** `NullBattle.cpp`, `NullBattle.h`

The executable contains the application entry point and top-level orchestration. It owns the two `Player` instances and the `BattleStateMachine`, which holds references to both players (see `BattleStateMachine.hpp:47-50` for ownership documentation).

Responsibilities:
- `main()` — bootstraps data loading, session UUID, player init, and runs the game loop
- `runPlayersActions()` — CLI prompt loop for both players' input per turn
- `initPlayers()` — hardcoded team setup using `PokemonTemplate::build()`
- `forceSwitchActivePokemon()` — prompts a player to select a replacement after KO
- `saveMatchResult()` — serializes match outcome to `match_<uuid>.json`

The game loop (`NullBattle.cpp:153-184`) is a `while (!battleEnd)` driven by a `switch` over `battleEngine.getState()`. It handles four states directly: `ACTION_TURN`, `ACTION_EXECUTING_TURN`, `SWITCH_AFTER_FAINT`, and `BATTLE_END`. The remaining four FSM states (`TEAM_SELECT`, `END_TURN`, `SWITCH_IN`, `USE_ITEM`) are defined but not handled in the loop.

### 5.2 Core Library (`nullbattle_core`)

Compiled as a static library with no external dependencies beyond the C++ STL. This is the domain layer — it holds all battle logic and domain models.

#### Domain Models (`core/models/`)

| Type | Kind | Key Fields |
|---|---|---|
| `Pokemon` | struct | `name`, `level`, `currentHP`, `types[2]`, `stats[6]`, `battleCondition`, `moves[4]` |
| `Move` | struct | `pp[2]`, `power` (optional), `priority`, `accuracy`, `category`, `type`, `name` |
| `Team` | struct | `party[6]` (fixed array), `activePokemon` (private index) |
| `Player` | struct | `id`, `team: Team`, `bag: Bag` |
| `Item` | struct | `name`, `itemType`, `healEffect`, `healHP` (optional) |
| `Bag` | struct | `items: vector<Item>` |
| `Types` | enum | 19 values (NONE through DRAGON); `typeFromString()` for JSON parsing |
| `BattleCondition` | enum class | NONE, BURNED, ASLEEP, PARALYZED, POISONED, FREEZED, CONFUSED, ATTRACTED |
| `MoveCategory` | enum | DEFAULT, PHYSICAL, SPECIAL |

`stats[6]` layout: `[maxHP, Atk, SpAtk, Def, SpDef, Speed]`. Accessor methods on `Pokemon` (`attack()`, `speed()`, etc.) index into this array.

`moves[4]` always has exactly 4 slots. `pp` is stored as `std::array<int, 2>` — `[currentPP, maxPP]`.

`power` uses `std::optional<int>` — `std::nullopt` for status moves. `Move::isDamageMove()` returns `power.has_value()`.

#### Battle Engines (`core/battle/`)

| Class | Responsibility |
|---|---|
| `BattleStateMachine` | Owns FSM state, stores player actions, delegates to `TurnEngine` |
| `TurnEngine` | Determines turn order, executes move/item/switch actions |
| `ConditionEngine` | Pre-turn action gating, move secondary effects, end-of-turn condition damage |

### 5.3 Data Library (`nullbattle_data`)

Compiled as a static library. Depends on `nullbattle_core` (PUBLIC) and `nlohmann_json` (PRIVATE).

`DATA_PATH` is a compile-time string macro defined in `data/CMakeLists.txt`:
```cmake
target_compile_definitions(nullbattle_data PRIVATE
    DATA_PATH="${CMAKE_SOURCE_DIR}/data/"
)
```

This resolves file paths without hardcoding them in C++ source. Used at `DataLoader.cpp:10` and `DataLoader.cpp:56`.

**`DataLoader`** has two public methods:
- `loadMoves()` → `std::map<std::string, Move>` — key is the move's slug name
- `loadPokemon()` → `std::vector<PokemonTemplate>`

**`PokemonTemplate`** is a factory struct. Its `build(level, moveNames[4], movesMap)` method (`PokemonTemplate.hpp:18`) produces a `Pokemon` instance with stats set directly from `baseStats` (no level-scaling formula applied).

**`LearnsetEntry`** is a plain DTO: `{std::string move, int level}`.

### 5.4 Common Utilities

Compiled directly into the executable (listed in `add_executable()` at `CMakeLists.txt:15`).

| File | Responsibility |
|---|---|
| `LogQueue.cpp` | Logging queue with stdout echo + file export |
| `UuidV4.cpp` | RFC 4122 UUID v4 generation (`std::mt19937_64`) |
| `pokemon_print_helpers.cpp` | Console rendering: battle panel, move panel, HP bars, victory screen |

`LogQueue` is instantiated as a global at `NullBattle.cpp:4` and passed by reference into all engines. It provides:
- `log(tag, text)` — adds to queue and prints to stdout
- `generateLogFile(sessionId)` — drains queue to `log_<uuid>.txt`
- `clearLog()` — clears the queue

`clearScreen()` in `pokemon_print_helpers` uses the ANSI escape sequence `\033[H\033[2J\033[3J`.

---

## 6. Domain Model

```
Player
  id: int
  team: Team
    party: std::array<Pokemon, 6>     ← fixed 6 slots; unused slots are zero-valued
    activePokemon: int                ← private index into party[]
    + inBattle() → Pokemon&
    + countAlive() → int
    + switchActivePokemon(index)
    + hasAlivePokemon() → bool
  bag: Bag
    items: std::vector<Item>

Pokemon
  name: std::string
  level: unsigned int
  currentHP: unsigned int
  types: std::array<Types, 2>         ← types[1] == Types::NONE for mono-type
  stats: std::array<unsigned int, 6>  ← [maxHP, Atk, SpAtk, Def, SpDef, Speed]
  battleCondition: BattleCondition
  moves: std::array<Move, 4>

Move
  pp: std::array<int, 2>             ← [currentPP, maxPP]
  power: std::optional<int>           ← nullopt for status moves
  priority: unsigned int
  accuracy: unsigned int              ← 0 means always-hit
  category: MoveCategory
  type: Types
  name: std::string

PokemonTemplate  (data layer only — not a battle entity)
  id: int
  name: std::string
  types: std::array<Types, 2>
  baseStats: std::array<unsigned int, 6>
  learnset: std::vector<LearnsetEntry>
  + build(level, moveNames[4], movesMap) → Pokemon
```

---

## 7. Battle FSM

`BattleStateMachine` manages the `currentState: BattleState` field. The enum is defined in `core/battle/BattleState.hpp`.

### States

| State | Description | Implemented |
|---|---|---|
| `TEAM_SELECT` | Players pick their starting team | No — skipped in init |
| `ACTION_TURN` | Both players submit their action for the turn | Yes |
| `ACTION_EXECUTING_TURN` | `TurnEngine` resolves and applies both actions | Yes |
| `END_TURN` | Post-turn cleanup, check for KOs | Partial |
| `SWITCH_AFTER_FAINT` | Forced switch after a Pokémon faints | Yes |
| `SWITCH_IN` | Voluntary switch (not a faint) | Defined, not handled in loop |
| `USE_ITEM` | Item use action | Defined, not handled in loop |
| `BATTLE_END` | One team has no remaining Pokémon | Yes |

### State Transition Diagram

```
                    ┌──────────────────────────────┐
                    │         ACTION_TURN          │
                    │  (both players submit action)│
                    └──────────────┬───────────────┘
                                   │ startExecutingTurn()
                                   ▼
                    ┌──────────────────────────────┐
                    │    ACTION_EXECUTING_TURN     │
                    │  (TurnEngine resolves turn)  │
                    └──────┬───────────────┬───────┘
                           │               │
               KO detected │               │ no KO
                           ▼               ▼
              ┌─────────────────┐   ┌─────────────────┐
              │ SWITCH_AFTER_   │   │   (check winner) │
              │    FAINT        │   └────────┬─────────┘
              └────────┬────────┘            │
                       │                     │ all alive
               switch  │                     ▼
               done    │            ┌─────────────────┐
                       │            │   ACTION_TURN    │
                       └───────────►│  (next turn)    │
                                    └─────────────────┘

                    (if all Pokémon on one team fainted)
                                    ▼
                    ┌──────────────────────────────┐
                    │          BATTLE_END          │
                    └──────────────────────────────┘
```

### Action Storage

Player actions are stored as `std::optional<BattleAction>` (`BattleStateMachine.hpp:54-55`). A `BattleAction` is a simple struct: `{ActionType type, int index}`. `ActionType` values are: `USE_MOVE`, `CHOOSE_POKEMON`, `USE_ITEM`, `RENDER`.

---

## 8. Turn Resolution Pipeline

`TurnEngine` is instantiated per-turn inside `BattleStateMachine::executeTurnActions()`, receiving the two stored `BattleAction` values and a `LogQueue` reference.

### Step 1: Order Determination (`determineOrder`)

Priority is evaluated in order, stopping at the first differentiator:

```
1. Switch action always precedes a move action
   (p2 switches + p1 moves → p2 goes first, and vice versa)

2. Move priority field
   (Quick Attack priority=1 beats Tackle priority=0)

3. Speed stat
   (higher speed acts first)

4. Speed tie → coin flip
   std::uniform_int_distribution<int>(1, 2) on std::mt19937
```

Source: `TurnEngine.cpp:20-60`.

### Step 2: Action Execution

Each player's action is executed in the resolved order. The execution path depends on `ActionType`:

**`USE_MOVE` (`executeMoveAction`)** — `TurnEngine.cpp:62-108`:

```
1. move.canUseMove()         → check currentPP > 0
2. conditionAllowsToAction() → pre-turn condition gate (paralysis, sleep, etc.)
3. accuracy roll             → generateRandom(100) <= move.accuracy || accuracy == 0
4. damage calculation (if isDamageMove()):
     dano = ((2 * level / 5 + 2) * power * attacker.attack() / defender.defense()) / 50 + 2
5. defender.receiveDamage(dano)
6. applyUsedMoveConditionIfApplicable(defender, move)  → secondary effects
7. checkPostActionBattleCondition(attacker)            → end-of-turn DoT
```

**Damage formula** is the Gen 3 base formula. **Note:** `attacker.attack()` always uses the physical Attack stat and `defender.defense()` always uses the physical Defense stat, regardless of `MoveCategory`. Special moves do not use Sp.Atk / Sp.Def yet.

**`USE_ITEM` (`executeItemAction`)** — `TurnEngine.cpp:110-119`:
Delegates to `Item::healPokemonHP()` or `Item::healPokemonCondition()` based on `item.isAHealHpItem()` / `item.hasAConditionHealEffect()`.

**`CHOOSE_POKEMON` (`switchActivePokemon`)** — `TurnEngine.cpp:121-123`:
Delegates to `Team::switchActivePokemon(index)`.

### RNG

`TurnEngine` owns a `std::mt19937 m_rng` seeded with `std::random_device{}()` at construction. `ConditionEngine` has its own separate `std::mt19937` instance. There is no shared RNG state between them.

---

## 9. Condition System

`ConditionEngine` is composed inside `TurnEngine` (not injected — instantiated at `TurnEngine.cpp:10`). It exposes three public methods.

### `conditionAllowsToAction(Pokemon&)` — Pre-turn gate

Called before executing a move. Returns `false` to block the action.

| Condition | Blocking logic | Notes |
|---|---|---|
| `PARALYZED` | `(1 / generateRandom(4)) <= 25` | Integer division bug — see §13 |
| `FREEZED` | `(1 / generateRandom(5)) <= 20` | Integer division bug |
| `ASLEEP` | `(1 / generateRandom(3)) <= 33.3` | Integer division bug |
| `ATTRACTED` | `(1 / generateRandom(2)) <= 50` | Integer division bug |
| `CONFUSED` | `(1 / generateRandom(3)) <= 33.3` | If blocked: self-damage with power=40, Physical formula |

Source: `ConditionEngine.cpp:10-72`.

### `applyUsedMoveConditionIfApplicable(Pokemon&, Move&)` — Secondary effects

Applied after damage lands (only if the defender survives — the call at `TurnEngine.cpp:95` is inside the `else` branch of the `dano > defender.currentHP` check).

| Move type | Move name | Effect | Chance |
|---|---|---|---|
| FIRE | ember, fire-punch, flamethrower, fire-blast, fire-fang | BURNED | 10% |
| FIRE | scald, lava-plume, steam-eruption, ice-burn, searing-shot | BURNED | 30% |
| FIRE | sacred-fire | BURNED | 50% |
| FIRE | will-o-wisp | BURNED | 100% |
| ELECTRIC | thunder-shock, thunder-punch, bolt-strike, volt-tackle, thunderbolt | PARALYZED | 10% |
| ELECTRIC | spark, discharge, thunder | PARALYZED | 30% |
| ELECTRIC | zap-cannon | PARALYZED | 50% |
| ELECTRIC | stun-spore, glare | PARALYZED | 75% |
| ELECTRIC | thunder-wave, nuzzle | PARALYZED | 100% |
| ICE | ice-beam, ice-punch, blizzard, powder-snow, freeze-dry, ice-fang | FREEZED | 10% |
| NORMAL | body-slam, lick | PARALYZED | 30% |
| DRAGON | dragon-breath | PARALYZED | 30% |

The chance check uses `(1.0 / seed) <= chance` where `seed` is a random integer from `generateRandom(100)`. For `chance == 1.0`, `seed` is hardcoded to `1`.

Source: `ConditionEngine.cpp:101-247`.

### `checkPostActionBattleCondition(Pokemon&)` — End-of-turn damage

| Condition | Damage per turn |
|---|---|
| `POISONED` | `round(maxHP * 0.125)` |
| `BURNED` | `round(maxHP * 0.0625)` |
| `FREEZED` | `round(maxHP * 0.0625)` (non-canonical — Gen 3 freeze deals no DoT) |

Source: `ConditionEngine.cpp:74-99`.

---

## 10. Data Pipeline

### Runtime: `DataLoader`

At startup, `main()` instantiates `DataLoader(logQueue)` and calls both load methods. The loaded data is passed into `initPlayers()` and then discarded — no data is kept alive in long-term storage beyond the `Player` instances.

```
DataLoader::loadMoves()
  → opens data/moves.json (via DATA_PATH macro)
  → iterates JSON object entries
  → for each entry: extract pp, priority, category (cast to MoveCategory enum),
                    accuracy, power (optional), type (typeFromString())
  → returns std::map<std::string, Move>

DataLoader::loadPokemon()
  → opens data/pokemon.json (via DATA_PATH macro)
  → iterates JSON array
  → for each entry: extract id, name, types[2], stats[6], learnset[]
  → returns std::vector<PokemonTemplate>
```

`typechart.json` is present in the data directory but is **not loaded at runtime** — it exists as a reference for future type effectiveness implementation.

### Offline: Python Pipeline

`python/fetch_pokemon.py` fetches data from `https://pokeapi.co/api/v2` and generates the three JSON files. This script is not part of the CMake build and must be run manually to refresh data.

### Data Formats

**`moves.json`** — object keyed by move slug:
```json
{
  "tackle": { "name": "tackle", "pp": 35, "power": 40, "accuracy": 100,
              "priority": 0, "category": 1, "type": "normal" }
}
```

**`pokemon.json`** — array ordered by Pokédex ID:
```json
[{
  "id": 1, "name": "bulbasaur",
  "types": ["grass", "poison"],
  "stats": [45, 49, 65, 49, 65, 45],
  "learnset": [{"move": "tackle", "level": 1}, ...]
}]
```

**`typechart.json`** — 18×18 matrix (not currently consumed at runtime).

---

## 11. Build System

### CMake Targets

| Target | Type | Source files |
|---|---|---|
| `NullBattle` | Executable | `NullBattle.cpp`, `common/*.cpp` |
| `nullbattle_core` | Static lib | `core/battle/*.cpp` |
| `nullbattle_data` | Static lib | `data/DataLoader.cpp` |
| `nlohmann_json::nlohmann_json` | Interface (header-only) | FetchContent |

### Build Presets (`CMakePresets.json`)

| Preset | Arch | Config | Generator |
|---|---|---|---|
| `x64-debug` | x64 | Debug | Ninja |
| `x64-release` | x64 | Release | Ninja |
| `x86-debug` | x86 | Debug | Ninja |
| `x86-release` | x86 | Release | Ninja |

### Build Commands

```bash
# Configure
cmake --preset x64-debug

# Build
cmake --build out/build/x64-debug
```

### Output Artifacts

```
out/build/x64-debug/
  NullBattle.exe
  NullBattle.pdb           # debug symbols
  core/nullbattle_core.lib
  data/nullbattle_data.lib
  _deps/json-src/          # nlohmann/json unpacked by FetchContent
  match_<uuid>.json        # generated at runtime
  log_<uuid>.txt           # generated at runtime
```

### MSVC Compile Flags (Debug)

`/DWIN32 /D_WINDOWS /W3 /GR /EHsc /MDd /Ob0 /Od /RTC1 -std:c++20 -ZI`

Hot Reload is enabled via CMake policy `CMP0141` with `EditAndContinue` format for Debug configs (`CMakeLists.txt:7-10`).

---

## 12. Persistence & Session Model

Each run generates a session UUID via `UuidV4::generate()` (`NullBattle.cpp:136`). This UUID names both output files.

### Match Result (`match_<uuid>.json`)

Written by `saveMatchResult()` (`NullBattle.cpp:102-127`) at `BATTLE_END`:

```json
{
  "sessionId": "62a004ad-2391-4560-ab2b-730fba5aca88",
  "totalTurns": 9,
  "winnerPlayer": {
    "id": 2,
    "partySize": 6
  }
}
```

### Session Log (`log_<uuid>.txt`)

Written by `logQueue.generateLogFile(session)` (`NullBattle.cpp:178`). Contains every `log()` call from all engines with their tag prefix:

```
[CoreEngine]       Starting session 62a004ad-...
[DataLoader]       Successfully loaded 386 Pokemon!
[BattleStateMachine] ...
[TurnEngine]       Determining battle order...
[ConditionEngine]  Pikachu is paralyzed. It won't let it use a move!
```

---

## 13. Known Issues & Technical Debt

### Integer Division in `ConditionEngine::conditionAllowsToAction()`

`ConditionEngine.cpp:19-43` uses `(1 / generateRandom(N)) <= threshold` where the division is integer division. For any `N > 1`, `1 / N == 0`. The comparison `0 <= 25` is always `true`, meaning every condition check always allows action — the blocking logic never fires. The intended behavior was `(1.0 / generateRandom(N)) <= (threshold / 100.0)`.

### `.cpp` Files Included via `#include`

`LogQueue.cpp` and `UuidV4.cpp` are `#include`-d in multiple headers (`BattleStateMachine.hpp:12`, `TurnEngine.hpp`, `ConditionEngine.hpp`, `DataLoader.hpp`) in addition to being listed in `add_executable()`. The `#pragma once` guard in `LogQueue.cpp` prevents ODR violations within a single translation unit, but this pattern is non-standard and fragile. The correct approach is to expose only headers and let the linker handle the symbols.

### No Stat Scaling by Level

`PokemonTemplate::build()` copies `baseStats` directly into `Pokemon::stats` without applying the Gen 3 stat growth formula (`((2 * base + iv + ev/4) * level / 100) + 5`). All Pokémon at level 5 fight with their base stat values.

### Physical/Special Split Not Applied

`TurnEngine.cpp:83` always uses `attacker.attack()` (physical Atk) and `defender.defense()` (physical Def) regardless of `MoveCategory`. Special moves like Thunderbolt and Flamethrower do not use Sp.Atk / Sp.Def.

### No STAB

Same-Type Attack Bonus (1.5× multiplier when move type matches the attacker's type) is not implemented in the damage formula.

### No Type Effectiveness

`typechart.json` is present but not loaded. The 2×/0.5×/0× effectiveness multipliers are not applied in damage calculation.

### No Critical Hit Roll

The Gen 3 critical hit mechanic (1/16 base chance, 2× damage) is not implemented.

### No Random Damage Roll

Gen 3 applies a random factor of 85–100 divided by 100 to the final damage. This roll is absent — damage is always deterministic given the same stats and move.

### `conditionAllowsToAction` Does Not Handle BURNED or POISONED

`ConditionEngine.cpp:17-65` has no cases for `BURNED` or `POISONED`. Burned/poisoned Pokémon are never action-gated (correct), but fall through to the `default: allows = true` branch, which is harmless.

### `BattleState` Switch in Game Loop Is Non-Exhaustive

`NullBattle.cpp:155-183` handles only 4 of the 8 `BattleState` values. `TEAM_SELECT`, `END_TURN`, `SWITCH_IN`, and `USE_ITEM` have no `case` branches. If the FSM transitions to any of these states, the loop spins silently.

---

## 14. Planned Features (Not Implemented)

| Feature | Notes |
|---|---|
| STAB multiplier | 1.5× when move type == attacker type |
| Type effectiveness | Load `typechart.json` at runtime; apply multiplier in damage formula |
| Critical hits | Gen 3 formula: 1/16 base rate, stage system for high-crit moves |
| Random damage roll | `rand * [85, 100] / 100` applied to final damage |
| Stat scaling by level | Gen 3 formula in `PokemonTemplate::build()` |
| Physical/Special split | Use `Sp.Atk` / `Sp.Def` when `MoveCategory == SPECIAL` |
| TEAM_SELECT state | Interactive team selection before battle |
| Dear ImGui UI | `ui/` directory is a placeholder |
| Catch2 unit tests | No test infrastructure exists |
| Volatile conditions duration | Sleep (2–5 turns), confusion (2–5 turns), freeze thaw chance |
| Leech Seed | `data/moves.json` includes it; no implementation |
| Multi-turn moves | Hyper Beam recharge, Fly/Dig invulnerability frames |
| Ability system | No ability data or engine exists |
| Weather effects | No weather state in FSM |
