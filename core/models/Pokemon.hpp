#ifndef POKEMON
#define POKEMON

#include "Move.hpp"
#include <array>

enum class BattleCondition : unsigned int {
	NONE      = 0,
	BURNED    = 1,
	ASLEEP    = 2,
	PARALYZED = 3,
	POISONED  = 4,
	FREEZED   = 5,
	CONFUSED  = 6,
	ATTRACTED = 7
};

struct Pokemon
{
	std::string name;
	unsigned int level;
	unsigned int currentHP;

	std::array<Types, 2> types;

	// 0 = maxHP, 1 = Atk, 2 = Sp.Atk, 3 = Def, 4 = Sp.Def, 5 = Spe.
	std::array<unsigned int, 6> stats;

	BattleCondition battleCondition;

	std::array<Move, 4> moves;

	public:
		unsigned int maxHP() const { return stats[0]; }
		unsigned int attack() const { return stats[1]; }
		unsigned int spAttack() const { return stats[2]; }
		unsigned int defense() const { return stats[3]; }
		unsigned int spDefense() const { return stats[4]; }
		unsigned int speed() const { return stats[5]; }

		bool isNotDefeated() const { return currentHP > 0; }
		bool isDefeated() const { return !isNotDefeated(); }

		bool isBurned() const { return battleCondition == BattleCondition::BURNED; }
		bool isAsleep() const { return battleCondition == BattleCondition::ASLEEP; }
		bool isParalyzed() const { return battleCondition == BattleCondition::PARALYZED; }
		bool isPoisoned() const { return battleCondition == BattleCondition::POISONED; }
		bool isFreezed() const { return battleCondition == BattleCondition::FREEZED; }
		bool isConfused() const { return battleCondition == BattleCondition::CONFUSED; }
		bool isAttracted() const { return battleCondition == BattleCondition::ATTRACTED; }

		bool hasAnyBattleCondition() const {
			return battleCondition != BattleCondition::NONE;
		}

		void applyCondition(BattleCondition condition) {
			battleCondition = condition;
		}

		void receiveDamage(int damage) {
			if (currentHP > 0) {
				currentHP -= damage;
			}
		}
};

#endif // POKEMON
