#include "PentrisAI.h"

/*Returns the valuation for the best found move and stores the best found move sequence into the bestMoveSequence member variable.
  maxDepth can be either 0 (in which case only the terminal positions of the current falling pentomino are enumerated and evaluated),
  or maxDepth can be 1 (in which case, through recursion, the next pentomino is also accounted for - i.e. the terminal positions of the
  current followed by next pentomino are enumerated and evaluated)*/
int PentrisAI::CalculateMoveSequence_Recursive(PentrisField field, unsigned char depth, unsigned char maxDepth)
{
    //Stores the currently analyzed move sequence
    std::vector<MoveData> moveSequence;
    //Stores the evaluation of the current position
    int eval;
    //Stores the worst-case valuation
    int maxEval = std::numeric_limits<int>::min();
    if (interrupt)
    {
        calculating = false;
        return maxEval + 1;
    } 
    
    //Look at the current or next pentomino depending on depth 0 or 1
    std::vector<int> pentomino;
    int posX, posY;
    if (depth == 0)
    {
        posX = field.pentominoX;
        posY = field.pentominoY;
        pentomino = field.currentPentomino;
        bestMoveSequence.clear();
    }
    else
    {
        posX = field.Width() / 2 - field.PENTOMINO_WIDTH / 2;
        posY = 0;
        pentomino = field.nextPentomino;
        if (!field.DoesPentominoFit(pentomino, posX, posY))
            return maxEval + 1;
    }
    
    //OUTERMOST LOOP 1 to enumerate moves - reflections
    for (int reflect = 0; reflect < 2; reflect++)
    {
        
        //Account for reflection symmetries: the centre point of each pentomino is always nonzero. Pentominos 10, 2, 7, 6, 8, 9 are reflection invariant
        if ((reflect >= 1) && ((pentomino[field.PENTOMINO_MID_INDEX] == 10) ||
            (pentomino[field.PENTOMINO_MID_INDEX] == 2) ||
            (pentomino[field.PENTOMINO_MID_INDEX] == 7) || 
            (pentomino[field.PENTOMINO_MID_INDEX] == 6) ||
            (pentomino[field.PENTOMINO_MID_INDEX] == 8) ||
            (pentomino[field.PENTOMINO_MID_INDEX] == 9)))
            continue;

        bool reflected = false;
        if ((reflect == 1) && field.DoesPentominoFit(field.ReflectPentomino(pentomino), posX, posY))
        {
            pentomino = field.ReflectPentomino((depth == 0) ? (field.currentPentomino) : (field.nextPentomino));
            moveSequence.push_back(MoveData(MoveType::REFLECT, 1));
            reflected = true;
        }

        //INNER LOOP 2 to enumerate moves - rotations
        for (int rotate = 0; rotate < 4; rotate++)
        {
            //Account for rotation symmetries: Pentomino 10 is rotation invariant, pentominos 2 and 12 are invariant to 180 degree rotation
            if ((rotate >= 1) && ((pentomino[field.PENTOMINO_MID_INDEX] == 10)))
                continue;
            if ((rotate >= 2) && ((pentomino[field.PENTOMINO_MID_INDEX] == 2) || 
                (pentomino[field.PENTOMINO_MID_INDEX] == 12)))
                continue;
            bool rotated = false;
            if ((rotate >= 1) && field.DoesPentominoFit(field.RotatePentomino(pentomino), posX, posY))
            {
                pentomino = field.RotatePentomino(pentomino);
                moveSequence.push_back(MoveData(MoveType::ROTATE, rotate));
                rotated = true;
            }
            int pentominoBoundLeft = field.PentominoBoundLeft(pentomino);
            int pentominoBoundRight = field.PentominoBoundRight(pentomino);
            int pentominoBoundBottom = field.PentominoBoundBottom(pentomino);

            //Lambda function to encapsulate a hard drop at "offset" to posX
            auto HardDrop = [&](int offset, int terminalY)
            {
                moveSequence.push_back(MoveData(MoveType::HARD_DROP, posX + offset));
                //MOVE SEQUENCE ENDED - evaluate field and undo
                field.InsertPentomino(pentomino, posX + offset, terminalY);
                if (depth == maxDepth)
                    eval = EvaluateField(field);
                else
                    eval = CalculateMoveSequence_Recursive(field, depth + 1, maxDepth);
                //Check if new optimum found
                if (eval > maxEval)
                {
                    if (depth == 0)
                        bestMoveSequence = moveSequence;
                    maxEval = eval;
                }
                field.RemovePentomino(pentomino, posX + offset, terminalY);
                moveSequence.pop_back();
            };

            //Lambda function to encapsulate a soft move down to terminalY at "offset" to posX, 
            //and then "offset_offset" moves to left (if negative) or right (if positive)
            auto DownSide = [&](int offset, int offset_offset, int terminalY)
            {
                if (field.DoesPentominoFit(pentomino, posX + offset + offset_offset, terminalY) && 
                   (!field.IsEmptyAbove(posX + offset + offset_offset + ((offset_offset < 0) ? pentominoBoundLeft : pentominoBoundRight), terminalY + field.PENTOMINO_WIDTH - pentominoBoundBottom + 1)))
                {
                    moveSequence.push_back(MoveData(MoveType::DOWN, terminalY));
                    moveSequence.push_back(MoveData((offset_offset < 0) ? MoveType::LEFT : MoveType::RIGHT, posX + offset + offset_offset));
                    moveSequence.push_back(MoveData(MoveType::HARD_DROP, 1));
                    //MOVE SEQUENCE ENDED - evaluate field and undo
                    field.InsertPentomino(pentomino, posX + offset + offset_offset, terminalY);
                    if (depth == maxDepth)
                        eval = EvaluateField(field);
                    else
                        eval = CalculateMoveSequence_Recursive(field, depth + 1, maxDepth);
                    if (eval > maxEval)
                    {
                        if (depth == 0)
                            bestMoveSequence = moveSequence;
                        maxEval = eval;
                    }
                    field.RemovePentomino(pentomino, posX + offset + offset_offset, terminalY);

                    moveSequence.pop_back();
                    moveSequence.pop_back();
                    moveSequence.pop_back();
                }
            };

            //INNER LOOP 3 to enumerate moves - left / right moves
            for (int offset = 0; ((posX - offset) >= 0) || ((posX + offset) < field.Width()); offset++)
            {
                int terminalY = field.GetTerminalY(pentomino, posX - offset, posY);

                if (field.DoesPentominoFit(pentomino, posX - offset, terminalY))
                {
                    if (offset != 0)
                        moveSequence.push_back(MoveData(MoveType::LEFT, posX - offset));
                    
                    HardDrop(-offset, terminalY);
                    //Go to terminal position and move left
                    DownSide(-offset, -1, terminalY);
                    //Go to terminal position and move right
                    DownSide(-offset, 1, terminalY);

                    if (offset != 0)
                        moveSequence.pop_back();

                }
                if (offset == 0)
                    continue;
                terminalY = field.GetTerminalY(pentomino, posX + offset, posY);
                if (field.DoesPentominoFit(pentomino, posX + offset, terminalY))
                {
                    if (offset != 0)
                        moveSequence.push_back(MoveData(MoveType::RIGHT, posX + offset));

                    HardDrop(offset, terminalY);
                    //Go to terminal position and move left
                    DownSide(offset, -1, terminalY);
                    //Go to terminal position and move right
                    DownSide(offset, 1, terminalY);

                    if (offset != 0)
                        moveSequence.pop_back();
                }
            }
            if (rotated)
                moveSequence.pop_back();
        }
        if (reflected)
            moveSequence.pop_back();
    }
    if (depth == 0)
        calculating = false;
    return maxEval;
}

int PentrisAI::EvaluateField(const PentrisField field)
{
    evalCalls++;
    int eval = 0;
    int maxHeight = 0;
    //The number of empty blocks in each row
    std::vector<int> blocksMissingRow;
    blocksMissingRow.resize(field.Height() - 1, 0);
    //The number of empty blocks above the highest block in each column
    std::vector<int> blocksMissingCol;
    blocksMissingCol.resize(field.Width() - 2, 0);
    int avg = 0;
    //The loop will detect overhangs; count empty blocks in rows; count empty blocks in columns; sum up column height differences
    for (int i = 1; i < field.Width() - 1; i++)
    {
        int columnHeight = 0;
        for (int j = 0; j < field.Height() - 1; j++)
        {

            if ((field(i,j) != 0) && (columnHeight == 0)) 
                columnHeight = (field.Height() - j - 1);
            else if (field(i, j) == 0)
            {
                //j loops from top to bottom. If columnHeight was already set in a prior loop, then an overhang must exist
                if (columnHeight > 0)
                    eval -= 50;
                    
                blocksMissingRow[j]++;
            }
        }
        blocksMissingCol[i - 1] = field.Height() - columnHeight - 1;

        if (columnHeight > maxHeight)
            maxHeight = columnHeight;

        //Add up the height difference between subsequent columns for later averaging
        if (i > 1)
            avg += abs(blocksMissingCol[i - 1] - blocksMissingCol[i - 2]);
    }
    //Punish extreme height differences between columns
    avg = (int)(avg * 20 / (field.Width() - 2));
    eval -= avg;

    //Reward filled rows
    for (auto &missingRow : blocksMissingRow)
        if (missingRow == 0)
            eval += 30;

    //Punish by height
    eval -= maxHeight;

    //Punish a field height at which the game would be lost
    if (field.Height() - maxHeight <= field.PENTOMINO_WIDTH + 1)
        eval -= 2000;
    return eval;
}

void PentrisAI::CalculateMoveSequence(const PentrisField field, unsigned char maxDepth)
{
    evalCalls = 0;
    PentrisField c_field = field;
    interrupt = false;
    calculating = true;
    threadSpawned = true;
    aiThread = std::thread(&PentrisAI::CalculateMoveSequence_Recursive, this, c_field, 0, maxDepth);
}

bool PentrisAI::AIThreadJoined()
{
    if (!calculating) 
    {
        if (threadSpawned)
        {
            aiThread.join();
            threadSpawned = false;
        }
        return true;
    }
    else
        return false;
}
