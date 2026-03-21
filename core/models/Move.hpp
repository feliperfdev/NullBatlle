#pragma once

#include "Types.hpp"
#include <string>
#include <optional>

struct Move
{
	// Array {currentPP, maxPP}
	std::array<int, 2> pp; // Power points

	std::optional<int> power;

	unsigned int priority;

	// 0 = Default; 1 = Físico ; 2 = Especial
	unsigned int category;

	Types type;

	std::string name;

	public:
		bool isPhysicalMove() const { return category == 1; }
		bool isSpecialMove() const { return category == 2; }

		bool isDamageMove() const { return power.has_value(); }
};