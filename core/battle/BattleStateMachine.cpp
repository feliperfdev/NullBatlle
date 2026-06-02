#include "BattleStateMachine.hpp"

int BattleStateMachine::getTotalTurns() const { return totalTurns; }

Player& BattleStateMachine::getPlayerById(int id) {
	return id == 1 ? player1 : player2;
}

BattleStateMachine::BattleStateMachine(
	Player& player1, Player& player2, LogQueue& logQueue
) : currentState(BattleState::TEAM_SELECT), winnerId(0), 
	player1(player1), player2(player2), logQueue(logQueue) {
	
	log("Initialized Player 1 team: " + printPokemonData(player1.team.party[0]));

	log("Initialized Player 2 team: " + printPokemonData(player2.team.party[0]));

	currentState = BattleState::ACTION_TURN;
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

int BattleStateMachine::getWinner() const { return winnerId; }

bool BattleStateMachine::gameHasWinner() const { return getWinner() != 0; }

bool BattleStateMachine::battleEnded() const { return currentState == BattleState::BATTLE_END; }

void BattleStateMachine::player1Action(BattleAction action) {
	p1Action = action;
}

void BattleStateMachine::player2Action(BattleAction action) {
	p2Action = action;
}

bool BattleStateMachine::checkIfP1HasAction() { return p1Action.has_value(); }
bool BattleStateMachine::checkIfP2HasAction() { return p2Action.has_value(); }

void BattleStateMachine::startExecutingTurn() { currentState = BattleState::ACTION_EXECUTING_TURN; }

void BattleStateMachine::startNewTurn() { currentState = BattleState::ACTION_TURN; }

void BattleStateMachine::player1ActionExecution(TurnEngine& turnEngine) {
	Pokemon& p1Poke = p1ActivePokemon();
	Pokemon& p2Poke = p2ActivePokemon();

	ActionType p1ActionType = p1Action.value().type;
	int p1Index = p1Action.value().index;

	if (p1ActionType == ActionType::USE_MOVE) {
		Move& selectedMove = p1Poke.moves.at(p1Index);
		turnEngine.executeMoveAction(p1Poke, p2Poke, selectedMove);
	}
	else if (p1ActionType == ActionType::CHOOSE_POKEMON) {
		whoWillSwitchPokemon = player1.id;
		currentState = BattleState::SWITCH_IN;
		turnEngine.switchActivePokemon(player1, p1Index);
	}
	else if (p1ActionType == ActionType::USE_ITEM) {
		Item& selectedItem = player1.bag.items.at(p1Index);
		turnEngine.executeItemAction(p1Poke, selectedItem);
	}
}

void BattleStateMachine::player2ActionExecution(TurnEngine& turnEngine) {
	Pokemon& p1Poke = p1ActivePokemon();
	Pokemon& p2Poke = p2ActivePokemon();

	ActionType p2ActionType = p2Action.value().type;
	int p2Index = p2Action.value().index;

	if (p2ActionType == ActionType::USE_MOVE) {
		Move& selectedMove = p2Poke.moves.at(p2Index);
		turnEngine.executeMoveAction(p2Poke, p1Poke, selectedMove);
	}
	else if (p2ActionType == ActionType::CHOOSE_POKEMON) {
		whoWillSwitchPokemon = player2.id;
		currentState = BattleState::SWITCH_IN;
		turnEngine.switchActivePokemon(player2, p2Index);
	}
	else if (p2ActionType == ActionType::USE_ITEM) {
		Item& selectedItem = player2.bag.items.at(p2Index);
		turnEngine.executeItemAction(p2Poke, selectedItem);
	}
}

void BattleStateMachine::takeDecision(int order, TurnEngine& turnEngine) {
	// Player 1 goes first
	if (order == 1) {
		player1ActionExecution(turnEngine);
		player2ActionExecution(turnEngine);
	}

	// Player 2 goes first
	else {
		player2ActionExecution(turnEngine);
		player1ActionExecution(turnEngine);
	}
}

void BattleStateMachine::executeTurnActions() {
	if (!checkIfP1HasAction() || !checkIfP2HasAction()) return;
	totalTurns++;

	TurnEngine turnEngine = TurnEngine(p1Action.value(), p2Action.value(), logQueue);

	Pokemon& p1Poke = p1ActivePokemon();
	Pokemon& p2Poke = p2ActivePokemon();

	int order = turnEngine.determineOrder(p1Poke, p2Poke);

	takeDecision(order, turnEngine);

	log(printPokemonData(p1Poke));
	log(printPokemonData(p2Poke));

	checkWinner();

	if (!gameHasWinner()) {
		bool p1NeedsToSwitch = playerNeedsToSwitch(player1);
		bool p2NeedsToSwitch = playerNeedsToSwitch(player2);

		whoWillSwitchPokemon = p1NeedsToSwitch ? 1 : (p2NeedsToSwitch ? 2 : 0);

		if (whoWillSwitchPokemon > 0) {
			currentState = BattleState::SWITCH_AFTER_FAINT;
		}
		else {
			currentState = BattleState::ACTION_TURN;
		}
	}
}

bool BattleStateMachine::playerNeedsToSwitch(Player& player) {
	return player.team.inBattle().isDefeated() && player.team.hasAlivePokemon();
}

bool BattleStateMachine::isOver(const std::array<Pokemon, 6>& playerTeam) const {
	for (const auto& p : playerTeam) {
		if (p.isNotDefeated()) return false;
	}
	return true;
}

void BattleStateMachine::log(const std::string& text) {
	logQueue.log("BattleStateMachine", text);
}

Pokemon& BattleStateMachine::p1ActivePokemon() { return player1.team.inBattle(); }
Pokemon& BattleStateMachine::p2ActivePokemon() { return player2.team.inBattle(); }