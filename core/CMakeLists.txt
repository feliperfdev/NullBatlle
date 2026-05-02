add_library(nullbattle_core
    battle/BattleStateMachine.cpp
    battle/TurnEngine.cpp
  "battle/ConditionEngine.cpp")

target_include_directories(nullbattle_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})