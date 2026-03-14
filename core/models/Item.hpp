#include <string>
#include <optional>

struct Item
{
	std::string name;
	unsigned int itemType;

	// 0 = None, 1 = Full heal, 2 = Burn heal, 3 = Antidote, 4 = Awakening
	int healEffect;

	std::optional<int> healHP;
};