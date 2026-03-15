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
		
		if (battleEngine.getState() == BattleState::ACTION_TURN) {
			battleEngine.player1Action(BattleAction{ ActionType::USE_MOVE, 0 });
			battleEngine.player2Action(BattleAction{ ActionType::USE_ITEM, 0 });
		}

		if (battleEngine.getState() == BattleState::ACTION_EXECUTING_TURN) {
			
		}
	}

	if (battleEngine.getState() == BattleState::BATTLE_END) {

	}


	return 0;
}
