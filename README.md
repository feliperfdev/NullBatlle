# NullBattle

A turn-based Pokémon battle engine written in C++17, replicating Generation 3 mechanics — damage formula, type effectiveness, speed/priority ordering, and status conditions.

---

## Overview

NullBattle is a from-scratch battle simulator that loads all 386 Gen 1–3 Pokémon and their moves from JSON data files, builds live Pokémon instances with base stats, and runs a full 1v1 battle loop through a finite state machine. The architecture is split into three independent libraries: `nullbattle_core` (battle logic), `nullbattle_data` (JSON loading), and `nullbattle_ui` (UI layer, in progress).

---

## Tech Stack

| Layer | Technology |
|---|---|
| **Language** | C++17 |
| **Build System** | CMake 3.10+ |
| **Compiler** | MSVC (with Hot Reload support) |
| **JSON Parsing** | [nlohmann/json](https://github.com/nlohmann/json) v3.12.0 (via FetchContent) |
| **UI** | Dear ImGui *(planned)* |
| **Testing** | Catch2 *(planned)* |
| **Platform** | Windows |

---

## How the Game Works

### 1. Data Loading

On startup, `DataLoader` reads two JSON files and populates in-memory maps:

- **`moves.json`** → `std::map<std::string, Move>` — all Gen 1–3 moves with PP, power, priority, category, accuracy, and type.
- **`pokemon.json`** → `std::vector<PokemonTemplate>` — all 386 Pokémon with base stats and learnsets.

Each `PokemonTemplate` exposes a `build(level, moveNames, movesMap)` factory that creates a live `Pokemon` instance with scaled stats and an assigned moveset.

### 2. Battle Flow (State Machine)

The `BattleStateMachine` drives the entire battle through the following states:

```
TEAM_SELECT
    └─→ ACTION_TURN          ← waits for both players to choose an action
            └─→ ACTION_EXECUTING_TURN   ← TurnEngine resolves and applies actions
                    └─→ END_TURN        ← cleans up, checks for faints
                            ├─→ SWITCH_AFTER_FAINT   ← forced switch on KO
                            ├─→ SWITCH_IN            ← voluntary switch
                            ├─→ USE_ITEM              ← item consumption
                            └─→ BATTLE_END            ← one team has no Pokémon left
```

Each player submits a `BattleAction` (Use Move / Switch Pokémon / Use Item) before `ACTION_EXECUTING_TURN` resolves them both.

### 3. Turn Ordering

`TurnEngine::determineOrder()` resolves who acts first using a four-level priority system that mirrors Gen 3 rules:

1. **Switch priority** — switching always acts before attacking.
2. **Move priority bracket** — moves with a higher `priority` value (e.g., Quick Attack = +1) act before normal moves.
3. **Speed stat** — the faster Pokémon acts first within the same priority bracket.
4. **Speed tie** — randomly resolved 50/50.

### 4. Damage Calculation

`TurnEngine::executeMoveAction()` applies the Generation 3 damage formula:

```
damage = floor((floor(2 * level / 5 + 2) * power * attack / defense) / 50) + 2
```

The formula is implemented. Currently `attack` and `defense` always resolve to the Physical stats (Atk/Def); the Special branch (SpAtk/SpDef for `category == 2`) is not yet wired in.

**Not yet implemented:** STAB (×1.5), type effectiveness, critical hits, random damage roll (0.85–1.00), and burn halving.

### 5. Accuracy

Before damage is applied, `TurnEngine::executeMoveAction()` rolls a value from 1–100 and compares it against `move.accuracy`. A move with `accuracy == 0` always hits.

### 6. Status Conditions

The `Pokemon` struct tracks `battleCondition` as an integer flag:

| Value | Condition |
|---|---|
| 0 | None |
| 1 | Burn |
| 2 | Sleep |
| 3 | Paralysis |
| 4 | Poison |
| 5 | Freeze |
| 6 | Confusion |
| 7 | Attraction |

Status flags are stored and query helpers (`isBurned()`, `isParalyzed()`, etc.) are implemented on `Pokemon`. End-of-turn effects (burn chip damage, poison damage, sleep counter, paralysis speed drop) and a dedicated `StatusManager` are not yet implemented.

### 7. Type Effectiveness

`typechart.json` stores the full 18×18 Gen 3 effectiveness matrix. The `Types` enum and string-to-type conversion (`typeFromString()`) are implemented in `Types.hpp`. Integration into the damage calculator is planned.

---

## Project Structure

```
NullBattle/
├── CMakeLists.txt          # Root build — defines targets and FetchContent deps
├── CMakePresets.json       # Debug / Release presets
├── NullBattle.cpp/.h       # Entry point and main game loop
│
├── core/                   # Pure battle logic — no UI dependency
│   ├── CMakeLists.txt      # Builds nullbattle_core static library
│   ├── battle/
│   │   ├── BattleState.hpp              # State enum + string map
│   │   ├── BattleAction.hpp             # ActionType enum + BattleAction struct
│   │   ├── BattleStateMachine.hpp/.cpp  # State transitions and main loop
│   │   └── TurnEngine.hpp/.cpp          # Turn ordering and damage application
│   └── models/
│       ├── Pokemon.hpp     # Stats, HP, types, moves, battle condition + accessors
│       ├── Move.hpp        # PP, power, priority, accuracy, category, type
│       ├── Team.hpp        # party[6], active index, defeated flags
│       ├── Player.hpp      # ID, team, bag
│       ├── Item.hpp        # Name, itemType, healEffect, healHP
│       ├── Bag.hpp         # items[100]
│       └── Types.hpp       # 19-value type enum (Gen 3 + FAIRY + NONE) and TypesMap
│
├── data/                   # JSON loading — depends on core
│   ├── CMakeLists.txt      # Builds nullbattle_data static library
│   ├── DataLoader.hpp/.cpp # nlohmann/json → core structs
│   ├── models/
│   │   ├── PokemonTemplate.hpp  # Template + build() factory
│   │   └── LearnsetEntry.hpp    # Move name + learn level
│   ├── pokemon.json        # 386 Pokémon — base stats and learnsets
│   ├── moves.json          # Gen 1–3 moves — power, PP, category, accuracy, type
│   └── typechart.json      # 18×18 type effectiveness matrix (Gen 3)
│
├── ui/                     # Dear ImGui layer — reads state, never mutates
│   └── CMakeLists.txt      # Builds nullbattle_ui (in progress)
│
├── python/                 # Data pipeline scripts
│   └── fetch_pokemon.py    # Converts raw data → project JSON format
│
└── tests/                  # Catch2 unit tests (planned)
    ├── test_damage.cpp      # STAB, crit, resistance scenarios
    ├── test_typechart.cpp   # All 18×18 type combinations
    ├── test_status.cpp      # Burn/poison/sleep end-of-turn effects
    └── test_turn_order.cpp  # Speed tie, Quick Attack priority
```

---

## Building

Requires CMake 3.10+ and MSVC. `nlohmann/json` is fetched automatically.

```bash
cmake --preset debug
cmake --build out/build/debug
```

Or use the **Release** preset:

```bash
cmake --preset release
cmake --build out/build/release
```

---

## Data Format

### `pokemon.json`
```json
{
  "id": 1,
  "name": "bulbasaur",
  "types": ["grass", "poison"],
  "stats": [45, 49, 65, 49, 65, 45],
  "learnset": [
    { "move": "tackle", "level": 1 },
    { "move": "vine-whip", "level": 9 }
  ]
}
```

Stats order: `[MaxHP, Atk, SpAtk, Def, SpDef, Speed]`

### `moves.json`
```json
{
  "name": "flamethrower",
  "type": "fire",
  "category": 2,
  "power": 95,
  "accuracy": 100,
  "pp": 15,
  "priority": 0
}
```

Categories: `1` = Physical, `2` = Special, `0` = Status

---

## Implementation Status

| Feature | Status |
|---|---|
| JSON data loading (Pokémon + moves) | Done |
| Core domain models | Done |
| Battle state machine | Done |
| Turn ordering (speed + priority) | Done |
| Accuracy roll | Done |
| Gen 3 damage formula (base) | Done |
| Physical vs Special stat selection | Planned |
| STAB modifier | Planned |
| Type effectiveness in damage | Planned |
| Critical hits | Planned |
| Random damage roll (0.85–1.00) | Planned |
| Burn halving attack in damage | Planned |
| Status condition flags + query helpers | Done |
| Status condition effects (end-of-turn) | Planned |
| Item data structures | Done |
| Item usage in battle | Planned |
| UI (Dear ImGui) | Planned |
| Unit tests (Catch2) | Planned |
