#pragma once

#include "Pokemon.hpp"
#include <array>

struct Team
{
	std::array<Pokemon, 6> party;

	private:
		int firstNotDefeated() {
			int pokemonIdx = 0;

			for (int i = 0; i < (int)party.size(); i++) {
				const Pokemon& pkmn = party.at(i);

				if (pkmn.currentHP > 0) {
					pokemonIdx = i;
					break;
				}
			}

			return pokemonIdx;
		}

		int activePokemon = firstNotDefeated();

	public:
		std::array<Pokemon, 6> getDefeatedPokemon() const {
			std::array<Pokemon, 6> def = {};

			for (int i = 0; i < (int)party.size(); i++) {
				const Pokemon& pkmn = party.at(i);

				if (!pkmn.isNotDefeated()) {
					def[i] = pkmn;
				}
			}

			return def;
		}

		bool checkDefeated(const std::string& name) const {
			for (const auto& poke : party) {
				if (poke.isDefeated() && poke.name == name) return true;
			}
			return false;
		}

		Pokemon& inBattle() { return party[activePokemon]; }
		const Pokemon& inBattle() const { return party[activePokemon]; }

		unsigned int countAlive() const {
			int count = 0;

			for (const auto& poke : party) {
				if (poke.currentHP > 0) { count++; }
			}

			return count;
		}

		unsigned int countDefeated() const { return (unsigned int)party.size() - countAlive(); }

		bool hasAlivePokemon() const { return countAlive() > 0; }

		void switchActivePokemon(unsigned int pokemonIndex) {
			if (party[pokemonIndex].isNotDefeated()) {
				activePokemon = pokemonIndex;
			}
		}
};