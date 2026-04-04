#ifndef BATTLE_STATE_MACHINE
#define BATTLE_STATE_MACHINE

#include <string>
#include <array>
#include <optional>
#include <iostream>
#include "../models/Player.hpp"
#include "../models/Pokemon.hpp"
#include "../models/Team.hpp"
#include "../../common/pokemon_print_helpers.hpp"
#include "BattleAction.hpp"
#include "BattleState.hpp"
#include "TurnEngine.hpp"

class BattleStateMachine {
	public:
		BattleState getState() const;

		BattleStateMachine(
			Player& player1, Player& player2);

		int getTotalTurns();

		void player1Action(BattleAction action);
		void player2Action(BattleAction action);

		void startNewTurn();
		void startExecutingTurn();
		void executeTurnActions();

		bool gameHasWinner();
		bool battleEnded();

		Pokemon& p1ActivePokemon();
		Pokemon& p2ActivePokemon();

		Player winnerPlayer;

		unsigned int whoWillSwitchPokemon = 0;

	private:
		Player& player1;
		Player& player2;

		int totalTurns = 0;

		std::optional<BattleAction> p1Action;
		std::optional<BattleAction> p2Action;

		bool isOver(const std::array<Pokemon, 6>& playerTeam) const;

		bool checkIfP1HasAction();
		bool checkIfP2HasAction();

		int winnerId;

		void checkWinner();

		int getWinner();

		BattleState currentState;

		bool playerNeedsToSwitch(Player& player);

		void log(std::string text);
};

#endif // BATTLE_STATE_MACHINE