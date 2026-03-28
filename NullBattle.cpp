#include "NullBattle.h"
#include <DataLoader.hpp>

void runPlayersActions(BattleStateMachine& engine) {

	Pokemon& p1Pokemon = engine.p1ActivePokemon();
	Pokemon& p2Pokemon = engine.p2ActivePokemon();

	std::cout << "[Player 1] > Faça uma ação: ";

	int p1Action;

	std::cin >> p1Action;

	if (ActionTypeMap[p1Action] == ActionType::USE_MOVE) {
		std::array<Move, 4> p1PokemonMoves = p1Pokemon.moves;

		for (auto& move : p1PokemonMoves) {
			std::cout << move.name + " - " + "(" + std::to_string(move.pp.front()) + "/" + std::to_string(move.pp.front()) + ") PP" << std::endl;
		}
	}


	std::cout << "[Player 2] > Faça uma ação: ";

	int p2Action;

	std::cin >> p2Action;

	if (ActionTypeMap[p2Action] == ActionType::USE_MOVE) {
		std::array<Move, 4> p2PokemonMoves = p2Pokemon.moves;

		for (auto& move : p2PokemonMoves) {
			std::cout << move.name + " - " + "(" + std::to_string(move.pp.front()) + "/" + std::to_string(move.pp.front()) + ") PP" << std::endl;
		}
	}

	engine.player1Action(BattleAction{ ActionTypeMap[p1Action], 0 });
	engine.player2Action(BattleAction{ ActionTypeMap[p2Action], 0 });

	std::cout << std::endl;
}

int main()
{
	setlocale(LC_ALL, "Portuguese");

	try {
		DataLoader dataLoader = DataLoader();

		auto movesMap = dataLoader.loadMoves();
		auto pokemonList = dataLoader.loadPokemon();

		PokemonTemplate& tmpl1 = pokemonList[0];
		PokemonTemplate& tmpl2 = pokemonList[3];

		Pokemon bulbasaur = tmpl1.build(
			5,
			{ "tackle", "growl", "vine-whip", "razor-leaf" },
			movesMap
		);

		Pokemon charmander = tmpl2.build(
			5,
			{ "tackle", "growl", "ember", "flamethrower" },
			movesMap
		);


		BattleStateMachine battleEngine = BattleStateMachine(
			{
				bulbasaur
			},

		{
			charmander
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
	}
	catch (const std::exception& e) {
		std::cerr << "Erro: " << e.what() << std::endl;
	}

	return 0;
}
