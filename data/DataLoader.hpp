#include <map>
#include <array>
#include <string>
#include <fstream>
#include <utility>
#include <vector>
#include <nlohmann/json.hpp>
#include "../core/models/Move.hpp"
#include "../core/models/Pokemon.hpp"
#include <models/PokemonTemplate.hpp>

using json = nlohmann::json;

class DataLoader {

	public:
		std::vector<Move> loadMoves();
		std::vector<PokemonTemplate> loadPokemon();
};