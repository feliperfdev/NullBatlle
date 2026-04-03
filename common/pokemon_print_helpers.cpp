#include "pokemon_print_helpers.hpp"

std::string printPokemonData(const Pokemon& pokemon) {
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