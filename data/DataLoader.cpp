#include "DataLoader.hpp"

std::map<std::string, Move> DataLoader::loadMoves() {
	std::map<std::string, Move> moves = {};

	std::ifstream f(std::string(DATA_PATH) + "moves.json");
	json data = json::parse(f);

	for (auto& move : data.items()) {
		json object = move.value();

		std::array<int, 2> pp = { 0, 0 };
		if (!object.at("pp").is_null()) {
			int ppVal = object.at("pp").get<int>();
			pp = { ppVal, ppVal };
		}

		unsigned int priority = 0;
		if (!object.at("priority").is_null()) {
			int priorVal = object.at("priority").get<int>();
			priority == priorVal;
		}
		unsigned int category = 0;
		if (!object.at("category").is_null()) {
			int catVal = object.at("category").get<int>();
			category == catVal;
		}

		unsigned int accuracy = 0;
		if (!object.at("accuracy").is_null()) {
			int accVal = object.at("accuracy").get<int>();
			accuracy == accVal;
		}


		std::optional<int> power = std::nullopt;
		if (!object.at("power").is_null()) {
			power = object["power"];
		}

		Move mv = { pp, power, priority, accuracy, category, typeFromString(object["type"]), object["name"]};

		moves[object.at("name").get<std::string>()] = mv;
	}

	return moves;
}

std::vector<PokemonTemplate> DataLoader::loadPokemon() {
	std::vector<PokemonTemplate> pokemonTemplate = {};

	std::ifstream f(std::string(DATA_PATH) + "pokemon.json");
	json data = json::parse(f);

	int pkmnCount = 0;

	for (auto& move : data.items()) {
		pkmnCount++;

		json object = move.value();

		std::array<Types, 2> types = {};

		for (int i = 0; i < object.at("types").size(); i++) {
			std::string t = object.at("types").at(i);

			types[i] = typeFromString(t);
		}

		if (object.at("types").size() == 1) {
			types[1] = Types::NONE;
		}

		std::vector<LearnsetEntry> learnset = {};

		for (int i = 0; i < object.at("learnset").size(); i++) {
			json move = object.at("learnset").at(i);

			learnset.push_back(LearnsetEntry{ move.at("move"), move.at("level") });
		}

		std::array<unsigned int, 6> stats;
		for (int i = 0; i < 6; i++) {
			stats[i] = object.at("stats").at(i).get<int>();
		}

		PokemonTemplate pkmn = { object.at("id"), object.at("name"), types, stats, learnset };

		pokemonTemplate.push_back(pkmn);
	}

	log("Successfully loaded " + std::to_string(pkmnCount) + " Pokemon!");

	return pokemonTemplate;
}

void DataLoader::log(std::string text) {
	std::cout << "[DataLoader] " + text << std::endl;
}
