#pragma once

#include <map>

enum class ActionType : int {
	USE_MOVE = 1,
	CHOOSE_POKEMON = 2,
	USE_ITEM = 3
};

struct BattleAction
{
	ActionType type;
	int index;
};

inline std::map<int, ActionType> ActionTypeMap = {
	{2, ActionType::CHOOSE_POKEMON},
	{1, ActionType::USE_MOVE},
	{3, ActionType::USE_ITEM}
};