#include "DataLoader.hpp"

std::vector<Move> DataLoader::loadMoves() {
	std::vector<Move> moves = {};

	std::ifstream f("moves.json");
	json data = json::parse(f);

	for (auto& move : data.items()) {
		json object = move.value();

		std::array<int, 2> pp = { object.at("pp"), object.at("pp") };

		std::optional<int> power = std::nullopt;
		if (!object.at("power").is_null()) {
			power = object.at("power").get<int>();
		}

		Move mv = { pp, power, object.at("priority"), object.at("category"), typeFromString(object.at("type")), object.at("name") };

		moves.push_back(mv);
	}

	return moves;
}
std::vector<PokemonTemplate> DataLoader::loadPokemon() {
	std::vector<PokemonTemplate> pokemonTemplate = {};

	std::ifstream f("pokemon.json");
	json data = json::parse(f);

	for (auto& move : data.items()) {
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

	return pokemonTemplate;
}
