#include <map>
#include <array>
#include <string>
#include <fstream>
#include <utility>
#include <vector>
#include <iostream>
#include <nlohmann/json.hpp>
#include "../core/models/Move.hpp"
#include "../core/models/Pokemon.hpp"
#include <models/PokemonTemplate.hpp>
#include "../common/LogQueue.cpp"

using json = nlohmann::json;

class DataLoader {
	public:
		DataLoader(
			LogQueue& logQueue
		);

		std::map<std::string, Move> loadMoves();
		std::vector<PokemonTemplate> loadPokemon();

	private:
		LogQueue& logQueue;

		void log(const std::string& text);
};