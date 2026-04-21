#ifndef TURN_ENGINE
#define TURN_ENGINE

#include <string>
#include <array>
#include <optional>
#include <iostream>
#include <random>
#include "BattleAction.hpp"
#include "../models/Pokemon.hpp"
#include "../models/Move.hpp"
#include "../models/Item.hpp"
#include "../models/Player.hpp"

class TurnEngine {
	public:
		int priority;

		TurnEngine(
			BattleAction p1Action,
			BattleAction p2Action
		);

		int determineOrder(
			const Pokemon& p1Pokemon,
			const Pokemon& p2Pokemon
		);

		void executeMoveAction(
			Pokemon& attacker,
			Pokemon& defender,
			Move& move
		);

		void executeItemAction(
			Pokemon& itemReceiver,
			Item& item
		);

		void switchActivePokemon(Player& player, int pokemonIndex);

	private:
		BattleAction p1Action;
		BattleAction p2Action;
		std::mt19937 m_rng;
		int generateRandom(int max);
		void log(const std::string& text);
};

#endif // TURN_ENGINE
