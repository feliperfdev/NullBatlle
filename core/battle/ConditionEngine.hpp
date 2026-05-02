#ifndef CONDITION_ENGINE
#define CONDITION_ENGINE

#include <string>
#include <array>
#include <optional>
#include <iostream>
#include <random>
#include <cmath>
#include "../models/Pokemon.hpp"
#include "../models/Move.hpp"
#include "../models/Item.hpp"
#include "../models/Player.hpp"

class ConditionEngine {
	public:
		bool conditionAllowsToAction(Pokemon& pokemon);
		void checkPostActionBattleCondition(Pokemon& pokemon);

		void applyUsedMoveConditionIfApplicable(Pokemon& pokemon, Move& move);

	private:
		std::mt19937 m_rng;

		int generateRandom(int max);
		void log(const std::string& text);
};

#endif // CONDITION_ENGINE