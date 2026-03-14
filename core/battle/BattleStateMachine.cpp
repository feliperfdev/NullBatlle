#include "BattleStateMachine.hpp"

BattleStateMachine::BattleStateMachine(
	const std::array<Pokemon, 6>& myTeam,
	const std::array<Pokemon, 6>& opositeTeam
) : currentState(BattleState::TEAM_SELECT), winnerId(0) {
	player1.id = 1;
	player1.team.party = myTeam;
	player1.team.defeated = getDefeatedPokemon(myTeam);
	getActivePokemon(player1.team);

	player2.id = 2;
	player2.team.party = opositeTeam;
	player2.team.defeated = getDefeatedPokemon(opositeTeam);
	getActivePokemon(player2.team);

	currentState = BattleState::ACTION_TURN;
}

BattleState BattleStateMachine::getState() const { return currentState; }

int BattleStateMachine::getWinner() {
	if (isOver(player1.team.party)) {
		winnerId = player2.id;
	}
	else if (isOver(player2.team.party)) {
		winnerId = player1.id;
	}

	return winnerId;
}

void BattleStateMachine::player1Action(BattleAction action) {

}

void BattleStateMachine::player2Action(BattleAction action) {

}

bool BattleStateMachine::isOver(const std::array<Pokemon, 6>& playerTeam) const {
	int count = 0;

	for (auto& p : playerTeam) {
		if (p.currentHP == 0) {
			count++;
		}
	}

	return count == playerTeam.size();
}

std::array<bool, 6> BattleStateMachine::getDefeatedPokemon(const std::array<Pokemon, 6>& party) {
	std::array<bool, 6> defeated = {};

	for (int i = 0; i < party.size(); i++) {
		if (party.at(i).currentHP == 0) {
			defeated[i] = true;
		}
		else {
			defeated[i] = false;
		}
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