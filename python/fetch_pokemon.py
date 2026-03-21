"""
NullBattle — Gen 3 Data Fetcher
================================
Busca dados da PokeAPI e gera:
  - pokemon.json   (386 pokémon com base stats + learnset simplificado)
  - moves.json     (todos os moves da Gen 3)
  - typechart.json (matriz de efetividade 18x18)

Uso:
    pip install requests
    python fetch_gen3_data.py

Os arquivos serão salvos na pasta ./data/
"""

import requests
import json
import os
import time

BASE_URL = "https://pokeapi.co/api/v2"
OUTPUT_DIR = "./data"
DELAY = 0.2  # segundos entre requests para não sobrecarregar a API

# Tipos da Gen 3 na ordem do jogo (índice 0 = normal, etc.)
TYPE_NAMES = [
    "normal", "fire", "water", "electric", "grass", "ice",
    "fighting", "poison", "ground", "flying", "psychic", "bug",
    "rock", "ghost", "dragon", "dark", "steel", "fairy"
]

# Mapeamento de nome do tipo para índice (1-based, igual ao seu enum)
TYPE_INDEX = {name: i + 1 for i, name in enumerate(TYPE_NAMES)}


def get(url: str) -> dict:
    """GET com retry simples."""
    for attempt in range(3):
        try:
            r = requests.get(url, timeout=15)
            r.raise_for_status()
            return r.json()
        except Exception as e:
            print(f"  Erro ({attempt+1}/3): {e}")
            time.sleep(1)
    raise RuntimeError(f"Falha ao buscar: {url}")


# ─────────────────────────────────────────────
# 1. pokemon.json
# ─────────────────────────────────────────────

def fetch_pokemon():
    print("\n=== Buscando pokémon (1–386) ===")
    pokemon_list = []

    for poke_id in range(1, 387):
        print(f"  [{poke_id}/386] ", end="", flush=True)

        data = get(f"{BASE_URL}/pokemon/{poke_id}")

        # Base stats na ordem: maxHP, Atk, SpAtk, Def, SpDef, Spe
        stats_map = {s["stat"]["name"]: s["base_stat"] for s in data["stats"]}
        stats = [
            stats_map.get("hp", 0),
            stats_map.get("attack", 0),
            stats_map.get("special-attack", 0),
            stats_map.get("defense", 0),
            stats_map.get("special-defense", 0),
            stats_map.get("speed", 0),
        ]

        # Tipos (até 2)
        types = [t["type"]["name"] for t in data["types"]]

        # Learnset simplificado: apenas moves aprendidos por level-up na Gen 3
        learnset = []
        for move_entry in data["moves"]:
            for version_detail in move_entry["version_group_details"]:
                vg = version_detail["version_group"]["name"]
                method = version_detail["move_learn_method"]["name"]
                level = version_detail["level_learned_at"]

                if vg in ("ruby-sapphire", "emerald", "firered-leafgreen") and method == "level-up":
                    learnset.append({
                        "move": move_entry["move"]["name"],
                        "level": level
                    })
                    break  # evita duplicatas entre versões da Gen 3

        # Remove duplicatas e ordena por level
        seen = set()
        learnset_clean = []
        for entry in sorted(learnset, key=lambda x: x["level"]):
            if entry["move"] not in seen:
                seen.add(entry["move"])
                learnset_clean.append(entry)

        pokemon_list.append({
            "id": poke_id,
            "name": data["name"],
            "types": types,
            "stats": stats,
            "learnset": learnset_clean
        })

        print(data["name"])
        time.sleep(DELAY)

    return pokemon_list


# ─────────────────────────────────────────────
# 2. moves.json
# ─────────────────────────────────────────────

def fetch_moves():
    print("\n=== Buscando moves da Gen 3 ===")

    # Busca lista de todos os moves
    all_moves_data = get(f"{BASE_URL}/move?limit=10000")
    all_move_urls = {m["name"]: m["url"] for m in all_moves_data["results"]}

    # Descobre quais moves existem na Gen 3 (ruby-sapphire, emerald, firered-leafgreen)
    gen3_version_groups = {"ruby-sapphire", "emerald", "firered-leafgreen"}

    moves_list = []
    total = len(all_move_urls)

    for i, (move_name, url) in enumerate(all_move_urls.items()):
        data = get(url)

        # Verifica se o move existe em alguma versão Gen 3
        version_groups = {vg["version_group"]["name"] for vg in data.get("contest_combos", {}) or {}}
        in_gen3 = any(
            vg["version_group"]["name"] in gen3_version_groups
            for vg in data.get("machines", [])
        )

        # Alternativa mais confiável: checar a generation
        generation = data.get("generation", {}).get("name", "")
        introduced_gen = int(generation.replace("generation-", "").replace(
            "i", "1").replace("ii", "2").replace("iii", "3").replace(
            "iv", "4").replace("v", "5").replace("vi", "6").replace(
            "vii", "7").replace("viii", "8")) if generation else 99

        # Converte geração romana para número
        gen_map = {
            "generation-i": 1, "generation-ii": 2, "generation-iii": 3,
            "generation-iv": 4, "generation-v": 5, "generation-vi": 6,
            "generation-vii": 7, "generation-viii": 8
        }
        introduced_gen = gen_map.get(generation, 99)

        # Só inclui moves introduzidos até Gen 3
        if introduced_gen > 3:
            continue

        move_type = data.get("type", {}).get("name", "normal")
        damage_class = data.get("damage_class", {}).get("name", "status")

        # category: 1 = físico, 2 = especial, 0 = status
        # Em Gen 3, físico/especial é determinado pelo tipo, não pelo move
        # Mas guardamos o damage_class da API como referência
        category_map = {"physical": 1, "special": 2, "status": 0}
        category = category_map.get(damage_class, 0)

        moves_list.append({
            "name": move_name,
            "type": move_type,
            "type_index": TYPE_INDEX.get(move_type, 1),
            "category": category,
            "power": data.get("power"),       # null se status
            "accuracy": data.get("accuracy"), # null se sempre acerta
            "pp": data.get("pp", 0),
            "priority": data.get("priority", 0),
            "effect": data.get("effect_entries", [{}])[0].get("short_effect", "") if data.get("effect_entries") else ""
        })

        print(f"  [{i+1}] {move_name} (gen {introduced_gen})")
        time.sleep(DELAY)

    return moves_list


# ─────────────────────────────────────────────
# 3. typechart.json
# ─────────────────────────────────────────────

def fetch_typechart():
    print("\n=== Buscando tabela de tipos ===")

    # Matriz 18x18 inicializada com 1.0 (neutro)
    # chart[atacante][defensor] = multiplicador
    chart = {atk: {dfd: 1.0 for dfd in TYPE_NAMES} for atk in TYPE_NAMES}

    for type_name in TYPE_NAMES:
        print(f"  {type_name}")
        data = get(f"{BASE_URL}/type/{type_name}")

        relations = data["damage_relations"]

        for t in relations["double_damage_to"]:
            chart[type_name][t["name"]] = 2.0

        for t in relations["half_damage_to"]:
            chart[type_name][t["name"]] = 0.5

        for t in relations["no_damage_to"]:
            chart[type_name][t["name"]] = 0.0

        time.sleep(DELAY)

    # Converte para formato de matriz indexada (mais fácil de usar em C++)
    # chart_indexed[atk_index][def_index] = multiplicador (índices 1-based)
    chart_indexed = {}
    for atk_name in TYPE_NAMES:
        atk_idx = TYPE_INDEX[atk_name]
        chart_indexed[atk_idx] = {}
        for def_name in TYPE_NAMES:
            def_idx = TYPE_INDEX[def_name]
            chart_indexed[atk_idx][def_idx] = chart[atk_name][def_name]

    return {
        "type_names": TYPE_NAMES,
        "type_index": TYPE_INDEX,
        "chart": chart_indexed
    }


# ─────────────────────────────────────────────
# Main
# ─────────────────────────────────────────────

def main():
    os.makedirs(OUTPUT_DIR, exist_ok=True)

    # Pokemon
    pokemon = fetch_pokemon()
    path = os.path.join(OUTPUT_DIR, "pokemon.json")
    with open(path, "w", encoding="utf-8") as f:
        json.dump(pokemon, f, indent=2, ensure_ascii=False)
    print(f"\n✓ Salvo: {path} ({len(pokemon)} pokémon)")

    # Moves
    moves = fetch_moves()
    path = os.path.join(OUTPUT_DIR, "moves.json")
    with open(path, "w", encoding="utf-8") as f:
        json.dump(moves, f, indent=2, ensure_ascii=False)
    print(f"✓ Salvo: {path} ({len(moves)} moves)")

    # Typechart
    typechart = fetch_typechart()
    path = os.path.join(OUTPUT_DIR, "typechart.json")
    with open(path, "w", encoding="utf-8") as f:
        json.dump(typechart, f, indent=2, ensure_ascii=False)
    print(f"✓ Salvo: {path}")

    print("\n=== Concluído! Arquivos em ./data/ ===")


if __name__ == "__main__":
    main()