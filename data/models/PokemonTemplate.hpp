#ifndef POKEMONTEMPLATE
#define POKEMONTEMPLATE

#include "LearnsetEntry.hpp"
#include "../../core/models/Pokemon.hpp"
#include "../../core/models/Move.hpp"
#include <array>
#include <vector>

struct PokemonTemplate {
    int id;
    std::string name;
    std::array<Types, 2> types;
    std::array<unsigned int, 6> baseStats;
    std::vector<LearnsetEntry> learnset;

    public:
        Pokemon build(
            const unsigned int level,
            const std::array<std::string, 4>& moveNames,
            const std::map<std::string, Move>& movesMap
        ) const {
            std::array<Move, 4> moves = {};

            for (int i = 0; i < moves.size(); i++) {
                std::string moveName = moveNames[i];

                Move move = movesMap.at(moveName);

                if (move.name == moveName) {
                    moves[i] = move;
                }
            }

            return Pokemon{ name, level, baseStats[0], types, baseStats, BattleCondition::NONE, moves };
        }
};

#endif // POKEMONTEMPLATE