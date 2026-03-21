add_library(nullbattle_data
    DataLoader.cpp
)

target_include_directories(nullbattle_data PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(nullbattle_data
    PRIVATE nlohmann_json::nlohmann_json
    PUBLIC  nullbattle_core
)