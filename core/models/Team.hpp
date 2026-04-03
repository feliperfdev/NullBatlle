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
		Pokemon& inBattle() { return party[activePokemon]; }

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

		bool checkDefeated(std::string name) {
			std::array<Pokemon, 6> def = getDefeatedPokemon();

			for (auto& poke : def) {
				return poke.name == name;
			}

			return false;
		}

		unsigned int countAlive() {
			int count = 0;

			for (auto& poke : party) {
				if (poke.currentHP > 0) { count++; }
			}

			return count;
		}

		unsigned int countDefeated() { return party.size() - countAlive(); }

		bool hasAlivePokemon() { return countAlive() > 0; }

		void switchActivePokemon(unsigned int pokemonIndex) {
			if (party[pokemonIndex].isNotDefeated()) {
				activePokemon = pokemonIndex;
			}
		}
};