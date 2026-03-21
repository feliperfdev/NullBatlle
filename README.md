# NullBattle

## 📁 Estrutura de Pastas: NullBattle

### 📂 Raiz do Projeto

* **CMakeLists.txt** — Raiz do build, define targets e dependências.
* **CMakePresets.json** — Presets de debug/release.
* **.gitignore**
* **README.md**

---

### 📂 core/

*Lógica pura, sem dependência de UI.*

* **CMakeLists.txt** — Lib estática `nullbattle_core`.
* **📂 models/** — *Structs de domínio.*
* **Pokemon.hpp** — Stats, HP atual, status condition.
* **Move.hpp** — Power, type, PP, categoria (físico/especial).
* **Team.hpp** — Array de 6 pokémon, índice do ativo.
* **Types.hpp** — Enum de tipos + tabela de efetividade Gen 3.


* **📂 battle/** — *Motor de batalha.*
* **BattleState.hpp** — Enum de estados (TeamSelect, Turn, SwitchIn, Faint, End).
* **BattleStateMachine.hpp/.cpp** — Transições de estado, loop principal.
* **TurnEngine.hpp/.cpp** — Ordenação por speed + priority.
* **DamageCalc.hpp/.cpp** — Fórmula Gen 3, STAB, crit, random.
* **StatusManager.hpp/.cpp** — Burn, poison, sleep, paralysis (end-of-turn).
* **BattleLog.hpp/.cpp** — Fila de mensagens para a UI consumir.



---

### 📂 ui/

*Camada Dear ImGui, só lê estado, nunca muta.*

* **CMakeLists.txt** — Lib `nullbattle_ui`, linka core + ImGui.
* **App.hpp/.cpp** — Init SDL/SFML + ImGui, game loop principal.
* **BattleView.hpp/.cpp** — Janela principal de batalha, HP bars.
* **TeamSelectView.hpp/.cpp** — Seleção de time (P1 e P2).
* **MoveMenuView.hpp/.cpp** — 4 moves do pokémon ativo, PP display.
* **LogView.hpp/.cpp** — Painel de log scrollável consumindo BattleLog.
* **Theme.hpp** — Paleta de cores e estilo ImGui do NullBattle.

---

### 📂 data/

*Assets de dados, lidos em runtime.*

* **pokemon.json** — Base stats dos 386 + learnset simplificado.
* **moves.json** — Todos os moves Gen 3 com atributos.
* **typechart.json** — Matriz de efetividade 18x18.
* **DataLoader.hpp/.cpp** — Parsing via nlohmann/json → structs do core.

---

### 📂 tests/

*Testes unitários (Catch2), só testam core/.*

* **CMakeLists.txt**
* **test_damage.cpp** — Cenários de dano: STAB, crit, resistência.
* **test_typechart.cpp** — Todas as combinações de tipo.
* **test_status.cpp** — Efeitos de burn/poison/sleep por turno.
* **test_turn_order.cpp** — Speed tie, quick attack priority.

---

### 📂 extern/

*Dependências via CMake FetchContent.*

* **imgui/** — Dear ImGui source.
* **nlohmann_json/** — Header-only JSON parser.
* **Catch2/** — Framework de testes.

---