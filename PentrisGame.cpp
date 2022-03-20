#include "PentrisGame.h"

/*Handles the player-dictated pentomino movement and other commands by the player*/
void PentrisGame::UserInputHandling(float fElapsedTime)
{
    if (!gameOver)
    {
        moveTimer -= fElapsedTime;
        bool resetMoveTimer = false;
        if (GetKey(olc::Key::LEFT).bHeld)
        { 
            //Ensure that if the key is held down, movement only occurs every moveAfterSeconds
            if (moveTimer <= 0) { 
                pentrisField.MoveLeftCurrentPentomino();
                resetMoveTimer = true;
            }
        }
        //If the key is released, always move-- i.e. player can move a piece as quickly as he can tap the key
        if (GetKey(olc::Key::LEFT).bReleased)
            resetMoveTimer = true; 
        if (GetKey(olc::Key::RIGHT).bHeld)
        {
            if (moveTimer <= 0) {
                pentrisField.MoveRightCurrentPentomino();
                resetMoveTimer = true;
            }
        }
        if (GetKey(olc::Key::RIGHT).bReleased)
            resetMoveTimer = true;
        if (GetKey(olc::Key::DOWN).bHeld)
        {
            if (moveTimer <= 0)
            {
                fallTimer = 0;
                resetMoveTimer = true;
            }
        }
        if (GetKey(olc::Key::RIGHT).bReleased)
            resetMoveTimer = true;
        if (GetKey(olc::Key::C).bPressed)
        {
            terminalY = pentrisField.GetTerminalY();
            pentrisField.RotateCurrentPentomino();
        }
        if (GetKey(olc::Key::V).bPressed)
        {
            terminalY = pentrisField.GetTerminalY();
            pentrisField.ReflectCurrentPentomino();
        }
        if (GetKey(olc::Key::SPACE).bPressed)
        {
            pentrisField.pentominoY = terminalY;
            fallTimer = 0;
        }
        if (resetMoveTimer)
            moveTimer = moveAfterSeconds;
    }
    if (GetKey(olc::Key::ENTER).bPressed)
        NewGame();
    if (GetKey(olc::Key::E).bPressed)
        std::cout << pentrisAI.EvaluateField(pentrisField);
    if (GetKey(olc::Key::A).bPressed)
    {
        aiLoop = !aiLoop;
        if (aiLoop) {
            pentrisAI.CalculateMoveSequence(pentrisField, 0);
            aiCalcStarted = timeElapsed;
            pentrisAI.bestMoveSequence = pentrisAI.bestMoveSequence;
        }
    }
    if (GetKey(olc::Key::P).bPressed)
    {
        //Speed up AI
        aiMoveAfterSeconds = std::min(0.0f, aiMoveAfterSeconds - 0.01f);
    }
    if (GetKey(olc::Key::O).bPressed)
    {
        //Slow down AI
        aiMoveAfterSeconds = std::max(0.0f, aiMoveAfterSeconds + 0.01f);
    }
    if (GetKey(olc::Key::D).bPressed)
    {
        pentrisAI.CalculateMoveSequence(pentrisField, 0);
        while (!pentrisAI.AIThreadJoined());
        for (auto &move : pentrisAI.bestMoveSequence) {
            if (move.moveType == MoveType::REFLECT) std::cout << "Reflect, ";
            else if (move.moveType == MoveType::ROTATE) std::cout << "Rotate " << move.destination << ", ";
            else if (move.moveType == MoveType::LEFT) std::cout << "Left " << move.destination << ", ";
            else if (move.moveType == MoveType::RIGHT) std::cout << "Right " << move.destination << ", ";
            else if (move.moveType == MoveType::DOWN) std::cout << "Down " << move.destination << ", ";
            else if (move.moveType == MoveType::HARD_DROP) std::cout << "Drop!";
            std::cout << std::endl;
        }
        std::cout << pentrisAI.evalCalls << std::endl;
    }
    if (GetKey(olc::Key::B).bPressed)
        std::cout << pentrisField.PentominoBoundLeft(pentrisField.currentPentomino) << ", " << pentrisField.PentominoBoundRight(pentrisField.currentPentomino) << ", " << pentrisField.PentominoBoundBottom(pentrisField.currentPentomino) << std::endl;
}

/*Handles the execution "input" supplied by the Pentris AI.
  Three main branches:
  - The AI is still calculating; hence only check if the threshhold is reached and force an interrupt
  - The AI is supposed to play and has finished calculating; hence execute the current move within
    the best found move sequence and then pop it out of the bestMoveSequence vector
  - The AI is supposed to play but the game is over, hence restart a new game*/
void PentrisGame::AIInputHandling(float fElapsedTime)
{
    aiMoveTimer -= fElapsedTime;
    //Check if the AI is currently still calculating
    if (!pentrisAI.AIThreadJoined())
    {
        //If so, force interrupt if time threshold is reached
        if (timeElapsed - aiCalcStarted > aiCalcCutoff)
            pentrisAI.interrupt = true;
    }
    else if ((aiLoop) && (!pentrisAI.bestMoveSequence.empty()) && pentrisField.DoesPentominoFit(pentrisField.currentPentomino, pentrisField.pentominoX, pentrisField.pentominoY) && (aiMoveTimer <= 0))
    {
        //If the thread is not calculating, the AI is supposed to play (aiLoop == true), the current pentomino fits and there are moves to execute -
        //then execute the move currently in line. This can only occur if aiMoveTimer <= 0, ie. every aiMoveAfterSeconds
        MoveData move = *pentrisAI.bestMoveSequence.begin();
        if (move.moveType == MoveType::HARD_DROP)
        {
            //Force the current piece down
            pentrisField.pentominoY = terminalY;
            fallTimer = 0;
            pentrisAI.bestMoveSequence.erase(pentrisAI.bestMoveSequence.begin());
        }
        if (move.moveType == MoveType::LEFT)
        {
            if (pentrisField.pentominoX <= move.destination) //No move to execute, the pentomino is at or ahead of destination
                pentrisAI.bestMoveSequence.erase(pentrisAI.bestMoveSequence.begin());
            else {
                if (!pentrisField.MoveLeftCurrentPentomino()) //Move pentomino left; if impossible,
                    pentrisAI.bestMoveSequence.begin()->destination++; //increase the destination of the current move
            }
        }
        if (move.moveType == MoveType::RIGHT)
        {
            if (pentrisField.pentominoX >= move.destination) //see above
                pentrisAI.bestMoveSequence.erase(pentrisAI.bestMoveSequence.begin());
            else {
                if (!pentrisField.MoveRightCurrentPentomino())
                    pentrisAI.bestMoveSequence.begin()->destination--;
            }
        }
        if (move.moveType == MoveType::DOWN)
        {
            if (pentrisField.pentominoY >= move.destination)
                pentrisAI.bestMoveSequence.erase(pentrisAI.bestMoveSequence.begin());
            else
                fallTimer = -1.0f; //force the pentomino down
        }
        if (move.moveType == MoveType::ROTATE)
        {
            if (move.destination <= 0)
                pentrisAI.bestMoveSequence.erase(pentrisAI.bestMoveSequence.begin());
            else
            {
                pentrisField.RotateCurrentPentomino();
                pentrisAI.bestMoveSequence.begin()->destination--;
            }
        }
        if (move.moveType == MoveType::REFLECT)
        {
            pentrisAI.bestMoveSequence.erase(pentrisAI.bestMoveSequence.begin());
            pentrisField.ReflectCurrentPentomino();
        }
        aiMoveTimer = std::min(aiMoveAfterSeconds, fallAfterSeconds - 0.02f);
    }
    else if ((aiLoop) && (gameOver))
    {
        NewGame();
        pentrisAI.CalculateMoveSequence(pentrisField);
        aiCalcStarted = timeElapsed;
        pentrisAI.bestMoveSequence = pentrisAI.bestMoveSequence;
    }
        
}

/* The main game logic method
   - Handles the natural downward movement of the pentomino according to internal timers
   - places it when it reaches its final destination, and marks/destroys filled lines according to internal timers.
   - Speeds up the game every 2 pieces
   - If the AI is supposed to play, it ensures that the moves are recalculated as needed*/
void PentrisGame::PentominoMovementHandling(float fElapsedTime)
{
    //Check if the game is ended in the current frame
    bool gameOverFrame = !pentrisField.DoesPentominoFit(pentrisField.currentPentomino, pentrisField.pentominoX, pentrisField.pentominoY);
    //if so, and it was running just before, update the cumulative variables
    if ((gameOverFrame) && (!gameOver))
    {
        scoreCumulative += score;
        linesFilledCumulative += linesFilled;
        games++;
    }
    gameOver = gameOverFrame;

    //Make the pentomino fall down if the game isn't over and the timer reached 0
    fallTimer -= fElapsedTime;
    bool recalculateAImove = false;
    if ((fallTimer <= 0) && (!gameOver))
    {
        fallTimer = fallAfterSeconds;
        //Move down the current pentomino. if MoveDownCurrentPentomino returns false, it is at the field end..
        if (!pentrisField.MoveDownCurrentPentomino())
        {
            //Hence insert - the method will cycle to the next pentomino
            pentrisField.InsertCurrentPentomino();
            pieceCount++;

            if (pieceCount % 2 == 0)
                if (fallAfterSeconds >= 0.05)
                    fallAfterSeconds -= 0.02;

            //Check for filled lines
            int newLinesFilled = pentrisField.MarkFilledRows(terminalY, terminalY + pentrisField.PENTOMINO_WIDTH - 1);
            if (newLinesFilled > 0) {
                //If clearTimer <= clearLinesAfterSeconds, then previously cleared lines are currently "flashing", 
                if (clearTimer <= clearLinesAfterSeconds)
                    //hence make sure that they will be destroyed them in this frame (to clear up the field for fast players!)
                    clearTimer = 0.0f;
                else
                    clearTimer = clearLinesAfterSeconds;
                linesFilled += newLinesFilled;
                score += (1 << newLinesFilled) * 100;
            }

            terminalY = pentrisField.GetTerminalY();
            //The field topology has changed because a pentomin was dropped, hence AI needs to recalculate its move
            recalculateAImove = true;
        }
    }

    clearTimer -= fElapsedTime;
    if (clearTimer <= 0)
    {
        //The flashing effect is supposed to end if clearTimer reaches 0 and filled lines are supposed to be removed
        pentrisField.ClearFilledRows();
        clearTimer = std::numeric_limits<float>::max();
        //The field topology has changed, hence AI needs to recalculate its move
        recalculateAImove = true;
    }
    if ((aiLoop) && (recalculateAImove))
    {
        while (!pentrisAI.AIThreadJoined()) //If the AI is currently calculating its move, then interrupt and wait for it to finish
            pentrisAI.interrupt = true;
        //Calculate new move
        pentrisAI.CalculateMoveSequence(pentrisField);
        aiCalcStarted = timeElapsed;
    }
}

/*Update and draw the background star effect*/
void PentrisGame::DrawStars(float fElapsedTime)
{
    for (auto& star : stars)
    {
        star.distance += star.speed * fElapsedTime * (star.distance / 100.0f);
        if (star.distance > 800.0f) {
            star.angle = Random(0.0f, 2.0f * 3.1459f);
            star.speed = Random(10.0f, 100.0f);
            star.distance = Random(20.0f, 200.0f);
            float lum = Random(0.3f, 1.0f);
            star.col = olc::PixelF(lum, lum, lum, 1.0f);
        }
        Draw(olc::vf2d(cos(star.angle), sin(star.angle)) * star.distance + origin, star.col * (star.distance / 100.0f));
        FillCircle(olc::vf2d(cos(star.angle), sin(star.angle)) * star.distance + origin, 1, star.col * (star.distance / 100.0f));
    }
}

/*Draws the game field, the current and next pentomino, the strings on the sidebar, and the background star effect*/
void PentrisGame::DrawHandling(float fElapsedTime)
{
    Clear(olc::BLACK);
    DrawStars(fElapsedTime);
    DrawField();
    FillCircle(X_OFFSET + (pentrisField.pentominoX + pentrisField.PENTOMINO_WIDTH / 2) * PIXELS_PER_UNIT + PIXELS_PER_UNIT / 2, 10, 5, olc::WHITE);
    //Draw "Shadow" of the current pentomino at the target destination
    DrawPentomino(pentrisField.currentPentomino, pentrisField.pentominoX, terminalY, false, olc::VERY_DARK_GREY);
    DrawPentomino(pentrisField.currentPentomino, pentrisField.pentominoX, pentrisField.pentominoY, true, 0);
    DrawString(X_OFFSET + pentrisField.Width() * PIXELS_PER_UNIT + 20, 40, "NEXT");
    DrawPentomino(pentrisField.nextPentomino, pentrisField.Width() + 1, 2, true, 0);
    DrawString(X_OFFSET + pentrisField.Width() * PIXELS_PER_UNIT + 20, 240, "Score: " + std::to_string(score));
    DrawString(X_OFFSET + pentrisField.Width() * PIXELS_PER_UNIT + 20, 260, "Lines: " + std::to_string(linesFilled));
    DrawString(X_OFFSET + pentrisField.Width() * PIXELS_PER_UNIT + 20, 280, "Games: " + std::to_string(games));
    if (games > 0)
        DrawString(X_OFFSET + pentrisField.Width() * PIXELS_PER_UNIT + 20, 300, "Avg lines per game: " + std::to_string(linesFilledCumulative / games));
    if (gameOver)
        DrawString(X_OFFSET + (pentrisField.Width() / 2 - 2) * PIXELS_PER_UNIT, (pentrisField.Height() / 2) * PIXELS_PER_UNIT, "GAME OVER", olc::WHITE, 2);
    DrawString(X_OFFSET + pentrisField.Width() * PIXELS_PER_UNIT + 20, 340, "CONTROLS");
    DrawString(X_OFFSET + pentrisField.Width() * PIXELS_PER_UNIT + 20, 360, "Left/Right/Down Arrow: Move");
    DrawString(X_OFFSET + pentrisField.Width() * PIXELS_PER_UNIT + 20, 380, "C: Rotate");
    DrawString(X_OFFSET + pentrisField.Width() * PIXELS_PER_UNIT + 20, 400, "V: Reflect");
    DrawString(X_OFFSET + pentrisField.Width() * PIXELS_PER_UNIT + 20, 420, "Space: Drop");
    DrawString(X_OFFSET + pentrisField.Width() * PIXELS_PER_UNIT + 20, 440, "Enter: New Game");
    DrawString(X_OFFSET + pentrisField.Width() * PIXELS_PER_UNIT + 20, 480, "A: Let the machine play");
    DrawString(X_OFFSET + pentrisField.Width() * PIXELS_PER_UNIT + 20, 500, "  O: Slow down");
    DrawString(X_OFFSET + pentrisField.Width() * PIXELS_PER_UNIT + 20, 520, "  P: Speed up");
}

float PentrisGame::Random(float a, float b)
{
    return (b - a) * (float(rand()) / float(RAND_MAX)) + a;
}

/*Draws the game field (current falling pentomino and next pentomino need to be drawn separately)*/
void PentrisGame::DrawField()
{
    for (int i = 0; i < pentrisField.Width(); i++)
        for (int j = 0; j < pentrisField.Height(); j++)
            if (pentrisField(i,j) != 0)
            {
                float alpha;
                //Ensure flashing effect for rows that have just been filled
                if (pentrisField(i, j) == pentrisField.FILLEDROW)
                    alpha = 1.0*((int)(timeElapsed*100) % 8 < 4);
                else alpha = 1.0;
                FillRect(X_OFFSET + i * PIXELS_PER_UNIT, Y_OFFSET + j * PIXELS_PER_UNIT, PIXELS_PER_UNIT, PIXELS_PER_UNIT, PENTOMINO_COLORMAP[pentrisField(i, j)] * alpha);
                DrawRect(X_OFFSET + i * PIXELS_PER_UNIT, Y_OFFSET + j * PIXELS_PER_UNIT, PIXELS_PER_UNIT, PIXELS_PER_UNIT, olc::VERY_DARK_GREY);
            }
}

/*Draws a pentomino at the posX and posY (top left coordinates) point of the field.
  If useColormap == false, then the argument color is used to fill the pentomino*/
void PentrisGame::DrawPentomino(const std::vector<int> pentomino, const int posX, const int posY, bool useColormap, olc::Pixel color)
{
    if (pentomino.size() < pentrisField.PENTOMINO_WIDTH * pentrisField.PENTOMINO_WIDTH)
        return;
    for (int i = 0; i < pentrisField.PENTOMINO_WIDTH; i++)
        for (int j = 0; j < pentrisField.PENTOMINO_WIDTH; j++)
            if (pentomino[i + j * pentrisField.PENTOMINO_WIDTH] != 0)
            {
                if (useColormap)
                    FillRect(X_OFFSET + (i + posX) * PIXELS_PER_UNIT, Y_OFFSET + (j + posY) * PIXELS_PER_UNIT, PIXELS_PER_UNIT, PIXELS_PER_UNIT, PENTOMINO_COLORMAP[pentomino[i + j * pentrisField.PENTOMINO_WIDTH]]);
                else
                    FillRect(X_OFFSET + (i + posX) * PIXELS_PER_UNIT, Y_OFFSET + (j + posY) * PIXELS_PER_UNIT, PIXELS_PER_UNIT, PIXELS_PER_UNIT, color);
                DrawRect(X_OFFSET + (i + posX) * PIXELS_PER_UNIT, Y_OFFSET + (j + posY) * PIXELS_PER_UNIT, PIXELS_PER_UNIT, PIXELS_PER_UNIT, olc::VERY_DARK_GREY);
            }
}

void PentrisGame::NewGame()
{
    pentrisField.Reset();
    score = 0;
    linesFilled = 0;
    pieceCount = 1;
    gameOver = false;
    moveTimer = 0.0f;
    fallAfterSeconds = 2.5f;
    fallTimer = fallAfterSeconds;
    clearTimer = std::numeric_limits<float>::max();
}

bool PentrisGame::OnUserCreate()
{
    sAppName = "Pentomino Puzzle";
    srand((unsigned int)time(NULL));
    stars.resize(starCount);
    for (auto& star : stars)
    {
        star.angle = Random(0.0f, 2.0f * 3.1459f);
        star.speed = Random(10.0f, 100.0f);
        star.distance = Random(20.0f, 200.0f);
        float lum = Random(0.3f, 1.0f);
        star.col = olc::PixelF(lum, lum, lum, 1.0f);
    }
    origin = { float(ScreenWidth() / 2), float(ScreenHeight() / 2) };
    return true;
}

bool PentrisGame::OnUserUpdate(float fElapsedTime)
{
    terminalY = pentrisField.GetTerminalY();
    UserInputHandling(fElapsedTime);
    AIInputHandling(fElapsedTime);
    PentominoMovementHandling(fElapsedTime);

    timeElapsed += fElapsedTime;

    DrawHandling(fElapsedTime);

    return true;
}

PentrisGame::~PentrisGame()
{
    while (!pentrisAI.AIThreadJoined())
        pentrisAI.interrupt = true;
}
