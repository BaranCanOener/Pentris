#pragma once

#include "PentrisField.h"
#include <vector>
#include <thread>

enum class MoveType { HARD_DROP, LEFT, RIGHT, DOWN, ROTATE, REFLECT };

struct MoveData {
    MoveType moveType = MoveType::HARD_DROP;
    int destination = 1;

    MoveData(MoveType mType, int dest) {
        moveType = mType;
        destination = dest;
    }
    MoveData() {};
};

class PentrisAI
{
private:
    bool calculating = false;
    bool threadSpawned = false;
    int CalculateMoveSequence_Recursive(PentrisField field, unsigned char depth = 0, unsigned char maxDepth = 1);
public:
    int evalCalls;

    bool interrupt = false;
    std::thread aiThread;
    std::vector<MoveData> bestMoveSequence;
    int EvaluateField(const PentrisField field);
    void CalculateMoveSequence(const PentrisField field, unsigned char maxDepth = 1);
    bool AIThreadJoined();
};

