#include "Types.hpp"
#include <string>
#include <optional>

struct Move
{
	// Array {currentPP, maxPP}
	std::array<int, 2> pp; // Power points

	std::optional<int> power;

	// 0 = Default; 1 = Físico ; 2 = Especial
	unsigned int category;

	Types type;

	std::string name;
};