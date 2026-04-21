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

using json = nlohmann::json;

class DataLoader {

	public:
		std::map<std::string, Move> loadMoves();
		std::vector<PokemonTemplate> loadPokemon();

	private:
		void log(const std::string& text);
};