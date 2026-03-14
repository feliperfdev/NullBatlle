#include "Move.hpp"

struct Pokemon
{
	int currentHP;
	
	// 0 = maxHP, 1 = Atk, 2 = Sp.Atk, 3 = Def, 4 = Sp.Def, 5 = Spe.
	std::array<int, 6> stats;

	// 1 = Burned
	// 2 = Sleep
	// 3 = Paralyzed
	// 4 = Poisoned
	// 5 = Freezed
	// 6 = Confusion
	// 7 = Attracted
	int battleCondition;

	std::array<Move, 4> moves;
};