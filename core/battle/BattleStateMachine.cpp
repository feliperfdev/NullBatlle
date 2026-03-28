#include "BattleStateMachine.hpp"

std::string printPokemonData(Pokemon pokemon) {
	return "{name: " + pokemon.name + ", level: " + std::to_string(pokemon.level) + ", moves: " + std::to_string(pokemon.moves.size())
		+ ", stats: " + "{" +
		std::to_string(pokemon.stats[0]) + ", " +
		std::to_string(pokemon.stats[1]) + ", " +
		std::to_string(pokemon.stats[2]) + ", " +
		std::to_string(pokemon.stats[3]) + ", " +
		std::to_string(pokemon.stats[4]) + ", " +
		std::to_string(pokemon.stats[5]) + "}}";
}

BattleStateMachine::BattleStateMachine(
	const std::array<Pokemon, 6>& myTeam,
	const std::array<Pokemon, 6>& opositeTeam
) : currentState(BattleState::TEAM_SELECT), winnerId(0) {
	player1.id = 1;
	player1.team.party = myTeam;
	player1.team.defeated = getDefeatedPokemon(myTeam);
	getActivePokemon(player1.team);

	log("Initialized Player 1 team: " + printPokemonData(player1.team.party[0]));

	player2.id = 2;
	player2.team.party = opositeTeam;
	player2.team.defeated = getDefeatedPokemon(opositeTeam);
	getActivePokemon(player2.team);

	log("Initialized Player 2 team: " + printPokemonData(player2.team.party[0]));

	currentState = BattleState::ACTION_TURN;

	log("Starting battle...");
}

BattleState BattleStateMachine::getState() const { return currentState; }

void BattleStateMachine::checkWinner() {
	if (isOver(player1.team.party)) {
		winnerId = player2.id;
		currentState = BattleState::BATTLE_END;
	}
	else if (isOver(player2.team.party)) {
		winnerId = player1.id;
		currentState = BattleState::BATTLE_END;
	}
}

int BattleStateMachine::getWinner() { return winnerId; }

bool BattleStateMachine::gameHasWinner() { return getWinner() != 0; }

void BattleStateMachine::player1Action(BattleAction action) {
	p1Action = action;
	executeTurnActions();
}

void BattleStateMachine::player2Action(BattleAction action) {
	p2Action = action;
	executeTurnActions();
}

bool BattleStateMachine::checkIfP1HasAction() { return p1Action.has_value(); }
bool BattleStateMachine::checkIfP2HasAction() { return p2Action.has_value(); }

void BattleStateMachine::executeTurnActions() {
	if (!checkIfP1HasAction() || !checkIfP2HasAction()) return;

	currentState = BattleState::ACTION_EXECUTING_TURN;

	log("Entrando em modo de execução do turno...");

	TurnEngine turnEngine = TurnEngine(p1Action.value(), p2Action.value());

	int order = turnEngine.determineOrder(
		player1.team.inBattle(),
		player2.team.inBattle()
	);
}

bool BattleStateMachine::isOver(const std::array<Pokemon, 6>& playerTeam) const {
	int count = 0;
	for (auto& p : playerTeam) { if (p.currentHP == 0) { count++; } }

	return count == playerTeam.size();
}

std::array<bool, 6> BattleStateMachine::getDefeatedPokemon(const std::array<Pokemon, 6>& party) {
	std::array<bool, 6> defeated = {};

	for (int i = 0; i < party.size(); i++) {
		defeated[i] = !party.at(i).isNotDefeated();
	}

	return defeated;
}

void BattleStateMachine::getActivePokemon(Team& playerTeam) {
	for (int i = 0; i < playerTeam.defeated.size(); i++) {
		if (!playerTeam.defeated.at(i)) {
			playerTeam.activePokemon = i;
			break;
		}
	}
}

void BattleStateMachine::log(std::string text) {
	std::cout << "[" + BattleStateMap[getState()] + "] > ";
	std::cout << text << std::endl;
}

Pokemon& BattleStateMachine::p1ActivePokemon() { return player1.team.inBattle(); }
Pokemon& BattleStateMachine::p2ActivePokemon() { return player2.team.inBattle(); }