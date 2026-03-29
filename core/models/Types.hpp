#include <map>
enum class Types : int {
	NONE = 0,
	FIRE = 1,
	GRASS = 2,
	WATER = 3,
	NORMAL = 4,
	ELECTRIC = 5,
	DARK = 6,
	POISON = 7,
	FLYING = 8,
	FIGHTING = 9,
	ICE = 10,
	PSYCHIC = 11,
	GROUND = 12,
	ROCK = 13,
	STEEL = 14,
	BUG = 15,
	DRAGON = 16,
	GHOST = 17,
	FAIRY = 18
};

inline std::map<std::string, Types> TypesMap = {
	{"fire",     Types::FIRE},
	{"grass",    Types::GRASS},
	{"water",    Types::WATER},
	{"normal",     Types::NORMAL},
	{"electric",    Types::ELECTRIC},
	{"dark",    Types::DARK},
	{"poison",     Types::POISON},
	{"flying",    Types::FLYING},
	{"fighting",    Types::FIGHTING},
	{"ice",     Types::ICE},
	{"psychic",    Types::PSYCHIC},
	{"ground",    Types::GROUND},
	{"rock",     Types::ROCK},
	{"steel",    Types::STEEL},
	{"bug",    Types::BUG},
	{"dragon",    Types::DRAGON},
	{"ghost",     Types::GHOST},
	{"fairy",    Types::FAIRY}
};

inline Types typeFromString(const std::string& name) {
	auto it = TypesMap.find(name);
	if (it != TypesMap.end()) return it->second;
	return Types::NORMAL;
}