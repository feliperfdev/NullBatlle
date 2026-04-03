#ifndef ITEM
#define ITEM

#include <string>
#include <optional>

struct Item
{
	std::string name;

	// 0 = User casual; 1 = Heal HP; 2 = Heal condition; 3 = Berry; 4 = Combat Item; 5 = Pokéball;
	unsigned int itemType;

	// 0 = None, 1 = Full heal, 2 = Burn heal, 3 = Antidote, 4 = Awakening, 
	// 5 = Paralyze Heal, 6 = Ice Heal
	unsigned int healEffect;

	std::optional<int> healHP;

	public:

		bool isAHealHpItem() { return healHP.has_value(); }

		bool hasAConditionHealEffect() { return healEffect == 0; }
		
		void healPokemonHP(Pokemon& pokemon) {
			if (!isAHealHpItem()) return;

			unsigned int resultHP = healHP.value() + pokemon.currentHP;

			if (resultHP > pokemon.maxHP()) {
				pokemon.currentHP = pokemon.maxHP();
			}
			else {
				pokemon.currentHP += resultHP;
			}
		}

		void healPokemonCondition(Pokemon& pokemon) {
			if (!pokemon.hasAnyBattleCondition()) return;

			if (healEffect == 1) {
				pokemon.battleCondition = 0;
			}

			if (pokemon.isBurned() && healEffect == 2) {
				pokemon.battleCondition = 0;
			}

			if (pokemon.isPoisoned() && healEffect == 3) {
				pokemon.battleCondition = 0;
			}

			if (pokemon.isAsleep() && healEffect == 4) {
				pokemon.battleCondition = 0;
			}

			if (pokemon.isParalyzed() && healEffect == 5) {
				pokemon.battleCondition = 0;
			}

			if (pokemon.isFreezed() && healEffect == 6) {
				pokemon.battleCondition = 0;
			}
		}
};

#endif // ITEM