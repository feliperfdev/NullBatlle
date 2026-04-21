#ifndef ITEM
#define ITEM

#include <string>
#include <optional>
#include "Pokemon.hpp"

enum class ItemType : unsigned int {
	CASUAL    = 0,
	HEAL_HP   = 1,
	HEAL_COND = 2,
	BERRY     = 3,
	COMBAT    = 4,
	POKEBALL  = 5
};

enum class HealEffect : unsigned int {
	NONE          = 0,
	FULL_HEAL     = 1,
	BURN_HEAL     = 2,
	ANTIDOTE      = 3,
	AWAKENING     = 4,
	PARALYZE_HEAL = 5,
	ICE_HEAL      = 6
};

struct Item
{
	std::string name;

	ItemType itemType;

	HealEffect healEffect;

	std::optional<int> healHP;

	public:

		bool isAHealHpItem() const { return healHP.has_value(); }

		bool hasAConditionHealEffect() const { return healEffect != HealEffect::NONE; }

		void healPokemonHP(Pokemon& pokemon) {
			if (!isAHealHpItem()) return;

			unsigned int resultHP = healHP.value() + pokemon.currentHP;

			if (resultHP > pokemon.maxHP()) {
				pokemon.currentHP = pokemon.maxHP();
			}
			else {
				pokemon.currentHP = resultHP;
			}
		}

		void healPokemonCondition(Pokemon& pokemon) {
			if (!pokemon.hasAnyBattleCondition()) return;

			if (healEffect == HealEffect::FULL_HEAL) {
				pokemon.battleCondition = BattleCondition::NONE;
			}

			if (pokemon.isBurned() && healEffect == HealEffect::BURN_HEAL) {
				pokemon.battleCondition = BattleCondition::NONE;
			}

			if (pokemon.isPoisoned() && healEffect == HealEffect::ANTIDOTE) {
				pokemon.battleCondition = BattleCondition::NONE;
			}

			if (pokemon.isAsleep() && healEffect == HealEffect::AWAKENING) {
				pokemon.battleCondition = BattleCondition::NONE;
			}

			if (pokemon.isParalyzed() && healEffect == HealEffect::PARALYZE_HEAL) {
				pokemon.battleCondition = BattleCondition::NONE;
			}

			if (pokemon.isFreezed() && healEffect == HealEffect::ICE_HEAL) {
				pokemon.battleCondition = BattleCondition::NONE;
			}
		}
};

#endif // ITEM
