#include "TurnEngine.hpp"

TurnEngine::TurnEngine(
	BattleAction& p1Action,
	BattleAction& p2Action
) {
	this->p1Action = p1Action;
	this->p2Action = p2Action;
	log("Started Turn Engine!");
}

int TurnEngine::determineOrder(Pokemon& p1Pokemon, Pokemon& p2Pokemon) {
	log("Determining battle order...");
	int whoActsFirst = 0;

	// Trocar de Pokémon é prioridade SEMPRE

	if (p1Action.type == ActionType::USE_MOVE && p2Action.type == ActionType::CHOOSE_POKEMON) {
		whoActsFirst = 2;
	}
	else if (p2Action.type == ActionType::USE_MOVE && p1Action.type == ActionType::CHOOSE_POKEMON) {
		whoActsFirst = 1;
	}

	if (whoActsFirst != 0) return whoActsFirst;

	// Se os dois usaram movimentos

	if (p1Action.type == ActionType::USE_MOVE && p2Action.type == ActionType::USE_MOVE) {
		Move p1UsedMove = p1Pokemon.moves.at(p1Action.index);
		Move p2UsedMove = p2Pokemon.moves.at(p2Action.index);

		if (p1UsedMove.priority > p2UsedMove.priority) { priority = 1; }
		else if (p1UsedMove.priority < p2UsedMove.priority) { priority = 2; }
		else { priority = 0; }

		whoActsFirst = priority;

		if (whoActsFirst != 0) return whoActsFirst;
		
		// Caso nenhum tenha prioridade por move:

		int p1Speed = p1Pokemon.speed();
		int p2Speed = p2Pokemon.speed();

		if (p1Speed == p2Speed) {
			whoActsFirst = (rand() % 2) + 1;
		}
		else {
			if (p1Speed > p2Speed) { whoActsFirst = 1; } else { whoActsFirst = 2; }
		}
	}

	return whoActsFirst;
}

void TurnEngine::executeMoveAction(
	Pokemon& attacker,
	Pokemon& defender,
	Move move
) {
	if (move.isDamageMove()) {
		unsigned int attackerAtkPoints = move.power.value();

		unsigned int dano = (((2 * attacker.level / 5 + 2) * move.power.value() * attacker.attack() / defender.defense()) / 50 + 2);

		if (defender.currentHP - attackerAtkPoints <= 0) {
			defender = Pokemon{ defender.name, defender.level, 0, defender.types, defender.stats, defender.battleCondition, defender.moves };
		}
		else {
			unsigned int defenderLoseHP = defender.currentHP - attackerAtkPoints;

			defender = Pokemon{ defender.name, defender.level, defenderLoseHP, defender.types, defender.stats, defender.battleCondition, defender.moves };
		}
	}
}

void TurnEngine::log(std::string text) {
	std::cout << "[TurnEngine] " + text << std::endl;
}