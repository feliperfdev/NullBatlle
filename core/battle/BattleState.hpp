#include <map>

enum class BattleState {
	TEAM_SELECT,
	ACTION_TURN,
	ACTION_EXECUTING_TURN,
	END_TURN,
	SWITCH_IN,
	USE_ITEM,
	SWITCH_AFTER_FAINT,
	BATTLE_END
};


inline std::map<BattleState, std::string> BattleStateMap = {
	{BattleState::TEAM_SELECT, "TEAM_SELECT"},
	{BattleState::ACTION_TURN, "ACTION_TURN"},
	{BattleState::ACTION_EXECUTING_TURN, "ACTION_EXECUTING_TURN"},
	{BattleState::END_TURN, "END_TURN"},
	{BattleState::SWITCH_IN, "SWITCH_IN"},
	{BattleState::USE_ITEM, "USE_ITEM"},
	{BattleState::SWITCH_AFTER_FAINT, "SWITCH_AFTER_FAINT"},
	{BattleState::BATTLE_END, "BATTLE_END"}
};
