#pragma once

#include "Types.hpp"
#include <string>
#include <optional>

enum class MoveCategory : unsigned int {
	DEFAULT  = 0,
	PHYSICAL = 1,
	SPECIAL  = 2
};

struct Move
{
	// Array {currentPP, maxPP}
	std::array<int, 2> pp; // Power points

	std::optional<int> power;

	unsigned int priority;

	unsigned int accuracy;

	MoveCategory category;

	Types type;

	std::string name;

	public:
		bool isPhysicalMove() const { return category == MoveCategory::PHYSICAL; }
		bool isSpecialMove() const { return category == MoveCategory::SPECIAL; }

		bool isDamageMove() const { return power.has_value(); }

		bool canUseMove() const { return pp.at(0) > 0; }

		void useMove() {
			int usedPP = pp.at(0);
			int maxPP = pp.at(1);

			if (usedPP == 0) { pp = { 0, maxPP }; }
			else { pp = { usedPP - 1, maxPP }; }
		}
};
