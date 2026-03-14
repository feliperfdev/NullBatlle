#include "NullBattle.h"

int main()
{
	
	std::array<Pokemon, 6> myTeam;
	std::array<Pokemon, 6> opositeTeam;

	BattleStateMachine battleEngine = BattleStateMachine(
		myTeam, opositeTeam
	);

	while (battleEngine.getWinner() == 0) {

	}

	return 0;
}
