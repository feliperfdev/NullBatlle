#include "TurnEngine.hpp"
#include <random>
#include <iostream>

TurnEngine::TurnEngine(
	BattleAction& p1Action,
	BattleAction& p2Action
) {
	this->p1Action = p1Action;
	this->p2Action = p2Action;
	log("Started Turn Engine!");
}

int generateRandom(int max) {
	std::random_device seed;
	std::mt19937 gen{ seed() };

	std::uniform_int_distribution<int> dist(1, max);

	int random_num = dist(gen);

	return random_num;
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
	Move& move
) {
	if (move.canUseMove()) {

		int roll = generateRandom(100);

		bool moveHits = (roll <= move.accuracy) || (move.accuracy == 0);

		if (moveHits) {
			move.useMove();

			log("Pokémon " + attacker.name + " used " + move.name + "!");

			if (move.isDamageMove()) {
				unsigned int attackerAtkPoints = move.power.value();

				unsigned int dano = (((2 * attacker.level / 5 + 2) * attackerAtkPoints * attacker.attack() / defender.defense()) / 50 + 2);

				if (dano > defender.currentHP) {
					defender.currentHP = 0;

					log("Pokémon " + defender.name + " will receive " + std::to_string(dano) + " HP of damage!");
					log("Pokémon " + defender.name + " was defeated!");
				}
				else {
					defender.receiveDamage(dano);

					log("Pokémon " + defender.name + " will receive " + std::to_string(dano) + " HP of damage!");
				}
			}
		}
		// Errou o move
		else {
			log("Pokémon " + attacker.name + " attack's missed!");
			return;
		}
	}
}

void TurnEngine::executeItemAction(
	Pokemon& itemReceiver,
	Item& item
) {

}

void TurnEngine::log(std::string text) {
	std::cout << "[TurnEngine] " + text << std::endl;
}