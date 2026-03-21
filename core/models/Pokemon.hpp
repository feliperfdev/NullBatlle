#pragma once

#include "Move.hpp"

struct Pokemon
{
	std::string name;
	int currentHP;
	
	// 0 = maxHP, 1 = Atk, 2 = Sp.Atk, 3 = Def, 4 = Sp.Def, 5 = Spe.
	std::array<int, 6> stats;

	// 0 = None
	// 1 = Burned
	// 2 = Sleep
	// 3 = Paralyzed
	// 4 = Poisoned
	// 5 = Freezed
	// 6 = Confusion
	// 7 = Attracted
	int battleCondition;

	std::array<Move, 4> moves;

	public:
		int maxHP() const { return stats[0]; }
		int attack() const { return stats[1]; }
		int spAttack() const { return stats[2]; }
		int defense() const { return stats[3]; }
		int sdDefense() const { return stats[4]; }
		int speed() const { return stats[5]; }

		bool isNotDefeated() const { return currentHP > 0; }

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
};