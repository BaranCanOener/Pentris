#define OLC_PGE_APPLICATION

#include <iostream>
#include "PentrisGame.h"


int main()
{

    PentrisGame game;
    if (game.Construct(700, 750, 1, 1))
    {
        game.Start();
    }

}