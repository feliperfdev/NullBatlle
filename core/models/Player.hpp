#ifndef PLAYER
#define PLAYER

#include "Team.hpp"
#include "Bag.hpp"
#include <vector>

struct Player
{
	int id;
	Team team;
	Bag bag;

	public:
		const std::vector<Item>& openBag() const { return bag.items; }
};

#endif // PLAYER