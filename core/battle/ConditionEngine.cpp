#include "ConditionEngine.hpp"

int ConditionEngine::generateRandom(int max) {
	std::uniform_int_distribution<int> dist(1, max);
	return dist(m_rng);
}

bool ConditionEngine::conditionAllowsToAction(Pokemon& pokemon) {
	bool allows = true;

	if (!pokemon.hasAnyBattleCondition()) return allows;

	std::string conditionText;

	switch (pokemon.battleCondition) {
		case BattleCondition::PARALYZED:
			allows = (1 / generateRandom(4)) <= 25;
			conditionText = "paralyzed";

			break;

		case BattleCondition::FREEZED:
			allows = (1 / generateRandom(5)) <= 20;
			conditionText = "freezed";

			break;

		case BattleCondition::ASLEEP:
			allows = (1 / generateRandom(3)) <= 33.3;
			conditionText = "asleep";

			break;

		case BattleCondition::ATTRACTED:
			allows = (1 / generateRandom(2)) <= 50;
			conditionText = "in love";

			break;

		case BattleCondition::CONFUSED:
			allows = (1 / generateRandom(3)) <= 33.3;
			conditionText = "confused";

			if (!allows) {
				int hurtItselfDamage =
					static_cast<int>(std::round((((((
						2 * pokemon.level) / 5)
						* pokemon.attack() * 40)
						/ pokemon.defense()) / 50) + 2));

				log(pokemon.name + " is confused. It'll receive " +
					std::to_string(hurtItselfDamage) + " HP of damage!");

				pokemon.receiveDamage(hurtItselfDamage);
			}

			break;

		default:
			allows = true;

			break;
	}

	if (!allows) {
		log(pokemon.name + " is " + conditionText + ". It won't let it use a move!");
	}

	return allows;
}

void ConditionEngine::checkPostActionBattleCondition(Pokemon& pokemon) {
	if (!pokemon.hasAnyBattleCondition()) return;

	switch (pokemon.battleCondition) {
		case BattleCondition::POISONED:

			pokemon.receiveDamage(
				static_cast<int>(std::round(pokemon.maxHP() * 0.125)));

			break;

		case BattleCondition::BURNED:

			pokemon.receiveDamage(
				static_cast<int>(std::round(pokemon.maxHP() * 0.0625)));

			break;
	}
}

void ConditionEngine::applyUsedMoveConditionIfApplicable(Pokemon& pokemon, Move& move) {
	switch (move.type) {
		case Types::FIRE:
			double chance = 0.0;

			if (
				move.name == "flamethrower" &&
				move.name == "fire-punch" &&
				move.name == "fire-blast" &&
				move.name == "fire-fang" &&
				move.name == "ember"
				) {
				chance = 0.1;
			}

			if (
				move.name == "scald" &&
				move.name == "lava-plume" &&
				move.name == "steam-eruption" &&
				move.name == "ice-burn" &&
				move.name == "searing-shot"
				) {
				chance = 0.3;
			}

			if (
				move.name == "sacred-fire"
				) {
				chance = 0.5;
			}

			if (
				move.name == "will-o-wisp"
				) {
				chance = 1;
			}

			if (chance == 0) return;

			int seed = chance == 1 ? 1 : generateRandom(100);

			if ((1 / seed) <= chance) {
				pokemon.applyCondition(BattleCondition::BURNED);
			}

			break;
	}
}

void ConditionEngine::log(const std::string& text) {
	std::cout << "[ConditionEngine] " + text << std::endl;
}