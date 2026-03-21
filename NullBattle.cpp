#include "NullBattle.h"

void runPlayersActions(BattleStateMachine& engine) {

	std::cout << "[1] > Faça uma ação: ";

	int p1Action;

	std::cin >> p1Action;

	engine.player1Action(BattleAction{ ActionTypeMap[p1Action], 0});


	std::cout << "[2] > Faça uma ação: ";

	int p2Action;

	std::cin >> p2Action;

	engine.player2Action(BattleAction{ ActionTypeMap[p2Action], 0 });

	std::cout << std::endl;
}

int main()
{
	setlocale(LC_ALL, "Portuguese");

	BattleStateMachine battleEngine = BattleStateMachine(
		{
			Pokemon {
				"Charmander",
				5,
				20,
				{ Types::FIRE },
				{ 20, 0, 0, 0, 0, 0 },
				0,
				{ Move{{15, 15}, 7, 0, 1, Types::NORMAL, "Tackle"} }
			}
		},

		{
			Pokemon{
				"Bulbasaur",
				5,
				20,
				{ Types::GRASS },
				{ 20, 0, 0, 0, 0, 0 },
				0,
				{ Move{{20, 20}, 6, 0, 1, Types::NORMAL, "Scratch"} }
			}
		}
	);

	while (!battleEngine.gameHasWinner()) {

		switch (battleEngine.getState()) {
			case BattleState::ACTION_TURN:
				runPlayersActions(battleEngine);

				break;
			case BattleState::ACTION_EXECUTING_TURN:
				// ...

				break;

			case BattleState::BATTLE_END:
				// ...

				break;

		}
	}

	return 0;
}
