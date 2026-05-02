#include "BattleStateMachine.hpp"

int BattleStateMachine::getTotalTurns() const { return totalTurns; }

BattleStateMachine::BattleStateMachine(
	Player& player1, Player& player2
) : currentState(BattleState::TEAM_SELECT), winnerId(0), player1(player1), player2(player2) {
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

void BattleStateMachine::executeTurnActions() {
	if (!checkIfP1HasAction() || !checkIfP2HasAction()) return;
	totalTurns++;

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
		turnEngine.executeMoveAction(p1Poke, p2Poke, selectedMoveP1);
		turnEngine.executeMoveAction(p2Poke, p1Poke, selectedMoveP2);
	}
	else {
		turnEngine.executeMoveAction(p2Poke, p1Poke, selectedMoveP2);
		turnEngine.executeMoveAction(p1Poke, p2Poke, selectedMoveP1);
	}

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
	if (player.team.inBattle().isDefeated()) {
		if (player.team.hasAlivePokemon()) {
			return true;
		}
	}

	return false;
}

bool BattleStateMachine::isOver(const std::array<Pokemon, 6>& playerTeam) const {
	for (const auto& p : playerTeam) {
		if (p.isNotDefeated()) return false;
	}
	return true;
}

void BattleStateMachine::log(const std::string& text) {
	std::cout << "[" + BattleStateMap[getState()] + "] > ";
	std::cout << text << std::endl;
}

Pokemon& BattleStateMachine::p1ActivePokemon() { return player1.team.inBattle(); }
Pokemon& BattleStateMachine::p2ActivePokemon() { return player2.team.inBattle(); }