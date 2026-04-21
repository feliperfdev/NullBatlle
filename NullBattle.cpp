#include "NullBattle.h"
#include <DataLoader.hpp>

void log(const std::string& text) {
	std::cout << "[CoreEngine] " + text << std::endl;
}

void runPlayersActions(BattleStateMachine& engine) {

	Pokemon& p1Pokemon = engine.p1ActivePokemon();
	Pokemon& p2Pokemon = engine.p2ActivePokemon();

	for (unsigned int k = 0; k <= 1; k++) {
		printBattlePanel();
		int actionIndex = 0;

		std::cout << "[Player  " + std::to_string(k+1) + "] > Faça uma ação: ";

		int action;

		std::cin >> action;

		if (ActionTypeMap[action] == ActionType::USE_MOVE) {
			printMovesPanel((k+1) == 1 ? p1Pokemon : p2Pokemon, k + 1);

			std::cout << "[Player  " + std::to_string(k + 1) + "] > Escolha um golpe: ";
			std::cin >> actionIndex;
		}

		if (actionIndex == 0) { actionIndex = 1; }

		if (k + 1 == 1) {
			engine.player1Action(BattleAction{ ActionTypeMap[action], actionIndex - 1 });
		} else { engine.player2Action(BattleAction{ ActionTypeMap[action], actionIndex - 1 }); }
	}

	engine.startExecutingTurn();

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

	for (auto& poke : player.team.party) {
		std::cout << printPokemonData(poke) << std::endl;
	}

	std::cout << "[Player " + std::to_string(player.id) + "] Escolha o index do pokémon do " +
		"seu time para substituir por " + player.team.inBattle().name + ": ";

	std::cin >> pokemonIndex;

	auto chosenPokemon = player.team.party[pokemonIndex];

	log("Pokémon escolhido: " + chosenPokemon.name);

	player.team.switchActivePokemon(pokemonIndex);
}

void saveMatchResult(
	const std::string& sessionId,
	int totalTurns,
	const Player& winnerPlayer
) {
	nlohmann::json result;

	result["sessionId"] = sessionId;
	result["totalTurns"] = totalTurns;
	result["winnerPlayer"] = {
		{"id",       winnerPlayer.id},
		{"partySize", winnerPlayer.team.party.size()}
	};

	std::string filename = "match_" + sessionId + ".json";
	std::ofstream file(filename);

	if (!file.is_open()) {
		throw std::runtime_error("Nao foi possivel salvar o resultado em: " + filename);
	}

	file << result.dump(2);
	file.close();

	log("Resultado salvo em " + filename + "\n");
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

		bool battleEnd = false;

		while (!battleEnd) {

			switch (battleEngine.getState()) {
				case BattleState::ACTION_TURN:
					log("Starting battle...");
					runPlayersActions(battleEngine);

					break;
				case BattleState::SWITCH_AFTER_FAINT:

					forceSwitchActivePokemon(
						battleEngine.whoWillSwitchPokemon == 1 ? player1 : player2);

					battleEngine.startNewTurn();

					break;
				case BattleState::ACTION_EXECUTING_TURN:
					log("Entrando em modo de execução do turno...");
					battleEngine.executeTurnActions();

					break;

				case BattleState::BATTLE_END:
					printVictoryScreen(battleEngine.winnerPlayer, battleEngine.getTotalTurns());
					saveMatchResult(session, battleEngine.getTotalTurns(), battleEngine.winnerPlayer);

					battleEnd = true;

					break;
			}
		}
	}
	catch (const std::exception& e) {
		std::cerr << "Erro: " << e.what() << std::endl;
	}

	return 0;
}