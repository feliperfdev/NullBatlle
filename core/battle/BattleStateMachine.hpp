#ifndef BATTLE_STATE_MACHINE
#define BATTLE_STATE_MACHINE

#include <string>
#include <array>
#include <optional>
#include <iostream>
#include "../models/Player.hpp"
#include "../models/Pokemon.hpp"
#include "../models/Team.hpp"
#include "BattleAction.hpp"
#include "BattleState.hpp"
#include "TurnEngine.hpp"

class BattleStateMachine {
	public:
		BattleState getState() const;

		BattleStateMachine(
			const std::array<Pokemon, 6>& myTeam,
			const std::array<Pokemon, 6>& opositeTeam);

		void player1Action(BattleAction action);
		void player2Action(BattleAction action);

		bool gameHasWinner();

	private:
		Player player1;
		Player player2;

		std::optional<BattleAction> p1Action;
		std::optional<BattleAction> p2Action;

		bool isOver(const std::array<Pokemon, 6>& playerTeam) const;

		bool checkIfP1HasAction();
		bool checkIfP2HasAction();

		void executeTurnActions();

		int winnerId;

		void checkWinner();

		int getWinner();

		BattleState currentState;

		std::array<bool, 6> getDefeatedPokemon(const std::array<Pokemon, 6>& party);

		void getActivePokemon(Team& playerTeam);

		void log(std::string text);
};

#endif // BATTLE_STATE_MACHINE