#include "Log.h"
#include "Constants.h"
#include "Game.h"
#include <iostream>

int main(int argc, char* args[])
{
    InitLogger();

    Game* pGame = new Game();
    std::cout << "Game is running..." << std::endl;

    pGame->Initialize(WINDOW_WIDTH, WINDOW_HEIGHT);

    while (pGame->IsRunning())
    {
        pGame->ProcessInput();
        pGame->Update();
        pGame->Render();
    }

    pGame->Destroy();
    CloseLogger();

    return 0;
}
