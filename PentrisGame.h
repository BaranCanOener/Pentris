#pragma once

#include "olcPixelGameEngine.h"
#include "PentrisField.h"
#include "PentrisAI.h"

struct Star {
    float angle = 0.0f;
    float distance = 0.0f;
    float speed = 0.0f;
    olc::Pixel col = olc::WHITE;
};

class PentrisGame : public olc::PixelGameEngine
{
private:

    //The game field
    PentrisField pentrisField;

    /*AI VARIABLES*/
    PentrisAI pentrisAI;
    //The AI is playing if "true"
    bool aiLoop = false;
    //The amount of seconds between AI moves
    float aiMoveAfterSeconds = 0.0f;
    //Stores a countdown from aiMoveAfterSeconds to 0 - at 0, the AI is allowed to play
    float aiMoveTimer = aiMoveAfterSeconds;
    //Caps the AI calculation time in seconds
    float aiCalcCutoff = 2.0f;
    //The point at which the AI started calculating (in seconds after the application started)
    float aiCalcStarted;

    /*PLAYER INPUT VARIABLES*/
    //If the user keeps left/right/down pressed, then the pentomino only moves every moveAfterSeconds (to prevent near instantaneous jumps to the border at a high framerate)
    float moveAfterSeconds = 0.08;
    //Stores a countdown from moveAfterSeconds to 0 - at 0, the pentomino is moved if the player holds down a key
    float moveTimer = 0.0f;

    /*PENTOMINO MOVEMENT VARIABLES*/
    //The amount of seconds after which the pentomino falls down
    float fallAfterSeconds = 2.5;
    //Stores a countdown from fallAfterSeconds to 0 - at 0, the pentomino is moved down. Can hence be set to 0 to force a fall on the current frame
    float fallTimer = fallAfterSeconds;
    //How many seconds a filled row is flashing for
    float clearLinesAfterSeconds = 0.5f;
    //Once a row is filled, then this variable stores a countdown from clearLinesAfterSeconds to 0. If 0 is reached, the game loop will clear filled lines
    float clearTimer = std::numeric_limits<float>::max();
    //Used to store the terminal y-position of the currently falling pentomino
    int terminalY;
    float timeElapsed = 0;

    /*GAME STATS*/
    //Number of pieces delivered to the player
    int pieceCount = 1;
    int score = 0;
    int scoreCumulative = 0;
    int linesFilled = 0;
    //Lines filled over all games
    int linesFilledCumulative = 0;
    int games = 0;
    bool gameOver = false;

    /*DRAWING VARIABLES AND CONSTANTS*/
    const int starCount = 500;
    std::vector<Star> stars;
    olc::vf2d origin;
    //How many pixels a single block is tall and wide
    const int PIXELS_PER_UNIT = 20;
    //The top left coordinate of the playing field
    const int X_OFFSET = 50;
    const int Y_OFFSET = 20;
    //Each Pentomino is identified by an integer 1-12, and each such integer corresponds to a color in which the pentomino is rendered. 13 is reserved for walls, and 14 for a flashing effect
    std::map<int, olc::Pixel> PENTOMINO_COLORMAP{
        {0, olc::BLACK},
        {1, olc::RED},
        {2, olc::GREEN},
        {3, olc::YELLOW},
        {4, olc::CYAN},
        {5, olc::DARK_MAGENTA},
        {6, olc::BLUE},
        {7, olc::DARK_GREEN},
        {8, olc::DARK_BLUE},
        {9, olc::DARK_YELLOW},
        {10, olc::Pixel(128,0,128)},
        {11, olc::Pixel(216,191,216)},
        {12, olc::DARK_CYAN},
        {13, olc::DARK_GREY},
        {14, olc::WHITE}
    };

    void DrawStars(float fElapsedTime);
    void UserInputHandling(float fElapsedTime);
    void AIInputHandling(float fElapsedTime);
    void PentominoMovementHandling(float fElapsedTime);
    void DrawHandling(float fElapsedTime);
public:
    float Random(float a, float b);
    void DrawField();
    void DrawPentomino(const std::vector<int> pentomino, const int posX, const int posY, bool useColormap, olc::Pixel color);

    void NewGame();

    bool OnUserCreate() override;
    bool OnUserUpdate(float fElapsedTime) override;
    ~PentrisGame();
};

