#include "Item.hpp"
#include <array>

static constexpr int BAG_MAX_SPACE = 100;

struct Bag
{
	std::array<Item, BAG_MAX_SPACE> items;
};