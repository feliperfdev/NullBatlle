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
		std::vector<Item> openBag() { return bag.items; }
};

#endif // PLAYER