#ifndef POKEMON
#define POKEMON

#include "Move.hpp"
#include <array>

struct Pokemon
{
	std::string name;
	unsigned int level;
	unsigned int currentHP;

	std::array<Types, 2> types;
	
	// 0 = maxHP, 1 = Atk, 2 = Sp.Atk, 3 = Def, 4 = Sp.Def, 5 = Spe.
	std::array<unsigned int, 6> stats;

	// 0 = None
	// 1 = Burned
	// 2 = Sleep
	// 3 = Paralyzed
	// 4 = Poisoned
	// 5 = Freezed
	// 6 = Confusion
	// 7 = Attracted
	unsigned int battleCondition;

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

		bool isBurned() const { return battleCondition == 1; }
		bool isAsleep() const { return battleCondition == 2; }
		bool isParalyzed() const { return battleCondition == 3; }
		bool isPoisoned() const { return battleCondition == 4; }
		bool isFreezed() const { return battleCondition == 5; }
		bool isConfused() const { return battleCondition == 6; }
		bool isAttracted() const { return battleCondition == 7; }

		bool hasAnyBattleCondition() const {
			return isBurned() || isAsleep() || isParalyzed() ||
				isPoisoned() || isFreezed() || isConfused() ||
				isAttracted();
		}

		void receiveDamage(int damage) {
			if (currentHP > 0) {
				currentHP -= damage;
			}
		}
};

#endif // POKEMON