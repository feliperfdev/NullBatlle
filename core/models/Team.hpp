#pragma once

#include "Pokemon.hpp"
#include <array>

struct Team
{
	std::array<Pokemon, 6> party ;
	int activePokemon;
	std::array<bool, 6> defeated;

	public:
		Pokemon& inBattle() { return party.at(activePokemon); }
};