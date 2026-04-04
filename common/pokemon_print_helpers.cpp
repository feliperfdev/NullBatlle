#include "pokemon_print_helpers.hpp"

void clearScreen() {
    // \033[H moves the cursor to the top left
    // \033[2J clears the entire screen
    std::cout << "\033[H\033[2J\033[3J" << std::flush;
}

std::string printPokemonData(const Pokemon& pokemon) {
	if (pokemon.name.empty() || pokemon.level == 0) return "";

	std::string hpBar = "";
	int barLength = 20;
	float hpRatio = (float)pokemon.currentHP / (float)pokemon.maxHP();
	int filled = (int)(hpRatio * barLength);

	for (int i = 0; i < barLength; i++) {
		hpBar += (i < filled) ? "=" : "-";
	}

	std::string condition = "OK";
	if (pokemon.isBurned())    condition = "BRN";
	if (pokemon.isPoisoned())  condition = "PSN";
	if (pokemon.isParalyzed()) condition = "PAR";
	if (pokemon.isAsleep())    condition = "SLP";
	if (pokemon.isFreezed())   condition = "FRZ";

	std::string result = "";
	result += "\n[-----------------------------------]\n";
	result += "| " + pokemon.name + " (Lv." + std::to_string(pokemon.level) + ")  [" + condition + "]\n";
	result += "| HP: [" + hpBar + "] " + std::to_string(pokemon.currentHP) + "/" + std::to_string(pokemon.maxHP()) + "\n";
	result += "| ATK:" + std::to_string(pokemon.attack()) +
		"  DEF:" + std::to_string(pokemon.defense()) +
		"  SPE:" + std::to_string(pokemon.speed()) + "\n";
	result += "| Moves: ";
	for (auto& m : pokemon.moves) {
		result += m.name + "(" + std::to_string(m.pp[0]) + "/" + std::to_string(m.pp[1]) + ") ";
	}
	result += "\n";
	result += "[-----------------------------------]";

	return result;
}

void printBattlePanel() {
    std::cout << "\n";

    std::cout << "\n";
    std::cout << "+------------------------------------------+\n";
    std::cout << "| [1] Batalhar    [2] Trocar Pokemon       |\n";
    std::cout << "| [3] Usar Item   [4] Render               |\n";
    std::cout << "+------------------------------------------+\n";
    std::cout << "";
}

void printMovesPanel(const Pokemon& pokemon, int playerId) {
    auto typeStr = [](Types t) -> std::string {
        switch (t) {
        case Types::FIRE:     return "FIRE";
        case Types::GRASS:    return "GRASS";
        case Types::WATER:    return "WATER";
        case Types::NORMAL:   return "NORMAL";
        case Types::ELECTRIC: return "ELECTRIC";
        case Types::PSYCHIC:  return "PSYCHIC";
        case Types::ICE:      return "ICE";
        case Types::DRAGON:   return "DRAGON";
        case Types::DARK:     return "DARK";
        case Types::FIGHTING: return "FIGHTING";
        case Types::POISON:   return "POISON";
        case Types::GROUND:   return "GROUND";
        case Types::FLYING:   return "FLYING";
        case Types::BUG:      return "BUG";
        case Types::ROCK:     return "ROCK";
        case Types::GHOST:    return "GHOST";
        case Types::STEEL:    return "STEEL";
        default:              return "???";
        }
        };

    auto moveCell = [&](int idx) -> std::string {
        const Move& m = pokemon.moves.at(idx);
        bool noPP = m.pp[0] == 0;

        std::string name = m.name;
        if (name.size() > 12) name = name.substr(0, 11) + ".";

        std::string pp = std::to_string(m.pp[0]) + "/" + std::to_string(m.pp[1]);
        std::string type = typeStr(m.type);

        // Linha 1: [N] NOME
        std::string line1 = "[" + std::to_string(idx + 1) + "] " + name;
        // Linha 2: TYPE         PP X/X
        int pad = 18 - (int)type.size() - (int)pp.size();
        std::string line2 = type + std::string(std::max(1, pad), ' ') + "PP " + pp;

        if (noPP) line1 = "--- " + name, line2 = type + " (SEM PP)";

        return line1 + "\n    " + line2;
        };

    std::cout << "\n";
    std::cout << "  Player " << playerId << " > Escolha um golpe:\n";
    std::cout << "+---------------------+---------------------+\n";
    std::cout << "| " << std::left << std::setw(20) << moveCell(0).substr(0, moveCell(0).find('\n'))
        << "| " << std::setw(20) << moveCell(1).substr(0, moveCell(1).find('\n')) << "|\n";
    std::cout << "| " << std::setw(20) << ("    " + moveCell(0).substr(moveCell(0).find('\n') + 5))
        << "| " << std::setw(20) << ("    " + moveCell(1).substr(moveCell(1).find('\n') + 5)) << "|\n";
    std::cout << "+---------------------+---------------------+\n";
    std::cout << "| " << std::setw(20) << moveCell(2).substr(0, moveCell(2).find('\n'))
        << "| " << std::setw(20) << moveCell(3).substr(0, moveCell(3).find('\n')) << "|\n";
    std::cout << "| " << std::setw(20) << ("    " + moveCell(2).substr(moveCell(2).find('\n') + 5))
        << "| " << std::setw(20) << ("    " + moveCell(3).substr(moveCell(3).find('\n') + 5)) << "|\n";
    std::cout << "+---------------------+---------------------+\n";
    std::cout << "  Golpe: ";
}

void printVictoryScreen(const Player& winner, int totalTurns) {
    auto hpBar = [](int current, int max, int width = 20) {
        int filled = std::max(0, (int)((float)current / max * width));
        std::string bar = "[";
        for (int i = 0; i < width; i++) bar += (i < filled) ? "#" : "-";
        return bar + "]";
        };

    // Encontra o pokémon ativo sobrevivente
    const Pokemon* survivor = nullptr;
    for (const auto& p : winner.team.party) {
        if (p.isNotDefeated()) { survivor = &p; break; }
    }

    int kos = 0;
    for (const auto& p : winner.team.party) {
        if (!p.isNotDefeated()) kos++;
    }

    std::cout << "\n";
    std::cout << "  * * * * * * * * * * * * * * * * * * *\n";
    std::cout << "\n";
    std::cout << "              V I T O R I A !\n";
    std::cout << "\n";
    std::cout << "           VENCEDOR: PLAYER " << winner.id << "\n";
    std::cout << "          vitoria por knockout\n";
    std::cout << "\n";
    std::cout << "  +-----------------------------------+\n";
    std::cout << "  |  Turnos:      " << std::left << std::setw(20) << totalTurns << "|\n";
    std::cout << "  |  KOs:         " << std::setw(20) << kos << "|\n";
    std::cout << "  +-----------------------------------+\n";

    if (survivor) {
        std::cout << "\n";
        std::cout << "  Pokemon sobrevivente:\n";
        std::cout << "  +-----------------------------------+\n";

        std::string nameLevel = survivor->name + "  Lv." + std::to_string(survivor->level);
        std::cout << "  | " << std::left << std::setw(34) << nameLevel << "|\n";

        std::string hpStr = std::to_string(survivor->currentHP) + " / " + std::to_string(survivor->maxHP()) + " HP";
        std::cout << "  | " << std::setw(34) << hpStr << "|\n";

        std::string bar = hpBar(survivor->currentHP, survivor->maxHP());
        std::cout << "  | " << std::setw(34) << bar << "|\n";
        std::cout << "  +-----------------------------------+\n";
    }

    std::cout << "\n";
    std::cout << "  * * * * * * * * * * * * * * * * * * *\n";
    std::cout << "\n";
    std::cout << "        [ pressione ENTER para sair ]\n";
    std::cout << "\n";

    std::cin.ignore();
    std::cin.get();
}