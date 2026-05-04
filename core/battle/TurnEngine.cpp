#include "TurnEngine.hpp"
#include <random>
#include <iostream>

TurnEngine::TurnEngine(
	BattleAction p1Action,
	BattleAction p2Action,
	LogQueue& logQueue
) : p1Action(std::move(p1Action)), p2Action(std::move(p2Action)), logQueue(logQueue),
    m_rng(std::random_device{}()), conditionEngine(ConditionEngine(logQueue))
{
	log("Started Turn Engine!");
}

int TurnEngine::generateRandom(int max) {
	std::uniform_int_distribution<int> dist(1, max);
	return dist(m_rng);
}

int TurnEngine::determineOrder(const Pokemon& p1Pokemon, const Pokemon& p2Pokemon) {
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
		std::uniform_int_distribution<int> coinFlip(1, 2);
		whoActsFirst = coinFlip(m_rng);
		return whoActsFirst;
	}

	return (p1Speed > p2Speed) ? 1 : 2;
}

void TurnEngine::executeMoveAction(
	Pokemon& attacker,
	Pokemon& defender,
	Move& move
) {
	if (move.canUseMove()) {

		if (!conditionEngine.conditionAllowsToAction(attacker)) return;

		unsigned int roll = generateRandom(100);

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
					
					conditionEngine.applyUsedMoveConditionIfApplicable(defender, move);
				}
			}
		}
		// Errou o move
		else {
			log("Pokémon " + attacker.name + " attack's missed!");

			return;
		}

		conditionEngine.checkPostActionBattleCondition(attacker);
	}
}

void TurnEngine::executeItemAction(
	Pokemon& itemReceiver,
	Item& item
) {
	if (item.isAHealHpItem()) {
		item.healPokemonHP(itemReceiver);
	} else if (item.hasAConditionHealEffect()) {
		item.healPokemonCondition(itemReceiver);
	}
}

void TurnEngine::switchActivePokemon(Player& player, int pokemonIndex) {
	player.team.switchActivePokemon(pokemonIndex);
}

void TurnEngine::log(const std::string& text) {
	logQueue.log("TurnEngine", text);
}
