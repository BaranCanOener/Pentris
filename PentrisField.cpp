#include "PentrisField.h"

PentrisField::PentrisField(const unsigned width, const unsigned height)
{
	fieldWidth = width;
	fieldHeight = height;
    Reset();
}

/*Construct field at default width and height*/
PentrisField::PentrisField()
{
    Reset();
}

/*Copy constructor*/
PentrisField::PentrisField(const PentrisField& rhs)
{
    fieldWidth = rhs.Width();
    fieldHeight = rhs.Height();
    pentominoX = rhs.pentominoX;
    pentominoY = rhs.pentominoY;
    currentPentomino = rhs.currentPentomino;
    nextPentomino = rhs.nextPentomino;
    blocks = rhs.blocks;
}

/*Resets the field to an empty state and generates a random next and current pentomino*/
void PentrisField::Reset()
{
    blocks.resize(fieldWidth * fieldHeight, 0);
    std::fill(blocks.begin(), blocks.end(), 0);
    for (int j = 0; j < fieldHeight; j++)
    {
        blocks[j * fieldWidth] = 13;
        blocks[j * fieldWidth + fieldWidth - 1] = 13;
    }
    for (int i = 0; i < fieldWidth; i++)
        blocks[(fieldHeight - 1) * fieldWidth + i] = 13;
    currentPentomino = GetRandomPentomino();
    nextPentomino = GetRandomPentomino();
    pentominoX = fieldWidth / 2 - PENTOMINO_WIDTH / 2;
    pentominoY = 0;
}

/*Sets each block to FILLEDROW if its corresponding row has no gaps
  This will only affect all rows from and including to the passed parameters
  The function returns the number of marked rows (that were not marked before!)*/
int PentrisField::MarkFilledRows(const int fromRow, const int toRow)
{
    int lines = 0;
    int from = std::max(fromRow, 0);
    int to = std::min(toRow, fieldHeight -  2);
    for (int j = from; j <= to; j++)
    {
        bool line = true;
        for (int i = 0; i < fieldWidth; i++)
        {
            if ((blocks[i + j * fieldWidth] == 0) || (blocks[i + j * fieldWidth] == FILLEDROW))
            {
                line = false;
                break;
            }
        }
        if (line)
        {
            lines++;
            for (int i = 1; i < fieldWidth - 1; i++) 
                blocks[i + j * fieldWidth] = FILLEDROW;
        }
    }
    return lines;
}

int PentrisField::ClearFilledRows()
{
    int filledRows = 0;
    for (int i = 0; i < fieldHeight - 1; i++)
        if (blocks[1 + i * fieldWidth] == FILLEDROW)
        {
            //Move everything above row i one row down
            //REMARK: Potential for efficiency gains using STL
            filledRows++;
            for (int k = 1; k < fieldWidth - 1; k++) {
                for (int j = i; j > 0; j--) {
                    blocks[k + j * fieldWidth] = blocks[k + (j - 1) * fieldWidth];
                    blocks[k + (j - 1) * fieldWidth] = 0;
                }
            }
        }
    return filledRows;
}

std::vector<int> PentrisField::GetRandomPentomino() const
{
    int rnd = std::rand() % 12;
    if (rnd == 0)
        return PENTOMINO1;
    else if (rnd == 1)
        return PENTOMINO2;
    else if (rnd == 2)
        return PENTOMINO3;
    else if (rnd == 3)
        return PENTOMINO4;
    else if (rnd == 4)
        return PENTOMINO5;
    else if (rnd == 5)
        return PENTOMINO6;
    else if (rnd == 6)
        return PENTOMINO7;
    else if (rnd == 7)
        return PENTOMINO8;
    else if (rnd == 8)
        return PENTOMINO9;
    else if (rnd == 9)
        return PENTOMINO10;
    else if (rnd == 10)
        return PENTOMINO11;
    else
        return PENTOMINO12;
}

/*Inserts a given pentomino into the game field at the top left position posX and posY
  This will only insert into spaces that are empty, i.e. it will only overwrite the field where it is set to 0*/
void PentrisField::InsertPentomino(const std::vector<int> pentomino, const int posX, const int posY)
{
    if (pentomino.size() < PENTOMINO_WIDTH * PENTOMINO_WIDTH)
        return;
    for (int i = 0; i < PENTOMINO_WIDTH; i++)
        for (int j = 0; j < PENTOMINO_WIDTH; j++)
            if ((posX + i > 0) && (posX + i < fieldWidth - 1) && (posY + j >= 0) && (posY + j < fieldHeight - 1) && (pentomino[i + j * PENTOMINO_WIDTH] != 0))
                blocks[posX + i + (posY + j) * fieldWidth] = pentomino[i + j * PENTOMINO_WIDTH];
}

/*Removes a given pentomino from the game field at the top left position posX and posY
  This will only set blocks to zero whose number coincide with the passed pentomino*/
void PentrisField::RemovePentomino(const std::vector<int> pentomino, const int posX, const int posY)
{
    if ((pentomino.size() < PENTOMINO_WIDTH * PENTOMINO_WIDTH))
        return;
    for (int i = 0; i < PENTOMINO_WIDTH; i++)
        for (int j = 0; j < PENTOMINO_WIDTH; j++)
            if ((posX + i > 0) && (posX + i < fieldWidth - 1) && (posY + j >= 0) && (posY + j < fieldHeight - 1) && (pentomino[i + j * PENTOMINO_WIDTH] == blocks[posX + i + (posY + j) * fieldWidth]))
                blocks[posX + i + (posY + j) * fieldWidth] = 0;
}

/*Checks if the given pentomino fits into the game field at the top left position posX and posY*/
bool PentrisField::DoesPentominoFit(const std::vector<int> pentomino, const int posX, const int posY) const
{
    if ((pentomino.size() < PENTOMINO_WIDTH * PENTOMINO_WIDTH) || (posY > fieldHeight))
        return false;
    //i loops through the pentomino columns, j through rows
    for (int i = 0; i < PENTOMINO_WIDTH; i++)
        for (int j = 0; j < PENTOMINO_WIDTH; j++)
        {
            //Check if blocks at posX+i, posY+j is nonempty (if coordinates are in range) - if so, there is a collision and hence return false
            if (((posX + i >= 0) && (posX + i < fieldWidth) && (posY + j >= 0) && (posY + j < fieldHeight) && (pentomino[i + j * PENTOMINO_WIDTH] != 0) && (blocks[posX + i + (posY + j) * fieldWidth] != 0))
                || ((pentomino[i + j * PENTOMINO_WIDTH] != 0) && ((posX + i < 0) || (posX + i >= fieldWidth))))
                return false;
        }
    return true;
}

/*Returns the leftmost column index for which there is a nonzero block in pentomino (between 0 and PENTOMINO_WIDTH; returns -1 if the pentomino is incorrectly sized or empty)*/
int PentrisField::PentominoBoundLeft(const std::vector<int> pentomino) const
{
    if (pentomino.size() < PENTOMINO_WIDTH * PENTOMINO_WIDTH)
        return -1;
    for (int i = 0; i < PENTOMINO_WIDTH; i++)
        for (int j = 0; j < PENTOMINO_WIDTH; j++)
            if (pentomino[i + j * PENTOMINO_WIDTH] != 0)
                return i;
    return -1;
}

/*Returns the rightmost column index for which there is a nonzero block in pentomino (between 0 and PENTOMINO_WIDTH; returns -1 if the pentomino is incorrectly sized or empty)*/
int PentrisField::PentominoBoundRight(const std::vector<int> pentomino) const
{
    if (pentomino.size() < PENTOMINO_WIDTH * PENTOMINO_WIDTH)
        return -1;
    for (int i = PENTOMINO_WIDTH - 1; i >= 0; i--)
        for (int j = 0; j < PENTOMINO_WIDTH; j++)
            if (pentomino[i + j * PENTOMINO_WIDTH] != 0)
                return i;
    return -1;
}

/*Returns the bottommost row index for which there is a nonzero block in pentomino (between 0 and PENTOMINO_WIDTH; returns -1 if the pentomino is incorrectly sized or empty)*/
int PentrisField::PentominoBoundBottom(const std::vector<int> pentomino) const
{
    if (pentomino.size() < PENTOMINO_WIDTH * PENTOMINO_WIDTH)
        return -1;
    for (int j = PENTOMINO_WIDTH - 1; j >= 0; j--)
        for (int i = 0; i < PENTOMINO_WIDTH; i++)
            if (pentomino[i + j * PENTOMINO_WIDTH] != 0)
                return j;
    return -1;
}

/*Rotates the given pentomino clockwise by 90 degrees, or returns the given pentomino if it is of insufficient size*/
std::vector<int> PentrisField::RotatePentomino(const std::vector<int> pentomino) const
{
    if (pentomino.size() < PENTOMINO_WIDTH * PENTOMINO_WIDTH)
        return pentomino;
    std::vector<int> rotPentomino;
    rotPentomino.resize(PENTOMINO_WIDTH * PENTOMINO_WIDTH, 0);
    for (int i = 0; i < PENTOMINO_WIDTH; i++)
        for (int j = 0; j < PENTOMINO_WIDTH; j++)
            rotPentomino[i + j * PENTOMINO_WIDTH] = pentomino[20 + j - 5 * i];
    return rotPentomino;
}

/*Reflects the given pentomino on its central vertical axis, or returns the given pentomino if it is of insufficient size*/
std::vector<int> PentrisField::ReflectPentomino(const std::vector<int> pentomino) const
{
    if (pentomino.size() < PENTOMINO_WIDTH * PENTOMINO_WIDTH)
        return pentomino;
    std::vector<int> refPentomino;
    refPentomino.resize(PENTOMINO_WIDTH * PENTOMINO_WIDTH, 0);
    for (int i = 0; i < PENTOMINO_WIDTH; i++)
        for (int j = 0; j < PENTOMINO_WIDTH; j++)
            refPentomino[i + j * PENTOMINO_WIDTH] = pentomino[PENTOMINO_WIDTH - i - 1 + j * PENTOMINO_WIDTH];
    return refPentomino;
}

bool PentrisField::RotateCurrentPentomino()
{
    std::vector<int> rotPentomino = RotatePentomino(currentPentomino);
    if (DoesPentominoFit(rotPentomino, pentominoX, pentominoY)) {
        currentPentomino = rotPentomino;
    }
    else if (DoesPentominoFit(rotPentomino, pentominoX + 1, pentominoY)) {
        pentominoX++;
        currentPentomino = rotPentomino;
    }
    else if (DoesPentominoFit(rotPentomino, pentominoX + 2, pentominoY)) {
        pentominoX += 2;
        currentPentomino = rotPentomino;
    }
    else if (DoesPentominoFit(rotPentomino, pentominoX - 1, pentominoY)) {
        pentominoX--;
        currentPentomino = rotPentomino;
    }
    else if (DoesPentominoFit(rotPentomino, pentominoX - 2, pentominoY)) {
        pentominoX -= 2;
        currentPentomino = rotPentomino;
    }
    else
        return false;
    return true;
}

bool PentrisField::ReflectCurrentPentomino()
{
    std::vector<int> refPentomino = ReflectPentomino(currentPentomino);
    if (DoesPentominoFit(refPentomino, pentominoX, pentominoY)) {
        currentPentomino = refPentomino;
    }
    //If the pentomino cannot be reflected in its current position, then test positions to the right & left
    else if (DoesPentominoFit(refPentomino, pentominoX + 1, pentominoY)) {
        pentominoX++;
        currentPentomino = refPentomino;
    }
    else if (DoesPentominoFit(refPentomino, pentominoX + 2, pentominoY)) {
        pentominoX += 2;
        currentPentomino = refPentomino;
    }
    else if (DoesPentominoFit(refPentomino, pentominoX - 1, pentominoY)) {
        pentominoX--;
        currentPentomino = refPentomino;
    }
    else if (DoesPentominoFit(refPentomino, pentominoX - 2, pentominoY)) {
        pentominoX -= 2;
        currentPentomino = refPentomino;
    }
    else
        return false;
    return true;
}

bool PentrisField::MoveLeftCurrentPentomino()
{
    if (DoesPentominoFit(currentPentomino, pentominoX - 1, pentominoY)) {
        pentominoX--;
        return true;
    }
    else
        return false;
}

bool PentrisField::MoveRightCurrentPentomino()
{
    if (DoesPentominoFit(currentPentomino, pentominoX + 1, pentominoY)) {
        pentominoX++;
        return true;
    }
    else
        return false;
}

bool PentrisField::MoveDownCurrentPentomino()
{
    if (DoesPentominoFit(currentPentomino, pentominoX, pentominoY + 1)) {
        pentominoY++;
        return true;
    }
    else
        return false;
}

void PentrisField::InsertCurrentPentomino()
{
    InsertPentomino(currentPentomino, pentominoX, pentominoY);
    currentPentomino = nextPentomino;
    nextPentomino = GetRandomPentomino();
    pentominoX = fieldWidth / 2 - PENTOMINO_WIDTH / 2;
    pentominoY = 0;
}

int PentrisField::GetTerminalY(std::vector<int> pentomino, int posX, int posY) const
{
    int terminalY = posY;
    while (DoesPentominoFit(pentomino, posX, terminalY + 1))
        terminalY++;
    return terminalY;
}

int PentrisField::GetTerminalY() const
{
    return GetTerminalY(currentPentomino, pentominoX, pentominoY);
}

bool PentrisField::IsEmptyAbove(const int posX, const int posY) const
{
    if ((posX > fieldWidth - 1) || (posX < 1))
        return false;
    for (int j = std::min(fieldHeight - 1, posY) - 1; j > 0; j--)
        if (blocks[posX + j * fieldWidth] != 0)
            return false;
    return true;
}

/*Checks whether either to the left or to the right of(posX, posY) there is an empty square of clearance * clearance size
  Example:
    3---444
    3----4
   33---P4
  Will return true for square at position P and clearance=3 (but false for =4)*/
bool PentrisField::MinOverhangClearance(const int posX, const int posY, const int clearance) const
{
    bool leftClearance = true;
    bool rightClearance = true;
    if (posX - clearance <= 0)
        leftClearance = false;
    if (posX + clearance >= fieldWidth - 1)
        rightClearance = false;
    if (((!leftClearance && !rightClearance)) || (posY <= clearance))
        return false;
    for (int offsetX = 1; offsetX <= clearance; offsetX++)
        for (int offsetY = 0; offsetY < clearance; offsetY++) {
            if ((posX - offsetX >= 0) && (blocks[posX - offsetX + (posY - offsetY) * fieldWidth] != 0))
                leftClearance = false;
            if ((posX + offsetX < fieldWidth) && (blocks[posX + offsetX + (posY - offsetY) * fieldWidth] != 0))
                rightClearance = false;
        }
    return (rightClearance || leftClearance);
}

const int& PentrisField::operator()(const unsigned posX, const unsigned posY) const
{
    return blocks[posX + posY * fieldWidth];
}

//Elementwise accessor
int& PentrisField::operator()(const unsigned posX, const unsigned posY)
{
    return blocks[posX + posY * fieldWidth];
}
