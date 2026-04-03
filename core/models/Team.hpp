#pragma once

#include "Pokemon.hpp"
#include <array>

struct Team
{
	std::array<Pokemon, 6> party;

	private:
		int firstNotDefeated() {
			int pokemonIdx = 0;

			for (int i = 0; i < party.size(); i++) {
				Pokemon pkmn = party.at(i);

				if (pkmn.currentHP > 0) {
					pokemonIdx = i;
					break;
				}
			}

			return pokemonIdx;
		}

		int activePokemon = firstNotDefeated();

	public:
		Pokemon& inBattle() { return party.at(activePokemon); }

		std::array<Pokemon, 6> getDefeatedPokemon() {
			std::array<Pokemon, 6> def = {};

			for (int i = 0; i < party.size(); i++) {
				Pokemon& pkmn = party.at(i);

				if (!pkmn.isNotDefeated()) {
					def[i] = pkmn;
				}
			}

			return def;
		}

		bool hasAlivePokemon() {
			bool hasAtLeastOneAlive = false;

			for (auto& poke : party) {
				if (poke.isNotDefeated()) {
					hasAtLeastOneAlive = true;
				}
			}

			return hasAtLeastOneAlive;
		}

		void switchActivePokemon(unsigned int pokemonIndex) {
			if (party[pokemonIndex].isNotDefeated()) {
				activePokemon = pokemonIndex;
			}
		}
};