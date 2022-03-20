#pragma once

#include <vector>

/*Encapsulates the width*height sized game field and each of the 12 possible pentominos.
Contains method for game field and pentomino manipulation
(clearing filled lines, rotating & reflecting pentominos etc.)*/
class PentrisField
{
private:
    //The game field measured in blocks (where each pentomino fits in a 5x5 block)
    int fieldWidth = 18;
    int fieldHeight = 35;
    //The field is stored as a vector of size field width * field height, accessed in 1d via field[x + y * FIELD_WIDTH]

public:
    std::vector<int> blocks;

    const int PENTOMINO_WIDTH = 5;
    //Each pentomino is identified by an integer 1-12 (excluding reflection symmetries),
    //and each pentomino is stored as a vector (where the identifying integer indicates the presence of a block)
    //This is used for collision detection and for rendering each pentomino in its respective color
    const std::vector<int> PENTOMINO1 =
    { 0, 0, 0, 0, 0,
      0, 0, 1, 1, 0,
      0, 1, 1, 0, 0,
      0, 0, 1, 0, 0,
      0, 0, 0, 0, 0 };

    const std::vector<int> PENTOMINO2 =
    { 0, 0, 2, 0, 0,
      0, 0, 2, 0, 0,
      0, 0, 2, 0, 0,
      0, 0, 2, 0, 0,
      0, 0, 2, 0, 0 };

    const std::vector<int> PENTOMINO3 =
    { 0, 0, 3, 0, 0,
      0, 0, 3, 0, 0,
      0, 0, 3, 0, 0,
      0, 0, 3, 3, 0,
      0, 0, 0, 0, 0 };

    const std::vector<int> PENTOMINO4 =
    { 0, 0, 0, 4, 0,
      0, 0, 4, 4, 0,
      0, 0, 4, 0, 0,
      0, 0, 4, 0, 0,
      0, 0, 0, 0, 0 };

    const std::vector<int> PENTOMINO5 =
    { 0, 0, 0, 0, 0,
      0, 0, 5, 5, 0,
      0, 0, 5, 5, 0,
      0, 0, 5, 0, 0,
      0, 0, 0, 0, 0 };

    const std::vector<int> PENTOMINO6 =
    { 0, 0, 0, 0, 0,
      0, 6, 6, 6, 0,
      0, 0, 6, 0, 0,
      0, 0, 6, 0, 0,
      0, 0, 0, 0, 0 };

    const std::vector<int> PENTOMINO7 =
    { 0, 0, 0, 0, 0,
      0, 7, 0, 7, 0,
      0, 7, 7, 7, 0,
      0, 0, 0, 0, 0,
      0, 0, 0, 0, 0 };

    const std::vector<int> PENTOMINO8 =
    { 0, 0, 0, 0, 0,
      0, 0, 8, 0, 0,
      0, 0, 8, 0, 0,
      0, 0, 8, 8, 8,
      0, 0, 0, 0, 0 };

    const std::vector<int> PENTOMINO9 =
    { 0, 0, 0, 0, 0,
      0, 9, 0, 0, 0,
      0, 9, 9, 0, 0,
      0, 0, 9, 9, 0,
      0, 0, 0, 0, 0 };

    const std::vector<int> PENTOMINO10 =
    { 0, 0, 0, 0, 0,
      0, 0, 10, 0, 0,
      0, 10, 10, 10, 0,
      0, 0, 10, 0, 0,
      0, 0, 0, 0, 0 };

    const std::vector<int> PENTOMINO11 =
    { 0, 0, 11, 0, 0,
      0, 11, 11, 0, 0,
      0, 0, 11, 0, 0,
      0, 0, 11, 0, 0,
      0, 0, 0, 0, 0 };

    const std::vector<int> PENTOMINO12 =
    { 0, 0, 0, 0, 0,
      0, 12, 12, 0, 0,
      0, 0, 12, 0, 0,
      0, 0, 12, 12, 0,
      0, 0, 0, 0, 0 };

    const int WALL = 13;
    const int FILLEDROW = 14;

    const int PENTOMINO_MID_INDEX = 12;

    //The top left coordinates of the current pentomino
    int pentominoX = fieldWidth / 2 - PENTOMINO_WIDTH / 2;
    int pentominoY = 0;

    //The current falling Pentomino and the next one in line
    std::vector<int> currentPentomino;
    std::vector<int> nextPentomino;

    PentrisField(const unsigned width, const unsigned height);
    PentrisField();
    PentrisField(const PentrisField& rhs);
    void Reset();
    int MarkFilledRows(const int fromRow, const int toRow);
    int ClearFilledRows();
    void InsertPentomino(const std::vector<int> pentomino, const int posX, const int posY);
    void RemovePentomino(const std::vector<int> pentomino, const int posX, const int posY);
    bool DoesPentominoFit(const std::vector<int> pentomino, const int posX, const int posY) const;
    int PentominoBoundLeft(const std::vector<int> pentomino) const;
    int PentominoBoundRight(const std::vector<int> pentomino) const;
    int PentominoBoundBottom(const std::vector<int> pentomino) const;
    std::vector<int> GetRandomPentomino() const;
    std::vector<int> RotatePentomino(const std::vector<int> pentomino) const;
    std::vector<int> ReflectPentomino(const std::vector<int> pentomino) const;
    bool RotateCurrentPentomino();
    bool ReflectCurrentPentomino();
    bool MoveLeftCurrentPentomino();
    bool MoveRightCurrentPentomino();
    bool MoveDownCurrentPentomino();
    void InsertCurrentPentomino();

    int GetTerminalY(std::vector<int> pentomino, int posX, int posY) const;
    int GetTerminalY() const;
    bool IsEmptyAbove(const int posX, const int posY) const;
    bool MinOverhangClearance(const int posX, const int posY, const int clearance) const;

    int Width() const { return fieldWidth; };
    int Height() const { return fieldHeight; };
    const int& operator()(const unsigned posX, const unsigned posY) const;
    int& operator()(const unsigned posX, const unsigned posY);
};

