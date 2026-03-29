#include "NullBattle.h"
#include <DataLoader.hpp>

void printMoves(Pokemon& pokemon) {
	std::array<Move, 4> pokemonMoves = pokemon.moves;

	int moveIndex = 0;
	for (auto& move : pokemonMoves) {
		moveIndex++;
		std::cout << "[" + std::to_string(moveIndex) + "] " + move.name + " - " + "(" + std::to_string(move.pp.front()) + "/" + std::to_string(move.pp.front()) + ") PP" << std::endl;
	}
}

void runPlayersActions(BattleStateMachine& engine) {

	Pokemon& p1Pokemon = engine.p1ActivePokemon();
	Pokemon& p2Pokemon = engine.p2ActivePokemon();

	int p1Index, p2Index = 0;

	std::cout << "[Player 1] > Faça uma ação: ";

	int p1Action;

	std::cin >> p1Action;

	if (ActionTypeMap[p1Action] == ActionType::USE_MOVE) { 
		printMoves(p1Pokemon);

		std::cout << "[Player 1] > Escolha um golpe: ";
		std::cin >> p1Index;
	}


	std::cout << "[Player 2] > Faça uma ação: ";

	int p2Action;

	std::cin >> p2Action;

	if (ActionTypeMap[p2Action] == ActionType::USE_MOVE) { 
		printMoves(p2Pokemon);

		std::cout << "[Player 2] > Escolha um golpe: ";
		std::cin >> p2Index;
	}

	if (p1Index <= 0) { p1Index = 1; } if (p2Index <= 0) { p2Index = 1; }

	engine.player1Action(BattleAction{ ActionTypeMap[p1Action], p1Index - 1 });
	engine.player2Action(BattleAction{ ActionTypeMap[p2Action], p2Index - 1 });

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
			{ bulbasaur }, { charmander }
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
					log("Player " + std::to_string(battleEngine.winnerPlayer.id) + " won the battle!");

					break;
			}
		}
	}
	catch (const std::exception& e) {
		std::cerr << "Erro: " << e.what() << std::endl;
	}

	return 0;
}

void log(std::string text) {
	std::cout << "[CoreEngine] " + text << std::endl;
}