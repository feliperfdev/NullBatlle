#pragma once

#include <map>

enum class ActionType {
	CHOOSE_POKEMON,
	USE_MOVE,
	USE_ITEM
};

struct BattleAction
{
	ActionType type;
	int index;
};

inline std::map<int, ActionType> ActionTypeMap = {
	{0, ActionType::CHOOSE_POKEMON},
	{1, ActionType::USE_MOVE},
	{2, ActionType::USE_ITEM}
};