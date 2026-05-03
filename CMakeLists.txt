# CMakeList.txt : CMake project for NullBattle, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.10)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("NullBattle")

# Add source to this project's executable.
add_executable(NullBattle "NullBattle.cpp" "NullBattle.h" "common/UuidV4.cpp" "common/pokemon_print_helpers.cpp" "common/LogQueue.cpp")

include(FetchContent)

FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.12.0/json.tar.xz)
FetchContent_MakeAvailable(json)

target_link_libraries(NullBattle PRIVATE
    nullbattle_core
    nullbattle_data
    nlohmann_json::nlohmann_json
)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET NullBattle PROPERTY CXX_STANDARD 20)
endif()


set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(core)
add_subdirectory(data)
add_subdirectory(ui)