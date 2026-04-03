#include "NullBattle.h"
#include <DataLoader.hpp>

void log(std::string text) {
	std::cout << "[CoreEngine] " + text << std::endl;
}

void printMoves(Pokemon& pokemon) {
	std::array<Move, 4> pokemonMoves = pokemon.moves;

	int moveIndex = 0;
	for (auto& move : pokemonMoves) {
		moveIndex++;
		std::cout << "[" + std::to_string(moveIndex) + "] " + move.name + " - " + "(" + std::to_string(move.pp.at(0)) + "/" + std::to_string(move.pp.at(1)) + ") PP" << std::endl;
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

void initPlayers(Player& player1, Player& player2, 
	std::vector<PokemonTemplate> pokemonList, std::map<std::string, Move> moves
) {
	player1.id = 1;
	player2.id = 2;

	PokemonTemplate pikachuTmp = pokemonList[24];
	PokemonTemplate charmanderTmp = pokemonList[3];
	PokemonTemplate bulbasaurTmp = pokemonList[0];
	PokemonTemplate squirtleTmp = pokemonList[6];

	Pokemon pikachu = pikachuTmp.build(
		5,
		{ "thunder-shock", "growl", "thunder-wave", "tail-whip" },
		moves
	);

	Pokemon bulbasaur = bulbasaurTmp.build(
		5,
		{ "tackle", "growl", "leech-seed", "vine-whip" },
		moves
	);

	Pokemon squirtle = squirtleTmp.build(
		5,
		{ "tackle", "bubble", "withdraw", "tail-whip" },
		moves
	);

	Pokemon charmander = charmanderTmp.build(
		5,
		{ "tackle", "growl", "ember", "flamethrower" },
		moves
	);

	player1.team.party = {pikachu, bulbasaur};
	player2.team.party = {charmander, squirtle};
}

void forceSwitchActivePokemon(Player& player) {
	log("Player " + std::to_string(player.id) + " vai trocar de Pokémon!");

	int pokemonIndex = 0;

	unsigned int partyIndex = 0;

	std::cout << "[Player " + std::to_string(player.id) + "] Escolha o index do pokémon do " +
		"seu time para substituir por " + player.team.inBattle().name + ": ";

	std::cin >> pokemonIndex;

	if (pokemonIndex > player.team.party.size() || pokemonIndex <= 0) {
		forceSwitchActivePokemon(player);
	}

	auto chosenPokemon = player.team.party.at(pokemonIndex);

	log("Pokémon escolhido: " + chosenPokemon.name);

	player.team.switchActivePokemon(pokemonIndex - 1);
}

int main()
{
	setlocale(LC_ALL, "Portuguese");

	try {
		DataLoader dataLoader = DataLoader();

		std::string session = UuidV4().generate();

		log("Starting session " + session + "...");

		auto movesMap = dataLoader.loadMoves();
		auto pokemonList = dataLoader.loadPokemon();

		Player player1, player2;

		initPlayers(player1, player2, pokemonList, movesMap);

		BattleStateMachine battleEngine = BattleStateMachine(
			player1, player2
		);

		while (!battleEngine.gameHasWinner()) {

			switch (battleEngine.getState()) {
				case BattleState::ACTION_TURN:
					log("Starting battle...");
					runPlayersActions(battleEngine);

					break;
				case BattleState::SWITCH_AFTER_FAINT:

					forceSwitchActivePokemon(
						battleEngine.whoWillSwitchPokemon == 1 ? player1 : player2);

					break;
				case BattleState::ACTION_EXECUTING_TURN:
					log("Entrando em modo de execução do turno...");

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