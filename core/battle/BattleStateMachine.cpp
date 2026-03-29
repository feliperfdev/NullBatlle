#include "BattleStateMachine.hpp"

std::string printPokemonData(const Pokemon& pokemon) {
	std::string hpBar = "";
	int barLength = 20;
	float hpRatio = (float)pokemon.currentHP / (float)pokemon.maxHP();
	int filled = (int)(hpRatio * barLength);

	for (int i = 0; i < barLength; i++) {
		hpBar += (i < filled) ? "=" : "-";
	}

	std::string condition = "OK";
	if (pokemon.isBurned())    condition = "BRN";
	if (pokemon.isPoisoned())  condition = "PSN";
	if (pokemon.isParalyzed()) condition = "PAR";
	if (pokemon.isAsleep())    condition = "SLP";
	if (pokemon.isFreezed())   condition = "FRZ";

	std::string result = "";
	result += "\n[-----------------------------------]\n";
	result += "| " + pokemon.name + " (Lv." + std::to_string(pokemon.level) + ")  [" + condition + "]\n";
	result += "| HP: [" + hpBar + "] " + std::to_string(pokemon.currentHP) + "/" + std::to_string(pokemon.maxHP()) + "\n";
	result += "| ATK:" + std::to_string(pokemon.attack()) +
		"  DEF:" + std::to_string(pokemon.defense()) +
		"  SPE:" + std::to_string(pokemon.speed()) + "\n";
	result += "| Moves: ";
	for (auto& m : pokemon.moves) {
		result += m.name + "(" + std::to_string(m.pp[0]) + "/" + std::to_string(m.pp[1]) + ") ";
	}
	result += "\n";
	result += "[-----------------------------------]";

	return result;
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
		winnerPlayer = player2;
		currentState = BattleState::BATTLE_END;
	}
	else if (isOver(player2.team.party)) {
		winnerId = player1.id;
		winnerPlayer = player1;
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

	Pokemon& p1Poke = p1ActivePokemon();
	Pokemon& p2Poke = p2ActivePokemon();

	int order = turnEngine.determineOrder(
		p1Poke,
		p2Poke
	);

	Move& selectedMoveP1 = p1Poke.moves.at(p1Action.value().index);
	Move& selectedMoveP2 = p2Poke.moves.at(p2Action.value().index);

	if (order == 1) {
		log(p1Poke.name + " vai primeiro!");
		turnEngine.executeMoveAction(p1Poke, p2Poke, selectedMoveP1);
		turnEngine.executeMoveAction(p2Poke, p1Poke, selectedMoveP2);
	}
	else {
		log(p2Poke.name + " vai primeiro!");
		turnEngine.executeMoveAction(p2Poke, p1Poke, selectedMoveP2);
		turnEngine.executeMoveAction(p1Poke, p2Poke, selectedMoveP1);
	}

	currentState = BattleState::END_TURN;

	log(printPokemonData(p1Poke));
	log(printPokemonData(p2Poke));

	checkWinner();

	if (!gameHasWinner()) {
		currentState = BattleState::ACTION_TURN;
	}
}

bool BattleStateMachine::isOver(const std::array<Pokemon, 6>& playerTeam) const {
	int count = 0;
	for (auto& p : playerTeam) { if (!p.isNotDefeated()) { count++; } }

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