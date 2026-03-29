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

	unsigned int accuracy;

	// 0 = Default; 1 = Físico ; 2 = Especial
	unsigned int category;

	Types type;

	std::string name;

	public:
		bool isPhysicalMove() const { return category == 1; }
		bool isSpecialMove() const { return category == 2; }

		bool isDamageMove() const { return power.has_value(); }

		bool canUseMove() const { return pp.at(0) > 0; }

		void useMove() {
			int usedPP = pp.at(0);
			int maxPP = pp.at(1);

			if (usedPP == 0) { pp = { 0, maxPP }; }
			else { pp = { usedPP - 1, maxPP }; }
		}
};