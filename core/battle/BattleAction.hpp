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