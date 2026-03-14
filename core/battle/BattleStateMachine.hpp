#ifndef BATTLE_STATE_MACHINE
#define BATTLE_STATE_MACHINE

#include <string>
#include <array>
#include <optional>
#include "../models/Player.hpp"
#include "../models/Pokemon.hpp"
#include "../models/Team.hpp"
#include "BattleAction.hpp"
#include "BattleState.hpp"

class BattleStateMachine {
	public:
		BattleState getState() const;

		BattleStateMachine(
			const std::array<Pokemon, 6>& myTeam,
			const std::array<Pokemon, 6>& opositeTeam);

		void player1Action(BattleAction action);
		void player2Action(BattleAction action);

		bool isOver(const std::array<Pokemon, 6>& playerTeam) const;

		int getWinner();

	private:
		Player player1;
		Player player2;

		int winnerId;

		BattleState currentState;

		std::array<bool, 6> getDefeatedPokemon(const std::array<Pokemon, 6>& party);

		void getActivePokemon(Team& playerTeam);
};

#endif // BATTLE_STATE_MACHINE