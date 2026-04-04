#include <string>
#include <array>
#include <vector>
#include <iostream>
#include <iomanip>
#include "../core/models/Pokemon.hpp"
#include "../core/models/Player.hpp"

void clearScreen();
std::string printPokemonData(const Pokemon& pokemon);
void printBattlePanel();
void printMovesPanel(const Pokemon& pokemon, int playerId);
void printVictoryScreen(const Player& winner, int totalTurns);