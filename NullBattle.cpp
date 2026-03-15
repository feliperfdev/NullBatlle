#include "NullBattle.h"

int main()
{
	BattleStateMachine battleEngine = BattleStateMachine(
		{
			Pokemon {
				"Charmander",
				20,
				{ 20, 0, 0, 0, 0, 0 },
				0,
				{ Move{{15, 15}, 7, 1, Types::NORMAL, "Tackle"} }
			}
		},

		{
			Pokemon{
				"Bulbasaur",
				20,
				{ 20, 0, 0, 0, 0, 0 },
				0,
				{ Move{{20, 20}, 6, 1, Types::NORMAL, "Scratch"} }
			}
		}
	);

	while (battleEngine.getWinner() == 0) {

	}

	if (battleEngine.getState() == BattleState::BATTLE_END) {

	}


	return 0;
}
