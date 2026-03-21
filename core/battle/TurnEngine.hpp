#ifndef TURN_ENGINE
#define TURN_ENGINE

#include <string>
#include <array>
#include <optional>
#include <iostream>
#include "BattleAction.hpp"
#include "../models/Pokemon.hpp"
#include "../models/Move.hpp"
#include "../models/Item.hpp"

class TurnEngine {
	public:
		int priority;

		TurnEngine(
			BattleAction& p1Action,
			BattleAction& p2Action
		);

		int determineOrder(
			Pokemon& p1Pokemon,
			Pokemon& p2Pokemon
		);

		void executeMoveAction(
			Pokemon& attacker,
			Pokemon& defender,
			Move move
		);

		void executeItemAction(
			Pokemon& itemReceiver,
			Item item
		);

	private:
		BattleAction p1Action;
		BattleAction p2Action;
		void log(std::string text);
};

#endif // TURN_ENGINE